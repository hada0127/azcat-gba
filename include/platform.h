#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef PLATFORM_GBA
/* GBA 빌드: libtonc 사용 */
#include <tonc.h>
#else
/* PC 빌드: 표준 타입 사용 */
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;

#endif /* PLATFORM_GBA */

#endif /* PLATFORM_H */
