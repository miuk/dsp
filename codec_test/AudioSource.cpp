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
    codec = NULL;
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
    if (codec)
        codec->clear();
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

size_t
AudioSource::procData(char* dst, size_t len)
{
    if (codec == NULL) {
        memcpy(dst, pos, len);
        return len;
    }
    size_t rlen = codec->codec((int16_t*)pos, len/2, (int16_t*)dst);
    rlen *= 2;
    return rlen;
}

size_t
AudioSource::adjustReadLen(qint64 maxSize)
{
    if (codec == NULL)
        return maxSize;
    int frame_size = codec->getFrameSize();
    return (maxSize / frame_size) * frame_size;
}

qint64
AudioSource::readData(char* data, qint64 maxSize)
{
    size_t rlen = adjustReadLen(maxSize);
    if ((size_t)rlen > rest)
        rlen = rest;
    rlen = procData(data, rlen);
    if (rlen <= rest) {
        pos += rlen;
        rest -= rlen;
    } else {
        pos += rest;
        rest = 0;
    }
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
    if (codec)
        codec->clear();
}
