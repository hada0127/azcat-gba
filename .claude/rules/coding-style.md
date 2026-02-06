---
paths:
  - "source/**/*.c"
  - "source/**/*.h"
  - "include/**/*.h"
---

# GBA C 코딩 스타일

## 네이밍
- 함수/변수: `snake_case`
- 상수/매크로: `UPPER_SNAKE_CASE`
- 타입: `PascalCase` (구조체, typedef)
- 파일: `소문자.c/.h`

## 고정소수점
- .8 fixed point 사용 (FP_SHIFT = 8)
- `#define FP(x) ((x) << 8)`
- `#define FP_TO_INT(x) ((x) >> 8)`
- 부동소수점 절대 사용 금지 (GBA에 FPU 없음)

## 상수 정의
```c
// 좌표계
#define SCREEN_W        240
#define SCREEN_H        160
#define HUD_H           16
#define GAME_AREA_H     144
#define PLAYER_Y        (HUD_H + 120)
#define GROUND_Y        144

// 오브젝트 제한
#define MAX_CATS        33
#define MAX_ITEMS       1
#define MAX_OAM_USED    40
```

## 헤더 가드
```c
#ifndef AZCAT_MODULE_H
#define AZCAT_MODULE_H
// ...
#endif // AZCAT_MODULE_H
```

## 주석
- 한글, 핵심만 (`// 고양이 착지 판정`)
- 함수 위에 한 줄 설명
- 복잡한 수식에만 인라인 주석
