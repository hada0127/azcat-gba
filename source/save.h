#ifndef SAVE_H
#define SAVE_H

#include "types.h"

#define SAVE_MAGIC 0x415A4341  /* "AZCA" */

typedef struct {
    u32 magic;
    s16 hiscore;
    u16 checksum;
} SaveData;

/* 체크섬 계산 */
u16 save_compute_checksum(const SaveData* sd);

/* 유효성 검증 */
bool save_is_valid(const SaveData* sd);

/* 저장 데이터 준비 (매직넘버, 체크섬 설정) */
void save_prepare(SaveData* sd, s16 hiscore);

/* GBA: SRAM 읽기/쓰기. PC: 메모리 버퍼 */
void save_write(const SaveData* sd);
void save_read(SaveData* sd);

#endif /* SAVE_H */
