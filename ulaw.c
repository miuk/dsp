#include "ulaw.h"

#define SIGN_BIT   0x80          /* Sign bit for a A-law byte. */
#define QUANT_MASK 0xf           /* Quantization field mask. */
#define NSEGS      8             /* Number of A-law segments. */
#define SEG_SHIFT  4             /* Left shift for segment number. */
#define SEG_MASK   0x70          /* Segment field mask. */
#define BIAS       0x84          /* Bias for linear code. */

static int16_t seg_end[8] = {
    0xFF, 0x1FF, 0x3FF, 0x7FF,
    0xFFF, 0x1FFF, 0x3FFF, 0x7FFF
};

static int16_t
search(int16_t val, int16_t *table, int size)
{
    int i;

    for (i = 0; i < size; i++) {
        if (val <= *table++)
            return i;
    }
    return size;
}

uint8_t
linear2ulaw(int16_t pcm_val) /* 2's complement (16-bit range) */
{
    int mask;
    int seg;
    uint8_t uval;

    /* Get the sign and the magnitude of the value. */
    if (pcm_val < 0) {
        pcm_val = BIAS - pcm_val;
        mask = 0x7F;
    } else {
        pcm_val += BIAS;
        mask = 0xFF;
    }

    /* Convert the scaled magnitude to segment number. */
    seg = search(pcm_val, seg_end, 8);

    /*
     * Combine the sign, segment, quantization bits;
     * and complement the code word.
     */
    if (seg >= 8)           /* out of range, return maximum value. */
        return (0x7F ^ mask);
    else {
        uval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0xF);
        return (uval ^ mask);
    }

}

int16_t
ulaw2linear(uint8_t ulaw_val)
{
    int t;

    /* Complement to obtain normal u-law value. */
    ulaw_val = ~ulaw_val;

    /*
     * Extract and bias the quantization bits. Then
     * shift up by the segment number and subtract out the bias.
     */
    t = ((ulaw_val & QUANT_MASK) << 3) + BIAS;
    t <<= ((unsigned)ulaw_val & SEG_MASK) >> SEG_SHIFT;

    return ((ulaw_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}
