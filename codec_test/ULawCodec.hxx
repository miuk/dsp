
#ifndef __ULAW_CODEC_HXX__
#define __ULAW_CODEC_HXX__

#include "Codec.hxx"

class ULawCodec : public Codec {
public:
    ULawCodec(void) { hz = 8000; };
    const char* getName(void) const;
    virtual void init(void) {};
    virtual void clear(void) {};
    virtual void setHz(int hz) { this->hz = hz; };
    virtual void setWAVFmtType(int) {};
    virtual int getFrameSize(void) { return 1; };
    virtual int encode(const int16_t* src, int srclen, char* dst);
    virtual int decode(const char* src, int srclen, int16_t* dst);
    virtual int codec(const int16_t* src, int srclen, int16_t* dst, int& bps);

private:
    int hz;
};

#endif // __SPEEX_CODEC_HXX__
