#ifndef __NULL_CODEC_HXX__
#define __NULL_CODEC_HXX__

#include "Codec.hxx"

class NullCodec : public Codec {
public:
    NullCodec(void);
    virtual ~NullCodec(void) {};
    const char* getName(void) const;
    virtual void init(void);
    virtual void clear(void);
    virtual void setHz(int hz) {
        this->hz = hz;
        frame_size = hz / 50;   // 20ms
    };
    virtual void setWAVFmtType(int type) { this->type = type; };
    virtual int getFrameSize(void);
    virtual int encode(const int16_t* src, int srclen, char* dst);
    virtual int decode(const char* src, int srclen, int16_t* dst);
    virtual int codec(const int16_t* src, int srclen, int16_t* dst, int& bps);
private:
    int hz;
    int type;
    int frame_size;
};

#endif // __NULL_CODEC_HXX__
