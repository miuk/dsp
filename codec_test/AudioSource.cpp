#include "AudioSource.hxx"
#include "ulaw.h"

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
    bps = 0;
    codec = NULL;
    codec_rest = NULL;
    codec_rest_len = 0;
}

AudioSource::~AudioSource(void)
{
    delete[] buf;
    delete[] codec_rest;
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
        if (rest < len)
            len = rest;
        memcpy(dst, pos, len);
        pos += len;
        rest -= len;
        return len;
    }
    size_t last_len = 0;
    if (codec_rest_len > 0) {
        memcpy(dst, codec_rest, codec_rest_len);
        last_len = codec_rest_len;
        dst += codec_rest_len;
        len -= codec_rest_len;
        delete[] codec_rest;
        codec_rest = NULL;
        codec_rest_len = 0;
    }
    if (rest < len)
        len = rest;
    int frame_size = codec->getFrameSize() * 2;
    size_t rlen = (len / frame_size) * frame_size;
    if ((int)rlen < frame_size) {
        pos += rest;
        rest = 0;
        return last_len;
    }
    if (rlen < len)
        rlen += frame_size;
    if (rest < rlen)
        rlen -= frame_size;
    if (rlen > len) {
        char buf[rlen];
        codec->codec((int16_t*)pos, rlen/2, (int16_t*)buf, bps);
        memcpy(dst, buf, len);
        codec_rest_len = rlen - len;
        codec_rest = new char[codec_rest_len];
        memcpy(codec_rest, buf + len, codec_rest_len);
    } else {
        codec->codec((int16_t*)pos, rlen/2, (int16_t*)dst, bps);
    }    
    last_len += len;
    pos += rlen;
    rest -= rlen;
    return last_len;
}

size_t
AudioSource::adjustReadLen(qint64 maxSize)
{
    if (codec == NULL)
        return maxSize;
    int frame_size = codec->getFrameSize() * 2;
    return (maxSize / frame_size) * frame_size;
}

qint64
AudioSource::readData(char* data, qint64 maxSize)
{
    if (maxSize <= 0) {
        return 0;
    }
    size_t rlen = procData(data, maxSize);
    return rlen;
};

bool
AudioSource::openFile(void)
{
    clear();
    WAV wav;
    string errmsg;
    if (wav.open(fname.toStdString(), errmsg) == false) {
        return false;
    }
    fmt = wav.getFmt();
    cout << "type=" << fmt.type
         << ", block_size=" << fmt.block_size
         << ", data_bytes=" << wav.getDataBytes()
         << endl;
    if (fmt.nch != 1) {
        cout << "not mono" << endl;
        wav.close(errmsg);
        return false;
    }
    bool ret = false;
    switch (fmt.type) {
    case WAVFmt::Type_pcm:
        ret = read_linear(wav, errmsg);
        break;
    case WAVFmt::Type_mulaw :
        ret = read_ulaw(wav, errmsg);
        break;
    default :
        errmsg = "unsupported type";
        break;
    }
    wav.close(errmsg);
    if (ret == false)
        return false;
    pos = buf;
    rest = len;
    cout << "read len=" << len
         << ", hz=" << getHz()
         << endl;
    if (codec != NULL) {
        codec->setWAVFmtType(fmt.type);
        codec->setHz(getHz());
    }
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
    delete[] codec_rest;
    codec_rest = NULL;
    codec_rest_len = 0;
}

bool
AudioSource::read_ulaw(WAV& wav, string& errmsg)
{
    if (fmt.bits_per_sample != 8) {
        errmsg = "ulaw bits must be 8";
        return false;
    }
    int srclen = wav.getDataBytes();
    char tmp[srclen];
    wav.read(tmp, srclen);
    len = srclen * 2;
    buf = new char[len];
    uint8_t* src_pos = (uint8_t*)tmp;
    int16_t* dst_pos = (int16_t*)buf;
    for (int i = 0; i < srclen; i++) {
        *dst_pos = ulaw2linear(*src_pos);
        dst_pos++;
        src_pos++;
    }
    return true;
}

bool
AudioSource::read_linear(WAV& wav, string& errmsg)
{
    if (fmt.bits_per_sample != 16) {
        errmsg = "pcm bits must be 16";
        return false;
    }
    len = wav.getDataBytes();
    buf = new char[len];
    wav.read(buf, len);
    return true;
}

QString
AudioSource::getTypeStr(void) const
{
    switch (fmt.type) {
    case WAVFmt::Type_pcm:
        return "PCM";
    case WAVFmt::Type_mulaw :
        return "u-law";
    default :
        return "unknown";
        break;
    }
}

int
AudioSource::getLengthSec(void) const
{
    return len / sizeof(int16_t) / fmt.samples_per_sec;
}
