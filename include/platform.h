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

/* tonc 키 입력 상수 (PC용) */
#define KEY_LEFT    0x0020
#define KEY_RIGHT   0x0010
#define KEY_UP      0x0040
#define KEY_DOWN    0x0080
#define KEY_A       0x0001
#define KEY_B       0x0002
#define KEY_L       0x0200
#define KEY_R       0x0100
#define KEY_START   0x0008
#define KEY_SELECT  0x0004

#endif /* PLATFORM_GBA */

#endif /* PLATFORM_H */
