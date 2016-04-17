#include "ULawCodec.hxx"
#include "ulaw.h"

#include <assert.h>
#include <string.h>
#include <iostream>
using namespace std;

const char*
ULawCodec::getName(void) const
{
    static const char* name = "ulaw";
    return name;
}

int
ULawCodec::getFrameSize(void)
{
    int ptime = pls->getPtime();
    return frame_size * (ptime / 20);
}

int
ULawCodec::encode(const int16_t* src, int srclen, char* dst)
{
    const int16_t* src_pos = src;
    uint8_t* dst_pos = (uint8_t*)dst;
    for (int i = 0; i < srclen; i++) {
        *dst_pos = linear2ulaw(*src_pos);
        src_pos++;
        dst_pos++;
    }
    return srclen;
}

int
ULawCodec::decode(const char* src, int srclen, int16_t* dst)
{
    const uint8_t* src_pos = (const uint8_t*)src;
    int16_t* dst_pos = dst;
    for (int i = 0; i < srclen; i++) {
        *dst_pos = ulaw2linear(*src_pos);
        src_pos++;
        dst_pos++;
    }
    return srclen;
}

int
ULawCodec::codec(const int16_t* src, int srclen, int16_t* dst, int& bps)
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
        else {
            for (int j = 0; j < frame_size; j++) {
                uint8_t u = linear2ulaw(src_pos[j]);
                dst_pos[j] = ulaw2linear(u);
            }
        }
        src_pos += frame_size;
        dst_pos += frame_size;
    }
    bps = hz * 8;
    return srclen;
}
