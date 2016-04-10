#include "G729aCodec.hxx"

extern "C" {
#define __unix__
#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
};

#include <string.h>

// for encoder
extern Word16 *new_speech;         /* Pointer to new speech data        */
static Word16 prm[PRM_SIZE];       /* Analysis parameters.                  */

// for decoder
Word16 bad_lsf;        /* bad LSF indicator   */
static Word16  synth_buf[L_FRAME+M], *synth; /* Synthesis                   */
static Word16  parm[PRM_SIZE+1];             /* Synthesis parameters        */
static Word16  Az_dec[MP1*2];                /* Decoded Az for post-filter  */
static Word16  T2[2];                        /* Pitch lag for 2 subframes   */

G729aCodec::G729aCodec(void)
{
}

G729aCodec::~G729aCodec(void)
{
}

const char*
G729aCodec::getName(void) const
{
    static const char* name = "G729a";
    return name;
}

void
G729aCodec::init(void)
{
    // for encoder
    Init_Pre_Process();
    Init_Coder_ld8a();
    Set_zero(prm, PRM_SIZE);
    // for decoder
    for (int i= 0; i < M; i++)
        synth_buf[i] = 0;
    synth = synth_buf + M;
    bad_lsf = 0;          /* Initialize bad LSF indicator */
    Init_Decod_ld8a();
    Init_Post_Filter();
    Init_Post_Process();
}

void
G729aCodec::clear(void)
{
}

int
G729aCodec::encode(const int16_t* src, int srclen, char* dst)
{
    int dstlen = sizeof(*src) * srclen;
    memcpy(dst, src, dstlen);
    return dstlen;
}

int
G729aCodec::decode(const char* src, int srclen, int16_t* dst)
{
    int dstlen = srclen / sizeof(*dst);
    memcpy(dst, src, srclen);
    return dstlen;
}

int
G729aCodec::codec(const int16_t* src, int srclen, int16_t* dst, int& bps)
{
    int n = srclen / L_FRAME;
    const int16_t* src_pos = src;
    int16_t* dst_pos = dst;
    int enclen = 0;
    int dstlen = 0;
    for (int i = 0; i < n; i++) {
        Word16 serial[SERIAL_SIZE];
        // encode
        memcpy(new_speech, src_pos, sizeof(int16_t) * L_FRAME);
        Pre_Process(new_speech, L_FRAME);
        Coder_ld8a(prm);
        prm2bits_ld8k(prm, serial);
        // decode
        bits2prm_ld8k(&serial[2], &parm[1]);
        //enclen += SERIAL_SIZE;
        enclen += PRM_SIZE;
        /* the hardware detects frame erasures by checking if all bits
           are set to zero
        */
        parm[0] = 0;           /* No frame erasure */
        for (int j = 2; j < SERIAL_SIZE; j++)
            if (serial[j] == 0 ) parm[0] = 1; /* frame erased     */
        /* check pitch parity and put 1 in parm[4] if parity error */
        parm[4] = Check_Parity_Pitch(parm[3], parm[4]);
        Decod_ld8a(parm, synth, Az_dec, T2);
        Post_Filter(synth, Az_dec, T2);        /* Post-filter */
        Post_Process(synth, L_FRAME);
        memcpy(dst_pos, synth, sizeof(int16_t) * L_FRAME);
        src_pos += L_FRAME;
        dst_pos += L_FRAME;
        dstlen += L_FRAME;
    }
    bps = (srclen > 0) ? (8000 * 8 * enclen / srclen) : 0;
    return dstlen;
}
