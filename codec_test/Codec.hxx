
#ifndef __CODEC_HXX__
#define __CODEC_HXX__

#include <stdint.h>

class Codec {
public:
    virtual ~Codec(void) {};
    virtual const char* getName(void) const = 0;
    virtual void init(void) = 0;
    virtual void clear(void) = 0;
    virtual void setHz(int hz) = 0;
    virtual void setWAVFmtType(int type) = 0;
    virtual int getFrameSize(void) = 0;
    virtual int encode(const int16_t* src, int srclen, char* dst) = 0;
    virtual int decode(const char* src, int srclen, int16_t* dst) = 0;
    virtual int codec(const int16_t* src, int srclen, int16_t* dst, int& bps) = 0;
};

#endif // __CODEC_HXX__
