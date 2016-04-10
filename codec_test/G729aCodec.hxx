#ifndef __G729A_CODEC_HXX__
#define __G729A_CODEC_HXX__

#include "Codec.hxx"

class G729aCodec : public Codec {
public:
    G729aCodec(void);
    virtual ~G729aCodec(void);
    const char* getName(void) const;
    virtual void init(void);
    virtual void clear(void);
    virtual void setHz(int) {};
    virtual void setWAVFmtType(int ) {};
    virtual int getFrameSize(void) { return 80; };
    virtual int encode(const int16_t* src, int srclen, char* dst);
    virtual int decode(const char* src, int srclen, int16_t* dst);
    virtual int codec(const int16_t* src, int srclen, int16_t* dst, int& bps);
private:
};

#endif // __G729A_CODEC_HXX__
