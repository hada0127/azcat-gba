"""
MP3 → 8-bit signed PCM → C 배열 변환
ffmpeg 직접 호출: 8192Hz mono 8-bit signed PCM
출력: data/snd_*.c + data/snd_*.h
"""
import os
import struct
import subprocess
import sys

SRC = os.path.join(os.path.dirname(__file__), '..', 'export', 'sounds')
DATA = os.path.join(os.path.dirname(__file__), '..', 'data')

os.makedirs(DATA, exist_ok=True)

SAMPLE_RATE = 8192

# ffmpeg 경로 (winget 설치 위치)
FFMPEG_PATHS = [
    'ffmpeg',  # PATH에 있으면 그냥 사용
    os.path.expandvars(
        r'%LOCALAPPDATA%\Microsoft\WinGet\Packages'
        r'\Gyan.FFmpeg_Microsoft.Winget.Source_8wekyb3d8bbwe'
        r'\ffmpeg-8.0.1-full_build\bin\ffmpeg.exe'
    ),
]

# (소스 파일, 출력 이름, 설명)
SOUNDS = [
    ('79.mp3',  'snd_bgm',      'BGM (루프)'),
    ('12.mp3',  'snd_bomb',     '폭탄 사용'),
    ('46.mp3',  'snd_item',     '아이템 획득'),
    ('54.mp3',  'snd_hit',      '고양이 피격'),
    ('126.mp3', 'snd_gameover', '게임오버'),
]


def find_ffmpeg():
    for p in FFMPEG_PATHS:
        try:
            subprocess.run([p, '-version'], capture_output=True, check=True)
            return p
        except (FileNotFoundError, subprocess.CalledProcessError):
            continue
    print('ERROR: ffmpeg를 찾을 수 없음')
    sys.exit(1)


def mp3_to_signed8(ffmpeg, path, sample_rate):
    """MP3 → 8-bit signed PCM 배열 (bytes)"""
    # ffmpeg로 raw s8 PCM 출력 (stdout pipe)
    cmd = [
        ffmpeg, '-y', '-i', path,
        '-ac', '1',                    # mono
        '-ar', str(sample_rate),       # 8192Hz
        '-f', 's8',                    # signed 8-bit
        '-acodec', 'pcm_s8',
        'pipe:1'
    ]
    result = subprocess.run(cmd, capture_output=True)
    if result.returncode != 0:
        print(f'ffmpeg error: {result.stderr.decode("utf-8", errors="replace")}')
        sys.exit(1)
    data = result.stdout
    # 4바이트 정렬 (DMA 전송 단위)
    pad = (4 - len(data) % 4) % 4
    data += b'\x00' * pad
    return data


def write_c_file(name, data, desc):
    """C 소스 파일 생성 (u32 패킹)"""
    n_words = len(data) // 4
    path = os.path.join(DATA, f'{name}.c')
    with open(path, 'w') as f:
        f.write(f'/* {desc} - {len(data)} bytes, {SAMPLE_RATE}Hz 8-bit signed PCM */\n')
        f.write(f'const unsigned int {name}_data[{n_words}] = {{\n')
        for i in range(0, len(data), 4):
            word = struct.unpack('<I', data[i:i+4])[0]
            if i % 32 == 0:
                f.write('    ')
            f.write(f'0x{word:08X},')
            if (i + 4) % 32 == 0 or i + 4 >= len(data):
                f.write('\n')
            else:
                f.write(' ')
        f.write('};\n')
    print(f'  {path} ({n_words} words, {len(data)} bytes)')


def write_h_file(name, data, desc):
    """H 헤더 파일 생성"""
    guard = name.upper() + '_H'
    path = os.path.join(DATA, f'{name}.h')
    with open(path, 'w') as f:
        f.write(f'#ifndef {guard}\n')
        f.write(f'#define {guard}\n\n')
        f.write(f'/* {desc} - {len(data)} bytes, {SAMPLE_RATE}Hz 8-bit signed PCM */\n')
        f.write(f'#define {name.upper()}_LEN {len(data)}\n')
        f.write(f'extern const unsigned int {name}_data[];\n\n')
        f.write(f'#endif /* {guard} */\n')
    print(f'  {path}')


def main():
    print('=== MP3 -> GBA PCM 변환 ===')
    ffmpeg = find_ffmpeg()
    print(f'ffmpeg: {ffmpeg}')
    for src_file, name, desc in SOUNDS:
        src_path = os.path.join(SRC, src_file)
        if not os.path.exists(src_path):
            print(f'  SKIP: {src_path} 없음')
            continue
        print(f'\n[{desc}] {src_file} -> {name}')
        data = mp3_to_signed8(ffmpeg, src_path, SAMPLE_RATE)
        write_c_file(name, data, desc)
        write_h_file(name, data, desc)
    print('\n완료!')


if __name__ == '__main__':
    main()
