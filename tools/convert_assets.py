"""
에셋 변환 스크립트
원본 이미지 → GBA용 리사이즈 → graphics/ 폴더에 저장
그 후 grit으로 data/*.c, data/*.h 생성
"""
import os
import sys
from PIL import Image, ImageDraw, ImageFont, ImageEnhance

SRC = os.path.join(os.path.dirname(__file__), '..', 'export', 'images')
GFX = os.path.join(os.path.dirname(__file__), '..', 'graphics')
DATA = os.path.join(os.path.dirname(__file__), '..', 'data')

os.makedirs(GFX, exist_ok=True)
os.makedirs(DATA, exist_ok=True)

PLAY_AREA_W = 180  # 게임 영역 너비 (오른쪽 60px = UI 패널)
UI_PANEL_COLOR = (40, 40, 56)  # 어두운 남색 UI 패널

def convert_bg(src_name, dst_name, size=(240, 160)):
    """배경 이미지 변환: 전체 화면 리사이즈"""
    src_path = os.path.join(SRC, src_name)
    dst_path = os.path.join(GFX, dst_name)
    im = Image.open(src_path).convert('RGB')
    result = im.resize(size, Image.LANCZOS)
    result.save(dst_path)
    print(f"  BG: {src_name} -> {dst_name} ({size[0]}x{size[1]})")
    return dst_path

def resize_preserve_aspect(im, target_size):
    """종횡비 보존 리사이즈 → target_size 내에 센터링, 나머지 투명"""
    tw, th = target_size
    ow, oh = im.size
    scale = min(tw / ow, th / oh)
    nw = max(1, int(ow * scale))
    nh = max(1, int(oh * scale))
    resized = im.resize((nw, nh), Image.LANCZOS)
    result = Image.new('RGBA', target_size, (0, 0, 0, 0))
    ox = (tw - nw) // 2
    oy = th - nh  # 하단 정렬 (캐릭터 발이 바닥에 닿도록)
    result.paste(resized, (ox, oy))
    return result

def convert_sprite(src_name, dst_name, size, transparent_bg=True):
    """스프라이트 변환 (4bpp, 16색). 종횡비 보존, 인덱스 0 = 투명"""
    src_path = os.path.join(SRC, src_name)
    dst_path = os.path.join(GFX, dst_name)
    im = Image.open(src_path).convert('RGBA')
    im = resize_preserve_aspect(im, size)

    if transparent_bg:
        # 알파 마스크 추출 (임계값 64: 반투명 그림자 보존)
        alpha_mask = [px[3] > 64 for px in im.getdata()]

        # 투명→마젠타 합성
        bg = Image.new('RGBA', im.size, (255, 0, 255, 255))
        bg.paste(im, (0, 0), im)
        rgb = bg.convert('RGB')

        # 불투명 픽셀만 모아서 15색 양자화 (인덱스 0은 투명용 예약)
        opaque_pixels = [px for px, opaque in zip(rgb.getdata(), alpha_mask) if opaque]

        if opaque_pixels:
            opaque_img = Image.new('RGB', (len(opaque_pixels), 1))
            opaque_img.putdata(opaque_pixels)
            opaque_q = opaque_img.quantize(colors=15, method=Image.MEDIANCUT)
            pal = opaque_q.getpalette()[:45]
        else:
            pal = [0] * 45

        # 팔레트: 인덱스 0 = 마젠타(투명), 1~15 = 실제 색상
        final_pal = [255, 0, 255] + pal
        ref = Image.new('P', (1, 1))
        ref.putpalette(final_pal + [0] * (768 - len(final_pal)))
        rgb_q = rgb.quantize(palette=ref, dither=Image.Dither.NONE)

        # 투명 픽셀을 강제로 인덱스 0
        pixels = list(rgb_q.getdata())
        for i in range(len(pixels)):
            if not alpha_mask[i]:
                pixels[i] = 0

        result = Image.new('P', size)
        result.putpalette(final_pal + [0] * (768 - len(final_pal)))
        result.putdata(pixels)
        result.save(dst_path)
    else:
        im = im.convert('RGB')
        im = im.quantize(colors=16, method=Image.MEDIANCUT)
        im.save(dst_path)

    print(f"  SPR: {src_name} -> {dst_name} ({size[0]}x{size[1]})")
    return dst_path

def convert_player_sprites():
    """플레이어 걷기 3프레임 + 사망 1프레임 (공유 15색 팔레트 + 인덱스 0 투명)"""
    frames = [
        ('56.png', 'spr_player_walk0.png', (32, 32)),
        ('61.png', 'spr_player_walk1.png', (32, 32)),
        ('63.png', 'spr_player_walk2.png', (32, 32)),
        ('66.png', 'spr_player_dead.png',  (64, 32)),
    ]

    # 1) RGBA 리사이즈(종횡비 보존) + 알파마스크 + RGB 합성
    alpha_masks = []
    rgb_imgs = []
    for src_name, _, size in frames:
        src_path = os.path.join(SRC, src_name)
        im = Image.open(src_path).convert('RGBA')
        im = resize_preserve_aspect(im, size)

        alpha_masks.append([px[3] > 64 for px in im.getdata()])

        bg = Image.new('RGBA', im.size, (255, 0, 255, 255))
        bg.paste(im, (0, 0), im)
        rgb_imgs.append(bg.convert('RGB'))

    # 2) 전체 불투명 픽셀 수집 → 15색 양자화
    all_opaque = []
    for i, img in enumerate(rgb_imgs):
        for px, opaque in zip(img.getdata(), alpha_masks[i]):
            if opaque:
                all_opaque.append(px)

    if all_opaque:
        opaque_img = Image.new('RGB', (len(all_opaque), 1))
        opaque_img.putdata(all_opaque)
        opaque_q = opaque_img.quantize(colors=15, method=Image.MEDIANCUT)
        pal = opaque_q.getpalette()[:45]
    else:
        pal = [0] * 45

    # 팔레트: 인덱스 0 = 마젠타(투명), 1~15 = 실제 색상
    final_pal = [255, 0, 255] + pal

    # 3) 각 프레임에 공통 팔레트 적용 + 투명 강제 인덱스 0
    for i, (src_name, dst_name, size) in enumerate(frames):
        dst_path = os.path.join(GFX, dst_name)

        ref = Image.new('P', (1, 1))
        ref.putpalette(final_pal + [0] * (768 - len(final_pal)))
        img_q = rgb_imgs[i].quantize(palette=ref, dither=Image.Dither.NONE)

        pixels = list(img_q.getdata())
        for j in range(len(pixels)):
            if not alpha_masks[i][j]:
                pixels[j] = 0

        result = Image.new('P', size)
        result.putpalette(final_pal + [0] * (768 - len(final_pal)))
        result.putdata(pixels)
        result.save(dst_path)
        print(f"  SPR: {src_name} -> {dst_name} ({size[0]}x{size[1]})")


def create_number_font():
    """숫자 0~9 폰트 타일 생성 (8x8 각 글자, 80x8 이미지)"""
    dst_path = os.path.join(GFX, 'font_numbers.png')
    # 80x8 이미지 (10개 숫자 * 8px)
    im = Image.new('RGB', (80, 8), (255, 0, 255))
    draw = ImageDraw.Draw(im)

    for i in range(10):
        x = i * 8
        # 간단한 비트맵 숫자
        draw_digit(draw, x, 0, str(i))

    im = im.quantize(colors=16, method=Image.MEDIANCUT)
    im.save(dst_path)
    print(f"  FONT: font_numbers.png (80x8)")
    return dst_path

def draw_digit(draw, x, y, digit):
    """간단한 3x5 비트맵 숫자 그리기 (8x8 타일 내부, 가운데 정렬)"""
    patterns = {
        '0': ['111','101','101','101','111'],
        '1': ['010','110','010','010','111'],
        '2': ['111','001','111','100','111'],
        '3': ['111','001','111','001','111'],
        '4': ['101','101','111','001','001'],
        '5': ['111','100','111','001','111'],
        '6': ['111','100','111','101','111'],
        '7': ['111','001','001','001','001'],
        '8': ['111','101','111','101','111'],
        '9': ['111','101','111','001','111'],
    }
    pat = patterns.get(digit, patterns['0'])
    ox, oy = x + 2, y + 1  # 8x8 타일 내 센터링
    for row, line in enumerate(pat):
        for col, ch in enumerate(line):
            if ch == '1':
                draw.point((ox + col, oy + row), fill=(255, 255, 255))

def convert_grade_images():
    """등급 이미지 21개를 1bpp C 배열로 변환 → data/grade_images.h"""
    dst_path = os.path.join(DATA, 'grade_images.h')

    # grade_index 순서: 0=d100(img 123), 1=d10(121), ..., 20=g10(83)
    GRADE_COUNT = 21
    IMG_W = 72  # 71 → 72 (8의 배수로 패딩, Mode 4 편의)
    IMG_H = 15

    lines = []
    lines.append('#ifndef GRADE_IMAGES_H')
    lines.append('#define GRADE_IMAGES_H')
    lines.append('')
    lines.append(f'#define GRADE_IMG_W {IMG_W}')
    lines.append(f'#define GRADE_IMG_H {IMG_H}')
    lines.append('')

    row_bytes = IMG_W  # 8bpp: 1바이트/픽셀
    all_data = []

    for gi in range(GRADE_COUNT):
        img_num = 123 - gi * 2
        src_path = os.path.join(SRC, f'{img_num}.png')
        im = Image.open(src_path).convert('RGBA')

        # 72xH로 리사이즈 (원본 71x15 → 72x15, 오른쪽 1px 패딩)
        padded = Image.new('RGBA', (IMG_W, IMG_H), (0, 0, 0, 0))
        padded.paste(im, (0, 0))

        # 8bpp 인덱스 데이터 생성: 불투명 픽셀=1, 투명=0
        pixels = []
        for y in range(IMG_H):
            for x in range(IMG_W):
                r, g, b, a = padded.getpixel((x, y))
                pixels.append(1 if a > 128 else 0)
        all_data.append(pixels)
        print(f"  GRADE: {img_num}.png -> index {gi}")

    # 전체 데이터를 하나의 배열로 합치기
    lines.append(f'/* 등급 이미지 {GRADE_COUNT}개, {IMG_W}x{IMG_H}, 8bpp 인덱스(0/1) */')
    lines.append(f'static const unsigned char grade_image_data[{GRADE_COUNT}][{IMG_W * IMG_H}] = {{')
    for gi in range(GRADE_COUNT):
        lines.append(f'    {{ /* grade {gi} (img {123-gi*2}) */')
        data = all_data[gi]
        for row in range(IMG_H):
            start = row * IMG_W
            row_data = data[start:start + IMG_W]
            hex_str = ','.join(f'{b}' for b in row_data)
            lines.append(f'        {hex_str},')
        lines.append('    },')
    lines.append('};')
    lines.append('')
    lines.append('#endif /* GRADE_IMAGES_H */')

    with open(dst_path, 'w') as f:
        f.write('\n'.join(lines) + '\n')
    print(f"  -> {dst_path} ({os.path.getsize(dst_path)} bytes)")

def main():
    print("=== 에셋 변환 시작 ===\n")

    # 배경 (Mode 4, 8bpp)
    print("[배경]")
    convert_bg('8.jpg', 'bg_title.png')
    day_path = convert_bg('74.jpg', 'bg_day.png')
    convert_bg('69.jpg', 'bg_night.png')
    convert_bg('71.jpg', 'bg_matrix.png')

    # 스프라이트 (OAM, 4bpp)
    print("\n[스프라이트]")
    # 플레이어 걷기 3프레임 + 사망 (공유 팔레트)
    convert_player_sprites()
    # 고양이 16x32 (48.png = 낙하 중 고양이, tall OAM)
    convert_sprite('48.png', 'spr_cat_white.png', (16, 32))
    convert_sprite('48.png', 'spr_cat_brown.png', (16, 32))
    # 고양이 앉은 자세 16x16 (41.png)
    convert_sprite('41.png', 'spr_cat_sit.png', (16, 16))
    # 아이템 16x16 (37=HP업, 43=폭탄)
    convert_sprite('37.png', 'spr_item_hp.png', (16, 16))
    convert_sprite('43.png', 'spr_item_bomb.png', (16, 16))
    # 폭발 32x32
    convert_sprite('52.png', 'spr_explosion.png', (32, 32))
    # 캐릭터 얼굴 16x16 (32x32 → 16x16 축소, HUD용)
    convert_sprite('28.png', 'spr_face_happy.png', (16, 16))
    convert_sprite('30.png', 'spr_face_normal.png', (16, 16))
    convert_sprite('32.png', 'spr_face_hurt.png', (16, 16))
    convert_sprite('34.png', 'spr_face_dead.png', (16, 16))

    # 폰트
    print("\n[폰트]")
    create_number_font()

    # 등급 이미지
    print("\n[등급 이미지]")
    convert_grade_images()

    print("\n=== 변환 완료 ===")
    print(f"graphics/ 에 저장됨. 이후 grit으로 data/*.c 생성 필요.")

if __name__ == '__main__':
    main()
