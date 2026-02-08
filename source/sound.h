#ifndef SOUND_H
#define SOUND_H

#include "types.h"

/* SFX ID */
#define SFX_HIT      0  /* 고양이 피격 */
#define SFX_GAMEOVER 1  /* 게임오버 */
#define SFX_BOMB     2  /* 폭탄 사용 */
#define SFX_ITEM     3  /* 아이템 획득 */

void sound_init(void);
void sound_play_bgm(u8 track);
void sound_play_sfx(u8 sfx_id);
void sound_update(void);
void sound_stop(void);

#endif /* SOUND_H */
