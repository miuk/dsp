
#ifndef __CODEC_HXX__
#define __CODEC_HXX__

#include <stdint.h>

#include "PacketLossSimulator.hxx"

class Codec {
public:
    Codec(void) { pls = 0; };
    void setPLS(PacketLossSimulator* pls) { this->pls = pls; };
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
protected:
    PacketLossSimulator* pls;
};

#endif // __CODEC_HXX__
