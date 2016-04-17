#ifndef __CODEC_WRAPPER_HXX__
#define __CODEC_WRAPPER_HXX__

#include "Codec.hxx"

#include <vector>

class CodecWrapper : public Codec {
public:
    CodecWrapper(PacketLossSimulator* pls);
    virtual ~CodecWrapper(void);
    const char* getName(void) const;
    virtual void init(void);
    virtual void clear(void);
    virtual void setHz(int hz);
    virtual void setWAVFmtType(int type);
    virtual int getFrameSize(void);
    virtual int encode(const int16_t* src, int srclen, char* dst);
    virtual int decode(const char* src, int srclen, int16_t* dst);
    virtual int codec(const int16_t* src, int srclen, int16_t* dst, int& bps);
    void addCodec(Codec* codec);
    void setCodec(int num);
    int getCurCodecNum(void) const { return curCodecNum; };
    int getMaxCodec(void) const { return (int)codecs.size(); };
    const char* getCodecName(int num) const;
private:
    Codec* curCodec;
    int curCodecNum;

    std::vector<Codec*> codecs;
};

#endif // __CODEC_WRAPPER_HXX__
