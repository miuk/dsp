#include "AudioSource.hxx"

#include <iostream>
using namespace std;

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

AudioSource::AudioSource(void)
{
    buf = NULL;
    len = 0;
    pos = NULL;
}

AudioSource::~AudioSource(void)
{
    delete[] buf;
}

bool
AudioSource::atEnd() const
{
    cout << "atEnd" << endl;
    if (buf == NULL)
        return true;
    if ((size_t)(pos - buf) >= len)
        return true;
    return false;
}

qint64
AudioSource::bytesAvailable() const
{
    cout << "bytesAvailable" << endl;
    if (buf == NULL)
        return 0;
    return rest + QIODevice::bytesAvailable();
}

bool
AudioSource::canReadLine() const
{
    cout << "canReadLine" << endl;
    return false;
}

void
AudioSource::close(void)
{
    clear();
};

bool
AudioSource::isSequential() const
{
    cout << "isSequential" << endl;
    return true;
}

bool
AudioSource::open(OpenMode mode)
{
    if (mode != QIODevice::ReadOnly)
        return false;
    QIODevice::open(mode);
    return openFile();
}

bool
AudioSource::reset()
{
    if (buf == NULL)
        return false;
    pos = buf;
    rest = len;
    return true;
}

bool
AudioSource::seek(qint64 pos)
{
    cout << "seek " << pos << endl;
    if (pos < 0)
        return false;
    QIODevice::seek(pos);
    if ((size_t)pos > len)
        return false;
    this->pos = buf + pos;
    return true;
}

qint64
AudioSource::size() const
{
    return bytesAvailable();
}

bool
AudioSource::waitForReadyRead(int)
{
    cout << "waitForReadyRead" << endl;
    return false;
}

qint64
AudioSource::writeData(const char*, qint64)
{
    return 0;
};

qint64
AudioSource::readData(char* data, qint64 maxSize)
{
    cout << "readData " << maxSize << endl;
    size_t rlen = maxSize;
    if ((size_t)maxSize > rest)
        rlen = rest;
    memcpy(data, pos, rlen);
    pos += rlen;
    rest -= rlen;
    cout << "readData maxSize=" << maxSize
         << ", rlen=" << rlen
         << ", rest=" << rest
         << endl;
    return rlen;
};

bool
AudioSource::openFile(void)
{
    clear();
    int fd = ::open(fname.toStdString().c_str(), O_RDONLY);
    if (fd < 0)
        return false;
    struct stat st;
    fstat(fd, &st);
    len = st.st_size;
    buf = new char[len];
    ::read(fd, buf, len);
    ::close(fd);
    pos = buf;
    rest = len;
    return true;
}

void
AudioSource::clear(void)
{
    delete[] buf;
    buf = NULL;
    len = 0;
    pos = NULL;
}
