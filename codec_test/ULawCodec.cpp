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
    memcpy(dst, src, sizeof(int16_t) * srclen);
    bps = hz * 8;
    return srclen;
}
