#include "NullCodec.hxx"
#include "wav.hxx"

#include <string.h>
NullCodec::NullCodec(void)
{
    hz = 0;
    type = WAVFmt::Type_pcm;
}

const char*
NullCodec::getName(void) const
{
    static const char* name = "Null";
    return name;
}

void
NullCodec::init(void)
{
}

void
NullCodec::clear(void)
{
}

int
NullCodec::encode(const int16_t* src, int srclen, char* dst)
{
    int dstlen = sizeof(*src) * srclen;
    memcpy(dst, src, dstlen);
    return dstlen;
}

int
NullCodec::decode(const char* src, int srclen, int16_t* dst)
{
    int dstlen = srclen / sizeof(*dst);
    memcpy(dst, src, srclen);
    return dstlen;
}

int
NullCodec::codec(const int16_t* src, int srclen, int16_t* dst, int& bps)
{
    int len = sizeof(*src) * srclen;
    memcpy(dst, src, len);
    bps = hz * 8;
    if (type != WAVFmt::Type_mulaw)
        bps *= 2;
    return srclen;
}
