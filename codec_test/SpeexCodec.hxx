
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
    virtual int getFrameSize(void) { return frame_size; };
    virtual int encode(const int16_t* src, int srclen, char* dst);
    virtual int decode(const char* src, int srclen, int16_t* dst);
    virtual int codec(const int16_t* src, int srclen, int16_t* dst);

    void setQuality(int value);
    void setXBR(int value);
private:
    void initXBR(int value);
private:
    void* enc_state;
    SpeexBits enc_bits;
    void* dec_state;
    SpeexBits dec_bits;
    int frame_size;

    int hz;
    int quality;
    int xBR;

    pthread_mutex_t mutex;
};

#endif // __SPEEX_CODEC_HXX__
