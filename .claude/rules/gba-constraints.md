---
paths:
  - "source/**/*.c"
  - "source/**/*.h"
  - "include/**/*.h"
  - "Makefile"
  - "data/**"
---

# GBA 하드웨어 제약 규칙

## VRAM 접근
- 16bit 단위로만 쓰기 (8bit 쓰기 불가)
- Mode 4에서 홀수 X 픽셀: 인접 픽셀과 함께 16bit read-modify-write
- OAM 갱신은 반드시 VBlank 중에만
- DMA 전송 활용하여 배경 복사 고속화

## Mode 4 비트맵
- 8bpp 팔레트 (256색)
- 더블 버퍼: 0x06000000 / 0x0600A000
- 1프레임 = 0x9600 bytes
- `vid_flip()` 으로 페이지 전환

## OAM 스프라이트
- 최대 128개, 이 프로젝트는 ~40개 사용
- 4bpp = 16색/뱅크, 총 16뱅크
- 크기: 8x8, 16x16, 32x32, 64x64 등 고정 크기만

## 팔레트
- BG 팔레트: 256색 (배경용)
- OBJ 팔레트: 16색 × 16뱅크 (스프라이트용)
- 팔레트 뱅크 간 색상 공유 불가

## 성능
- 고양이 33개 동시 처리 시 30fps 이상 유지
- 무거운 연산은 VBlank 밖에서
- memcpy/memset 대신 DMA 전송 권장

## 금지사항
- float/double 사용 금지 (FPU 없음)
- malloc/free 사용 금지 (동적 할당 불가)
- 표준 라이브러리 최소 사용 (ROM 크기 절감)
