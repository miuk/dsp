#ifndef __ULAW_H__
#define __ULAW_H__

#include <stdint.h>

uint8_t linear2ulaw(int16_t pcm_val);
int16_t ulaw2linear(uint8_t ulaw_val);

#endif // __ULAW_H__
