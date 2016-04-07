#include "SpeexCodec.hxx"

#include <string.h>
#include <iostream>
using namespace std;

static const int bpsList[] = {
    2150,
    3950,
    5950,
    8000,
    8000,
    11000,
    11000,
    15000,
    15000,
    18200,
    24600,
};

SpeexCodec::SpeexCodec(void)
{
    enc_state = NULL;
    dec_state = NULL;
    frame_size = 0;
    rest = NULL;
    rest_len = 0;

    quality = 5;
    xBR = 0;

    pthread_mutex_init(&mutex, NULL);
}

const char*
SpeexCodec::getName(void) const
{
    static const char* name = "Speex";
    return name;
}

void
SpeexCodec::init(void)
{
    initXBR(xBR);
}

void
SpeexCodec::initXBR(int value)
{
    pthread_mutex_lock(&mutex);
    xBR = value;
    if (enc_state != NULL)
        speex_encoder_destroy(enc_state);
    enc_state = speex_encoder_init(&speex_nb_mode);
    speex_bits_init(&enc_bits);
    if (dec_state != NULL)
        speex_decoder_destroy(dec_state);
    dec_state = speex_decoder_init(&speex_nb_mode);
    speex_bits_init(&dec_bits);
    speex_encoder_ctl(enc_state, SPEEX_GET_FRAME_SIZE, &frame_size);
    if (xBR == 2) {
        int work = 1;
        speex_encoder_ctl(enc_state, SPEEX_SET_VBR, &work);
    }
    pthread_mutex_unlock(&mutex);
    setQuality(quality);
    cout << "SpeexCodec::initXBR, frame_size=" << frame_size
         << ", xBR=" << xBR
         << ", quality=" << quality
         << endl;
}

void
SpeexCodec::setQuality(int value)
{
    //cout << "SpeexCodec::setQuality " << value << endl;
    pthread_mutex_lock(&mutex);
    quality = value;
    switch (xBR) {
    case 0 :                    // CBR
        speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &value);
        break;
    case 1 : {
        int bps = bpsList[quality];
        speex_encoder_ctl(enc_state, SPEEX_SET_ABR, &bps);
    }
        break;
    case 2 : {                   // VBR
        float fval = (float)value;
        speex_encoder_ctl(enc_state, SPEEX_SET_VBR_QUALITY, &fval);
        int bps = bpsList[quality];
        speex_encoder_ctl(enc_state, SPEEX_SET_VBR_MAX_BITRATE, &bps);
    }
        break;
    default :
        break;
    }
    pthread_mutex_unlock(&mutex);
    //cout << "SpeexCodec::setQuality done " << value << endl;
}

void
SpeexCodec::setXBR(int value)
{
    if (value == xBR)
        return;
    initXBR(value);
}

void
SpeexCodec::clear(void)
{
    cout << "SpeexCodec::clear" << endl;
    delete[] rest;
    rest = NULL;
    rest_len = 0;
}

int
SpeexCodec::encode(const int16_t* src, int srclen, char* dst)
{
    pthread_mutex_lock(&mutex);

    int buflen = rest_len + srclen;
    cout << "SpeexCodec::encode buflen=" << buflen << endl;
    int16_t buf[buflen];
    if (rest_len > 0)
        memcpy(buf, rest, sizeof(int16_t) * rest_len);
    memcpy(buf + sizeof(int16_t), src, sizeof(int16_t) * srclen);
    rest_len = 0;
    delete[] rest;
    rest = NULL;
    
    int dstlen = 0;
    const int16_t* tail = buf + buflen;
    char* dst_pos = dst;
    int16_t* pos = buf;
    int n = buflen / frame_size;
    for (int i = 0; i < n; i++) {
        speex_bits_reset(&enc_bits);
        speex_encode_int(enc_state, pos, &enc_bits);
        int nbyte = speex_bits_write(&enc_bits, dst_pos, frame_size*2);
        pos += frame_size;
        dstlen += nbyte;
        dst_pos += nbyte;
    }
    if (pos < tail) {
        rest_len = tail - pos;
        rest = new int16_t[rest_len];
        memcpy(rest, pos, sizeof(int16_t) * rest_len);
    }

    pthread_mutex_unlock(&mutex);

    cout << "SpeexCodec::encode dstlen=" << dstlen << endl;
    return dstlen;
}

int
SpeexCodec::decode(const char* src, int srclen, int16_t* dst)
{
    pthread_mutex_lock(&mutex);

    char buf[srclen];
    memcpy(buf, src, srclen);
    //speex_bits_read_from(&dec_bits, buf, srclen);
    speex_bits_read_whole_bytes(&dec_bits, buf, srclen);
    int16_t* dst_pos = dst;
    int dstlen = 0;
    while (speex_bits_remaining(&dec_bits) > 0) {
        int ret = speex_decode_int(dec_state, &dec_bits, dst_pos);
        if (ret == 0) {
            dstlen += frame_size;
            dst_pos += frame_size;
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
    cout << "SpeexCodec::decode " << dstlen << endl;
    return dstlen * 2;
}

int
SpeexCodec::codec(const int16_t* src, int srclen, int16_t* dst)
{
    pthread_mutex_lock(&mutex);
    int buflen = rest_len + srclen;
    int16_t buf[buflen];
    if (rest_len > 0)
        memcpy(buf, rest, sizeof(int16_t) * rest_len);
    memcpy(buf + sizeof(int16_t), src, sizeof(int16_t) * srclen);
    rest_len = 0;
    delete[] rest;
    rest = NULL;

    int enclen = 0;
    int dstlen = 0;
    const int16_t* tail = buf + buflen;
    int16_t* dst_pos = dst;
    int16_t* pos = buf;
    int n = buflen / frame_size;
    for (int i = 0; i < n; i++) {
        char encoded[frame_size*2];
        speex_bits_reset(&enc_bits);
        speex_encode_int(enc_state, pos, &enc_bits);
        int nbyte = speex_bits_write(&enc_bits, encoded, frame_size*2);
        enclen += nbyte;
        speex_bits_read_from(&dec_bits, encoded, nbyte);
        int ret = speex_decode_int(dec_state, &dec_bits, dst_pos);
        if (ret != 0) {
            cout << "SpeexCodec::codec decode failed " << ret << endl;
        }
        pos += frame_size;
        dstlen += frame_size;
        dst_pos += frame_size;
    }
    if (pos < tail) {
        rest_len = tail - pos;
        rest = new int16_t[rest_len];
        memcpy(rest, pos, sizeof(int16_t) * rest_len);
    }
    pthread_mutex_unlock(&mutex);
    int bps = (srclen > 0) ? (64000 * enclen / srclen) : 0;
    cout << "SpeexCodec::codec buflen=" << buflen
         << ", srclen=" << srclen
         << ", dstlen=" << dstlen
         << ", enclen=" << enclen
         << ", restlen=" << rest_len
         << ", bps=" << bps
         << endl;
    return dstlen;
}