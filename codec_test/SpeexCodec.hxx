
#ifndef __SPEEX_CODEC_HXX__
#define __SPEEX_CODEC_HXX__

#include "Codec.hxx"
#include "pthread.h"
#include <speex/speex.h>

class SpeexCodec : public Codec {
public:
    SpeexCodec(void);
    virtual ~SpeexCodec(void) {
        pthread_mutex_destroy(&mutex);
    };
    const char* getName(void) const;
    virtual void init(void);
    virtual void clear(void);
    virtual void setHz(int hz);
    virtual void setWAVFmtType(int) {};
    virtual int getFrameSize(void);
    virtual int encode(const int16_t* src, int srclen, char* dst);
    virtual int decode(const char* src, int srclen, int16_t* dst);
    virtual int codec(const int16_t* src, int srclen, int16_t* dst, int& bps);

    void setQuality(int value);
    void setXBR(int value);
    void setVAD(bool value);
    void setDTX(bool value);
    void setENH(bool value);
    void setPLC(int percent);
    bool getVAD(void) const { return vad; };
    bool getDTX(void) const { return dtx; };
    bool getENH(void) const { return enh; };
    int getPLC(void) const { return plc_tuning; };
private:
    void initXBR(int value);
    void initOptions(void);
private:
    void* enc_state;
    SpeexBits enc_bits;
    void* dec_state;
    SpeexBits dec_bits;
    int frame_size;

    int hz;
    int quality;
    int xBR;

    // encoder options
    bool vad; // voice activity detection, default is ?
    bool dtx; // discontinuous transmission, default is ?
    int plc_tuning;
    // decoder options
    bool enh; // perceptual enhancer, default is true

    pthread_mutex_t mutex;
};

#endif // __SPEEX_CODEC_HXX__
