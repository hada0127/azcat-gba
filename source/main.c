#ifdef PLATFORM_GBA

#include <tonc.h>
#include "types.h"
#include "game.h"

int main(void) {
    /* 빈 엔트리포인트 - Step 6에서 구현 */
    irq_init(NULL);
    irq_enable(II_VBLANK);

    while (1) {
        VBlankIntrWait();
    }

    return 0;
}

#endif /* PLATFORM_GBA */
