# azcat-gba: 아! 고양이를 피해라 대왕 - GBA 컨버팅

2003년 Flash 게임 "아! 고양이를 피해라 대왕"을 GBA ROM으로 새로 제작하는 프로젝트.

## 대화 스타일

- 반말 사용: ~임, ~했음, ~야, ~해, ~됨, ~인듯 등 간결한 반말체
- 불필요한 설명 생략, 핵심만 전달
- 코드 변경 시 이유 한 줄로

## 기술 스택

- **언어**: C (devkitARM + libtonc)
- **타겟**: GBA (ARM7TDMI, 240x160, Mode 4)
- **에셋 변환**: grit
- **테스트**: mGBA 에뮬레이터
- **빌드**: `make` (아래 빌드 명령어 참조)

## 빌드 명령어

Windows 환경에서 devkitPro msys2 셸을 통해 빌드. 반드시 아래 명령어 사용:

```bash
/c/devkitPro/msys2/usr/bin/bash.exe -c "export DEVKITARM=/opt/devkitpro/devkitARM; export DEVKITPRO=/opt/devkitpro; export PATH=/opt/devkitpro/msys2/usr/bin:/opt/devkitpro/devkitARM/bin:/opt/devkitpro/tools/bin:\$PATH; export TEMP=/tmp; export TMP=/tmp; export TMPDIR=/tmp; cd /c/Users/taro1/Downloads/azcat; make 2>&1"
```

- 클린 빌드 시 `make clean 2>&1;` 를 `make 2>&1` 앞에 추가
- `TEMP=/tmp` 필수 (없으면 C:\WINDOWS\ 권한 오류)
- devkitPro 경로: `C:\devkitPro` (msys2 내에서는 `/opt/devkitpro`)

## Git 워크플로우 (필수)

**모든 작업 단위가 끝나면 반드시 직접 git commit + push를 수행해야 함.**
사용자가 별도로 요청하지 않아도, 코드 작성/수정/파일 생성이 포함된 작업이 완료되면 자동으로 아래를 실행:

```
git add <변경된 파일들>
git commit -m "커밋 메시지

Co-Authored-By: Claude Opus 4.6 <noreply@anthropic.com>"
git push
```

규칙:
- 커밋 메시지는 한글, 간결하게 (예: "고양이 낙하 시스템 구현")
- `git add`는 변경 파일만 명시적으로 (git add -A 금지)
- .gba 파일은 빌드 후 항상 함께 커밋
- .elf, .o, .d, .map, build/ 등 기타 빌드 산출물은 절대 커밋하지 않음
- push 실패 시(remote 없음 등) 무시하고 진행
- 단순 조사/질문 응답처럼 파일 변경이 없는 경우는 커밋하지 않음

## 프로젝트 구조

```
azcat-gba/
├── source/        # C 소스코드 (.c/.h)
├── graphics/      # 변환 전 원본 이미지
├── data/          # grit 변환된 에셋 (.c/.h)
├── include/       # 공통 헤더, 상수 정의
├── Makefile
└── azcat.gba      # ROM 출력 (커밋 대상)
```

## 참조 문서

- 게임 스펙 상세: @.claude/specs/game-spec.md
- GBA 기술 스펙: @.claude/specs/gba-technical.md
- 에셋 변환 파이프라인: @.claude/specs/asset-pipeline.md
- 난이도/등급 테이블: @.claude/specs/difficulty-grades.md
- 원본 분석 리포트: @report.md
- 구현 계획/체크리스트: @plan.md

## 코딩 규칙

- 고정소수점 .8 fixed point (FP_SHIFT=8)
- 네이밍: snake_case
- 매직넘버 금지 → `#define` 상수
- 주석: 한글, 최소한만
- OAM 인덱스 충돌 주의 (최대 128개, ~40개 사용)
- VRAM 쓰기는 16bit 단위, VBlank 중에만 OAM 갱신

## 주의사항

- Mode 4 = 8bpp 팔레트, 배경당 256색 제한
- OAM 스프라이트 = 4bpp, 팔레트 뱅크당 16색
- 홀수 X 픽셀 쓸 때 16bit read-modify-write 필요
- 원본 Flash 로직 최대한 충실 이식 + GBA 하드웨어 맞춤 조정
