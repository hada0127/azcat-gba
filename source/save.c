#include "save.h"
#include <string.h>

#ifdef PLATFORM_GBA
/* SRAM 식별 문자열 — 에뮬레이터가 ROM에서 이 패턴을 찾아 save type 자동 감지 */
const char sram_id[] = "SRAM_Vnnn";
#endif

u16 save_compute_checksum(const SaveData* sd) {
    /* magic의 상위/하위 16비트 XOR + hiscore */
    u16 hi = (u16)(sd->magic >> 16);
    u16 lo = (u16)(sd->magic & 0xFFFF);
    return hi ^ lo ^ (u16)sd->hiscore;
}

bool save_is_valid(const SaveData* sd) {
    if (sd->magic != SAVE_MAGIC) return false;
    if (sd->checksum != save_compute_checksum(sd)) return false;
    return true;
}

void save_prepare(SaveData* sd, s16 hiscore) {
    sd->magic = SAVE_MAGIC;
    sd->hiscore = hiscore;
    sd->checksum = save_compute_checksum(sd);
}

#ifdef PLATFORM_GBA
/* GBA SRAM 바이트 단위 읽기/쓰기 */
#define SRAM_BASE ((volatile u8*)0x0E000000)

void save_write(const SaveData* sd) {
    const u8* src = (const u8*)sd;
    volatile u8* dst = SRAM_BASE;
    u32 i;
    for (i = 0; i < sizeof(SaveData); i++) {
        dst[i] = src[i];
    }
}

void save_read(SaveData* sd) {
    u8* dst = (u8*)sd;
    volatile u8* src = SRAM_BASE;
    u32 i;
    for (i = 0; i < sizeof(SaveData); i++) {
        dst[i] = src[i];
    }
}

#else
/* PC 스텁: 메모리 버퍼 */
static u8 sram_buf[256];

void save_write(const SaveData* sd) {
    memcpy(sram_buf, sd, sizeof(SaveData));
}

void save_read(SaveData* sd) {
    memcpy(sd, sram_buf, sizeof(SaveData));
}
#endif
