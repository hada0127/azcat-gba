#ifndef SOUND_H
#define SOUND_H

#include "types.h"

void sound_init(void);
void sound_play_bgm(u8 track);
void sound_play_sfx(u8 sfx_id);
void sound_stop(void);

#endif /* SOUND_H */
