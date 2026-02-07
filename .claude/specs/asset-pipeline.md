# 에셋 파이프라인

## 변환 도구

- **grit**: 이미지 → GBA용 .c/.h 변환 (devkitPro 포함)
- **ImageMagick/ffmpeg**: 리사이즈, 색조 변환 등 전처리

## 배경 이미지 (Mode 4, 8bpp, 256색)

| 용도 | 원본 파일 | 변환 스펙 | grit 옵션 |
|------|----------|----------|-----------|
| 타이틀 배경 | `export/images/8.jpg` | 240×160 리사이즈/크롭 | `-gb -gB8 -p` |
| 낮 배경 | `export/images/74.jpg` | 왼쪽 220px 크롭 → 240×160 | `-gb -gB8 -p` |
| 밤 배경 | `export/images/69.jpg` | 240×160 리사이즈 | `-gb -gB8 -p` |
| 매트릭스 배경 | `export/images/71.jpg` | 240×160 | `-gb -gB8 -p` |
| 게임오버 배경 | 게임 배경 재활용 | 밝기 50% 감소 | 런타임 팔레트 조작 |

### 배경 변환 순서

1. 원본 이미지를 240×160으로 리사이즈 (비율 유지 + 크롭)
2. 256색으로 감색 (quantize)
3. grit으로 .c/.h 변환
4. 팔레트 데이터 + 픽셀 데이터 생성

## 스프라이트 (OAM, 4bpp, 16색/뱅크)

| 용도 | 원본 파일 | 크기 | 프레임 수 | 팔레트 뱅크 |
|------|----------|------|----------|------------|
| 플레이어 | `export/images/56.png` | 32×32 | 4 (좌/우/정지/사망) | 0 |
| 흰 고양이 | `export/images/37.png` | 16×16 | 2 (낙하/착지) | 1 |
| 갈색 고양이 | `export/images/39.png` | 16×16 | 2 (낙하/착지) | 1 (공유 가능) |
| 아이템 4종 | 41.png, 43.png 등 | 16×16 | 4종 | 2 |
| 폭발 이펙트 | `export/images/52.png` | 32×32 | 3~4 | 3 |
| 얼굴(헤헤) | `export/images/28.png` | 32×32 | 1 | 4 |
| 얼굴(흠) | `export/images/30.png` | 32×32 | 1 | 4 (공유) |
| 얼굴(아파) | `export/images/32.png` | 32×32 | 1 | 4 (공유) |
| 얼굴(깨당) | `export/images/34.png` | 32×32 | 1 | 4 (공유) |
| 폭탄 아이콘 | 별도 제작 | 16×16 | 1 | 5 |

### 스프라이트 grit 옵션

```
-gB4 -Mh -p -gt -gT[투명색]
```
- `-gB4`: 4bpp
- `-Mh`: 가로 정렬
- `-gt`: 타일 모드
- `-gT[색]`: 투명색 지정

## 폰트/텍스트

| 용도 | 방식 | 비고 |
|------|------|------|
| 숫자 0~9 | 8×8 타일 폰트 | 점수 표시용 |
| "SCORE", "HISCORE" | 미리 렌더링 타일 | 고정 텍스트 |
| 한글 등급명 | 비트맵 미리 렌더링 | 등급 표시용 |
| 게임오버 텍스트 | `export/images/81~123.png` 참고 | 재제작 |

## 사운드 (후순위)

| 용도 | 원본 파일 | GBA 변환 |
|------|----------|---------|
| BGM | `export/sounds/79.mp3` | MOD/S3M 또는 raw PCM 8bit |
| 효과음 | `12.mp3`, `46.mp3`, `54.mp3`, `126.mp3` | raw PCM 8bit, 8~16kHz |

- MaxMod 라이브러리 사용 권장
- 사운드 없이도 동작하도록 설계

## 원본 스크립트 참조

게임 로직 이식 시 아래 ActionScript 파일 참조:

| 시스템 | 참조 파일 |
|--------|----------|
| 게임 초기화 | `export/scripts/frame_28/DoAction.as` |
| 게임 루프 | `export/scripts/DefineSprite_77/frame_2/DoAction.as` |
| 고양이 로직 | `export/scripts/DefineSprite_55/frame_1/DoAction.as` |
| 아이템 로직 | `export/scripts/DefineSprite_47/frame_1/DoAction.as` |
| 아이템 효과 | `export/scripts/DefineSprite_47/frame_23/DoAction.as` |
| 플레이어 좌이동 | `export/scripts/DefineSprite_68/frame_26/DoAction.as` |
| 플레이어 우이동 | `export/scripts/DefineSprite_68/frame_38/DoAction.as` |
| 키 입력 | `export/scripts/DefineButton2_78/*.as` |
| 게임오버 | `export/scripts/frame_51/DoAction.as` |
