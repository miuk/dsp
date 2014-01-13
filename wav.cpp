#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "wav.hxx"

WAVFmt::WAVFmt(void)
{
    type = Type_unknown;
    nch = 0;
    samples_per_sec = 0;
    bytes_per_sec = 0;
    block_size = 0;
    bits_per_sample = 0;
}

WAVFmt&
WAVFmt::operator=(const WAVFmt& src)
{
    this->type = src.type;
    this->nch = src.nch;
    this->samples_per_sec = src.samples_per_sec;
    this->bytes_per_sec = src.bytes_per_sec;
    this->block_size = src.block_size;
    this->bits_per_sample = src.bits_per_sample;
    return *this;
}

bool
WAVFmt::read(int fd, string& errmsg)
{
    unsigned int len;
    if (::read(fd, &len, sizeof(len)) != sizeof(len)) {
        errmsg = "WAVFmt length too short";
        return false;
    }
    if (len > 65536) {
        errmsg = "WAVFmt length too long";
        return false;
    }
    char buf[len];
    if (::read(fd, buf, len) != len) {
        errmsg = "WAVFmt length mismatch";
        return false;
    }
    type = static_cast<Type>(*(short*)(buf + 0));
    nch = *(short*)(buf + 2);
    samples_per_sec = *(int*)(buf + 4);
    bytes_per_sec = *(int*)(buf + 8);
    block_size = *(short*)(buf + 12);
    bits_per_sample = *(short*)(buf + 14);
    return true;
}

bool
WAVFmt::write(int fd, string& errmsg) const
{
    unsigned int len = 16;
    char buf[len];
    *(short*)(buf + 0) = type;
    *(short*)(buf + 2) = nch;
    *(int*)(buf + 4) = samples_per_sec;
    *(int*)(buf + 8) = bytes_per_sec;
    *(short*)(buf + 12) = block_size;
    *(short*)(buf + 14) = bits_per_sample;
    if (::write(fd, &len, sizeof(len)) != sizeof(len)) {
        errmsg = "WAVFmt write failed";
        return false;
    }
    if (::write(fd, buf, len) != len) {
        errmsg = "WAVFmt write failed";
        return false;
    }
    return true;
}

WAVFmt::WAVFmt(Type type, int nch, int samples_per_sec, int bits_per_sample)
{
    this->type = type;
    this->nch = nch;
    this->samples_per_sec = samples_per_sec;
    this->bits_per_sample = bits_per_sample;
    int bytes_per_sample = bits_per_sample / 8;
    bytes_per_sec = nch * bytes_per_sample * samples_per_sec;
    block_size = nch * bytes_per_sample;
}

int
WAVFmt::calcBytes(int nsample) const
{
    int bytes_per_sample = bits_per_sample / 8;
    return bytes_per_sample * nch * nsample;
}

int
WAVFmt::calcSamples(int nbyte) const
{
    int bytes_per_sample = bits_per_sample / 8;
    return nbyte / (bytes_per_sample * nch);
}

WAV::WAV(void)
{
    bytes_data = 0;
    fd = -1;
    bWrite = false;
}

bool
WAV::open(const string& fname, string& errmsg)
{
    if (fd != -1) {
        errmsg = "already opened";
        return false;
    }
    fd = ::open(fname.c_str(), O_RDONLY);
    if (fd < 0) {
        errmsg = strerror(errno);
        return false;
    }
    char sbuf[4];
    if (::read(fd, sbuf, sizeof(sbuf)) != sizeof(sbuf)) {
        errmsg = "read RIFF header";
        goto err_end;
    }
    if (memcmp(sbuf, "RIFF", sizeof(sbuf)) != 0) {
        errmsg = "wrong RIFF header";
        goto err_end;
    }
    if (::read(fd, sbuf, sizeof(sbuf)) != sizeof(sbuf)) { // rest data size
        errmsg = "read total size";
        goto err_end;
    }
    if (::read(fd, sbuf, sizeof(sbuf)) != sizeof(sbuf)) {
        errmsg = "read WAVE header";
        goto err_end;
    }
    if (memcmp(sbuf, "WAVE", sizeof(sbuf)) != 0) {
        errmsg = "wrong WAVE header";
        goto err_end;
    }
    if (seek_to("fmt ") == false) {
        errmsg = "fmt chunk not found";
        goto err_end;
    }
    if (fmt.read(fd, errmsg) == false)
        goto err_end;
    if (seek_to("data") == false) {
        errmsg = "data chunk not found";
        goto err_end;
    }
    unsigned int len;
    if (read_length(len) == false) {
        errmsg = "read data length";
        return false;
    }
    bytes_data = len;

    return true;

err_end:
    ::close(fd);
    fd = -1;
    return false;
}

bool
WAV::seek_to(const string& chunk_name)
{
    for (;;) {
        char sbuf[4];
        if (::read(fd, sbuf, sizeof(sbuf)) != 4)
            return false;
        if (memcmp(chunk_name.c_str(), sbuf, sizeof(sbuf)) == 0)
            return true;
        unsigned int len;
        if (read_length(len) == false)
            return false;
        if (lseek(fd, len, SEEK_CUR) == -1)
            return false;
    }
}

bool
WAV::read_length(unsigned int& len)
{
    len = 0;
    if (::read(fd, &len, sizeof(len)) != sizeof(len))
        return false;
    return true;
}

bool
WAV::open(const string& fname, const WAVFmt& fmt, string& errmsg)
{
    if (fd != -1) {
        errmsg = "already opened";
        return false;
    }
    this->fmt = fmt;
    fd = ::open(fname.c_str(), O_RDWR|O_CREAT, 0664);
    if (fd < 0) {
        errmsg = strerror(errno);
        return false;
    }
    if (::write(fd, "RIFF", 4) != 4) {
        errmsg = "write RIFF header";
        goto err_end;
    }
    char sbuf[4];
    *(int*)sbuf = 0;
    if (::write(fd, sbuf, sizeof(sbuf)) != sizeof(sbuf)) { // dummy
        errmsg = "write total length";
        goto err_end;
    }
    if (::write(fd, "WAVE", 4) != 4) {
        errmsg = "write WAVE header";
        goto err_end;
    }
    if (::write(fd, "fmt ", 4) != 4) {
        errmsg = "write fmt header";
        goto err_end;
    }
    if (fmt.write(fd, errmsg) == false)
        goto err_end;
    if (::write(fd, "data", 4) != 4) {
        errmsg = "write data header";
        goto err_end;
    }
    if (::write(fd, sbuf, sizeof(sbuf)) != sizeof(sbuf)) { // dummy
        errmsg = "write data length";
        goto err_end;
    }
    bWrite = true;
    return true;

err_end:
    ::close(fd);
    fd = -1;
    return false;
}

bool
WAV::close(string& errmsg)
{
    if (fd == -1) {
        errmsg = "not opened";
        return false;
    }
    if (bWrite == false) {
        ::close(fd);
        fd = -1;
        return true;
    }
    if (lseek(fd, 4, SEEK_SET) == -1) {
        errmsg = "failed to seek to top";
        goto err_end;
    }
    char sbuf[4];
    *(int*)sbuf = bytes_data + 4 + 4 + 4 + 16 + 4 + 4;
    if (::write(fd, sbuf, sizeof(sbuf)) != sizeof(sbuf)) {
        errmsg = "write total length";
        goto err_end;
    }
    if (lseek(fd, 4, SEEK_CUR) == -1) {
        errmsg = "failed to seek to fmt chunk";
        goto err_end;
    }
    if (seek_to("data") == false) {
        errmsg = "failed to seek to data chunk";
        goto err_end;
    }
    *(int*)sbuf = bytes_data;
    if (::write(fd, sbuf, sizeof(sbuf)) != sizeof(sbuf)) {
        errmsg = "write data length";
        goto err_end;
    }
    ::close(fd);
    fd = -1;
    return true;

err_end:
    ::close(fd);
    fd = -1;
    return false;
}

int
WAV::read(char* buf, int nsample)
{
    int nbyte = fmt.calcBytes(nsample);
    int nread = ::read(fd, buf, nbyte);
    return nread;
}

int
WAV::write(const char* buf, int nsample)
{
    int nbyte = fmt.calcBytes(nsample);
    int nwrite = ::write(fd, buf, nbyte);
    if (nwrite >= 0)
        bytes_data += nwrite;
    return nwrite;
}
