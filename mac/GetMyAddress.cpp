#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/sockio.h>
#include <net/if.h>
#include <errno.h>
#include <ifaddrs.h>

#include <string>
#include <vector>
#include <iostream>
using namespace std;

class MyAddress {
public:
    MyAddress(void) {
        family = AF_UNSPEC;
        bUp = false;
        bLinkLocal = false;
        bLoopback = false;
    }
    MyAddress(const MyAddress& src) { *this = src; };
    MyAddress& operator=(const MyAddress& src) {
        ifname = src.ifname;
        family = src.family;
        address = src.address;
        bUp = src.bUp;
        bLinkLocal = src.bLinkLocal;
        bLoopback = src.bLoopback;
        return *this;
    };

    void print(ostream& out) const {
        out << "ifname=" << ifname
            << ", family=" << family
            << ", address=" << address
            << ", bUp=" << bUp
            << ", bLinkLocal=" << bLinkLocal
            << ", bLoopback=" << bLoopback
            << endl;
    };

public:
    string ifname;
    int family;
    string address;
    bool bUp;
    bool bLinkLocal;
    bool bLoopback;
};

bool
getMyAddress2(vector<MyAddress>& addrs)
{
    struct ifaddrs* ifa;
    const struct ifaddrs* cursor;
    if (getifaddrs(&ifa) != 0)
        return false;
    for (const struct ifaddrs* cursor = ifa;
         cursor != NULL; cursor = cursor->ifa_next) {
        if (cursor->ifa_addr->sa_family != AF_INET &&
            cursor->ifa_addr->sa_family != AF_INET6)
            continue;
        char host[NI_MAXHOST];
        getnameinfo((struct sockaddr*)cursor->ifa_addr,
                    cursor->ifa_addr->sa_len,
                    host, sizeof(host),
                    NULL, 0, NI_NUMERICHOST);
        MyAddress addr;
        char* cptr = strchr(cursor->ifa_name, ':');
        if (cptr != NULL)
            *cptr = '\0';
        addr.ifname = string(cursor->ifa_name);
        addr.family = cursor->ifa_addr->sa_family;
        addr.address = string(host);
        addr.bUp = (cursor->ifa_flags & IFF_UP);
        addr.bLoopback = (cursor->ifa_flags & IFF_LOOPBACK);
        if (cursor->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6* sin = (struct sockaddr_in6*)cursor->ifa_addr;
            addr.bLinkLocal = IN6_IS_ADDR_LINKLOCAL(&sin->sin6_addr);
        }
        addrs.push_back(addr);
    }
    freeifaddrs(ifa);
    return true;
}

bool
getMyAddress(vector<MyAddress>& addrs)
{
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return false;
    }

    char buf[BUFSIZ];
    struct ifconf ifc;
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
        return false;
    }

    for (char* ptr = buf; ptr < buf + ifc.ifc_len;) {
        struct ifreq* ifr = (struct ifreq*)ptr;
        ptr += sizeof(ifr->ifr_name) + ifr->ifr_addr.sa_len;
        if (ifr->ifr_addr.sa_family != AF_INET &&
            ifr->ifr_addr.sa_family != AF_INET6)
            continue;
        char host[NI_MAXHOST];
        getnameinfo((struct sockaddr*)&ifr->ifr_addr, ifr->ifr_addr.sa_len,
                    host, sizeof(host),
                    NULL, 0, NI_NUMERICHOST);
        MyAddress addr;
        char* cptr = strchr(ifr->ifr_name, ':');
        if (cptr != NULL)
            *cptr = '\0';
        addr.ifname = string(ifr->ifr_name);
        addr.family = ifr->ifr_addr.sa_family;
        addr.address = string(host);
        struct ifreq ifr_copy = *ifr;
        if (ioctl(sock, SIOCGIFFLAGS, &ifr_copy) == 0) {
            addr.bUp = (ifr_copy.ifr_flags & IFF_UP);
            addr.bLoopback = (ifr_copy.ifr_flags & IFF_LOOPBACK);
        }
        if (ifr->ifr_addr.sa_family == AF_INET6) {
            struct sockaddr_in6* sin = (struct sockaddr_in6*)&ifr->ifr_addr;
            addr.bLinkLocal = IN6_IS_ADDR_LINKLOCAL(&sin->sin6_addr);
        }
        addrs.push_back(addr);
    }

    close(sock);
    return true;
}

int
main(int ac, char* av[])
{
    vector<MyAddress> addrs;
    getMyAddress2(addrs);
    for (vector<MyAddress>::const_iterator i = addrs.begin();
         i != addrs.end(); i++) {
        const MyAddress& addr = *i;
        addr.print(cout);
    }
}
