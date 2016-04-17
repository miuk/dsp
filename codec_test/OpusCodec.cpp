#include "OpusCodec.hxx"

#include <assert.h>
#include <string.h>
#include <iostream>
using namespace std;

OpusCodec::OpusCodec(void)
{
    enc = NULL;
    dec = NULL;
    frame_size = 0;

    hz = 8000;
    bps = 12000;
    xBR = 0;

    complexity = 5;
    dtx = false;
    inband_fec = false;
    lsb_depth = 24;
    plc_tuning = 0;
    prediction_disabled = false;
    gain = 0;

    pthread_mutex_init(&mutex, NULL);
}

const char*
OpusCodec::getName(void) const
{
    static const char* name = "Opus";
    return name;
}

void
OpusCodec::init(void)
{
    initXBR(xBR);
}

void
OpusCodec::initXBR(int value)
{
    pthread_mutex_lock(&mutex);
    xBR = value;
    if (enc != NULL)
        opus_encoder_destroy(enc);
    if (dec != NULL)
        opus_decoder_destroy(dec);
    int err;
    enc = opus_encoder_create(hz, 1, OPUS_APPLICATION_VOIP, &err);
    dec = opus_decoder_create(hz, 1, &err);
    frame_size = hz / 50;       // 20ms frame
    int val = (xBR == 0) ? 0 : 1;
    opus_encoder_ctl(enc, OPUS_SET_VBR(val));
    pthread_mutex_unlock(&mutex);
    setBps(bps);
    initOptions();
    opus_encoder_ctl(enc, OPUS_GET_EXPERT_FRAME_DURATION(&val));
    cout << "OpusCodec::initXBR, frame_size=" << frame_size
         << ", xBR=" << xBR
         << ", bps=" << bps
         << ", expert_frame_duration=" << val
         << endl;
}

void
OpusCodec::setBps(int value)
{
    //cout << "OpusCodec::setQuality " << value << endl;
    pthread_mutex_lock(&mutex);
    bps = value;
    opus_encoder_ctl(enc, OPUS_SET_BITRATE(bps));
    pthread_mutex_unlock(&mutex);
    //cout << "OpusCodec::setQuality done " << value << endl;
}

void
OpusCodec::setHz(int hz)
{
    if (this->hz == hz)
        return;
    this->hz = hz;
    initXBR(xBR);
}

void
OpusCodec::setXBR(int value)
{
    if (value == xBR)
        return;
    initXBR(value);
}

void
OpusCodec::setComplexity(int value)
{
    complexity = value;
    opus_encoder_ctl(enc, OPUS_SET_COMPLEXITY(value));
}

void
OpusCodec::setDTX(bool value)
{
    dtx = value;
    int val = (value) ? 1 : 0;
    opus_encoder_ctl(enc, OPUS_SET_DTX(val));
}

void
OpusCodec::setInbandFEC(bool value)
{
    inband_fec = value;
    int val = (value) ? 1 : 0;
    opus_encoder_ctl(enc, OPUS_SET_INBAND_FEC(val));
}

void
OpusCodec::setLSBDepth(int value)
{
    lsb_depth = value;
    opus_encoder_ctl(enc, OPUS_SET_LSB_DEPTH(value));
}

void
OpusCodec::setPLC(int value)
{
    plc_tuning = value;
    opus_encoder_ctl(enc, OPUS_SET_PACKET_LOSS_PERC(value));
}

void
OpusCodec::setPredictionDisabled(bool value)
{
    prediction_disabled = value;
    int val = (value) ? 1 : 0;
    opus_encoder_ctl(enc, OPUS_SET_PREDICTION_DISABLED(val));
}

void
OpusCodec::setGain(int value)
{
    gain = value;
    opus_decoder_ctl(dec, OPUS_SET_GAIN(value));
}

void
OpusCodec::initOptions(void)
{
    setComplexity(complexity);
    setDTX(dtx);
    setInbandFEC(inband_fec);
    setLSBDepth(lsb_depth);
    setPLC(plc_tuning);
    setPredictionDisabled(prediction_disabled);
    setGain(gain);
}

int
OpusCodec::getFrameSize(void)
{
    int ptime = pls->getPtime();
    return frame_size * (ptime / 20);
}

void
OpusCodec::clear(void)
{
    cout << "OpusCodec::clear" << endl;
}

int
OpusCodec::encode(const int16_t* src, int srclen, char* dst)
{
    int n = srclen / frame_size;
    int dstlen = 0;
    uint8_t* dst_pos = (uint8_t*)dst;
    const int16_t* src_pos = src;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < n; i++) {
        int nbyte = opus_encode(enc, src_pos, frame_size, dst_pos, frame_size*2);
        src_pos += frame_size;
        dstlen += nbyte;
        dst_pos += nbyte;
    }
    pthread_mutex_unlock(&mutex);
    return dstlen;
}

int
OpusCodec::decode(const char* src, int srclen, int16_t* dst)
{
    return 0;
}

int
OpusCodec::codec(const int16_t* src, int srclen, int16_t* dst, int& bps)
{
    int n = srclen / frame_size;
    int enclen = 0;
    int dstlen = 0;
    int16_t* dst_pos = dst;
    const int16_t* src_pos = src;
    int nf = pls->getPtime() / 20;
    bool bLost = false;
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < n; i++) {
        if (i % nf == 0)
            bLost = pls->isLost();
        uint8_t encoded[frame_size*2];
        int nbyte = opus_encode(enc, src_pos, frame_size, encoded, frame_size*2);
        enclen += nbyte;
        uint8_t* bits = (bLost) ? NULL : encoded;
        int ret = opus_decode(dec, bits, nbyte, dst_pos, frame_size, 0);
        if (ret <= 0) {
            cout << "OpusCodec::codec decode failed " << ret << endl;
        }
        src_pos += frame_size;
        dstlen += frame_size;
        dst_pos += frame_size;
    }
    pthread_mutex_unlock(&mutex);
    bps = (srclen > 0) ? (hz * 8 * enclen / srclen) : 0;
    /*
    cout << "OpusCodec::codec"
         << ", srclen=" << srclen
         << ", dstlen=" << dstlen
         << ", enclen=" << enclen
         << ", bps=" << bps
         << endl;
    */
    return dstlen;
}
