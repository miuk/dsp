
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
    virtual int getFrameSize(void);
    virtual int encode(const int16_t* src, int srclen, char* dst);
    virtual int decode(const char* src, int srclen, int16_t* dst);
    virtual int codec(const int16_t* src, int srclen, int16_t* dst, int& bps);

    void setBps(int value);
    void setXBR(int value);

    void setComplexity(int value);
    void setDTX(bool value);
    void setInbandFEC(bool value);
    void setLSBDepth(int value);
    void setPLC(int value);
    void setPredictionDisabled(bool value);
    void setGain(int value);
private:
    void initXBR(int value);
    void initOptions(void);
private:
    OpusEncoder* enc;
    OpusDecoder* dec;
    int frame_size;

    int hz;
    int bps;
    int xBR;

    // encoder options
    int complexity; // 0-10
    bool dtx;
    bool inband_fec;
    int lsb_depth; // 8-24
    int plc_tuning; // 0-100
    bool prediction_disabled;
    // decoder options
    int gain; // -32768 - 32767

    pthread_mutex_t mutex;
};

#endif // __OPUS_CODEC_HXX__
