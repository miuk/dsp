
#ifndef __OPUS_CODEC_HXX__
#define __OPUS_CODEC_HXX__

#include "Codec.hxx"
#include "pthread.h"
#include <opus/opus.h>

class OpusCodec : public Codec {
public:
    OpusCodec(void);
    virtual ~OpusCodec(void) {
        pthread_mutex_destroy(&mutex);
    };
    const char* getName(void) const;
    virtual void init(void);
    virtual void clear(void);
    virtual void setHz(int hz);
    virtual void setWAVFmtType(int) {};
    virtual int getFrameSize(void) { return frame_size; };
    virtual int encode(const int16_t* src, int srclen, char* dst);
    virtual int decode(const char* src, int srclen, int16_t* dst);
    virtual int codec(const int16_t* src, int srclen, int16_t* dst, int& bps);

    void setBps(int value);
    void setXBR(int value);
private:
    void initXBR(int value);
private:
    OpusEncoder* enc;
    OpusDecoder* dec;
    int frame_size;

    int hz;
    int bps;
    int xBR;

    pthread_mutex_t mutex;
};

#endif // __OPUS_CODEC_HXX__
