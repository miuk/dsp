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
    frame_size = hz / 100;
    int val = (xBR == 0) ? 0 : 1;
    opus_encoder_ctl(enc, OPUS_SET_VBR(val));
    pthread_mutex_unlock(&mutex);
    setBps(bps);
    cout << "OpusCodec::initXBR, frame_size=" << frame_size
         << ", xBR=" << xBR
         << ", bps=" << bps
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
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < n; i++) {
        uint8_t encoded[frame_size*2];
        int nbyte = opus_encode(enc, src_pos, frame_size, encoded, frame_size*2);
        enclen += nbyte;
        int ret = opus_decode(dec, encoded, nbyte, dst_pos, frame_size, 0);
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
