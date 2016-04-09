#ifndef __ULAW_H__
#define __ULAW_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
uint8_t linear2ulaw(int16_t pcm_val);
int16_t ulaw2linear(uint8_t ulaw_val);
#ifdef __cplusplus
};
#endif
#endif // __ULAW_H__
