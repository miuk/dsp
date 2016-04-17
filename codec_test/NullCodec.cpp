#include "NullCodec.hxx"
#include "wav.hxx"

#include <string.h>
#include <iostream>
using namespace std;

NullCodec::NullCodec(void)
{
    hz = 0;
    type = WAVFmt::Type_pcm;
    frame_size = 0;
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

int
NullCodec::getFrameSize(void)
{
    int ptime = pls->getPtime();
    return frame_size * (ptime / 20);
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
    int n = srclen / frame_size;
    int len = sizeof(int16_t) * frame_size;
    const int16_t* src_pos = src;
    int16_t* dst_pos = dst;
    int nf = pls->getPtime() / 20;
    bool bLost = false;
    for (int i = 0; i < n; i++) {
        if (i % nf == 0)
            bLost = pls->isLost();
        if (bLost)
            memset(dst_pos, 0, len);
        else
            memcpy(dst_pos, src_pos, len);
        src_pos += frame_size;
        dst_pos += frame_size;
    }
    bps = hz * 8;
    if (type != WAVFmt::Type_mulaw)
        bps *= 2;
    return srclen;
}
