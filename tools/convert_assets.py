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

PLAY_AREA_RATIO = 220 / 320  # 원본 320x240에서 플레이 영역 비율 (좌측 220px)

def convert_bg(src_name, dst_name, size=(240, 160), crop_ui=False):
    """배경 이미지 변환: crop_ui=True면 UI 패널 제거 후 리사이즈"""
    src_path = os.path.join(SRC, src_name)
    dst_path = os.path.join(GFX, dst_name)
    im = Image.open(src_path).convert('RGB')
    w, h = im.size
    # 320x240 게임 배경은 오른쪽 ~100px이 UI 패널 → 왼쪽만 크롭
    if crop_ui and w > 220:
        crop_w = int(w * PLAY_AREA_RATIO)
        im = im.crop((0, 0, crop_w, h))
        print(f"  BG: {src_name} -> crop {crop_w}x{h} -> {dst_name} ({size[0]}x{size[1]})")
    else:
        print(f"  BG: {src_name} -> {dst_name} ({size[0]}x{size[1]})")
    result = im.resize(size, Image.LANCZOS)
    result.save(dst_path)
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

def pad_original(im, target_size):
    """원본 픽셀 사이즈 유지, OAM 캔버스에 패딩만 (스케일링 없음). 하단 중앙 정렬."""
    tw, th = target_size
    ow, oh = im.size
    # 원본이 타겟보다 크면 종횡비 보존 리사이즈 (fallback)
    if ow > tw or oh > th:
        return resize_preserve_aspect(im, target_size)
    result = Image.new('RGBA', target_size, (0, 0, 0, 0))
    ox = (tw - ow) // 2
    oy = th - oh  # 하단 정렬
    result.paste(im, (ox, oy))
    return result

def convert_sprite(src_name, dst_name, size, transparent_bg=True, use_original=True):
    """스프라이트 변환 (4bpp, 16색). use_original=True면 원본 사이즈 유지(패딩만)"""
    src_path = os.path.join(SRC, src_name)
    dst_path = os.path.join(GFX, dst_name)
    im = Image.open(src_path).convert('RGBA')
    if use_original:
        im = pad_original(im, size)
    else:
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
    """플레이어 걷기 3프레임 + 사망 1프레임 (공유 15색 팔레트 + 인덱스 0 투명)
    원본 사이즈 유지: walk 30x53→32x64, dead 54x25→64x32"""
    frames = [
        ('56.png', 'spr_player_walk0.png', (32, 64)),
        ('61.png', 'spr_player_walk1.png', (32, 64)),
        ('63.png', 'spr_player_walk2.png', (32, 64)),
        ('66.png', 'spr_player_dead.png',  (64, 32)),
    ]

    # 1) RGBA 원본 사이즈 패딩 + 알파마스크 + RGB 합성
    alpha_masks = []
    rgb_imgs = []
    for src_name, _, size in frames:
        src_path = os.path.join(SRC, src_name)
        im = Image.open(src_path).convert('RGBA')
        im = pad_original(im, size)

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

        # 8bpp 인덱스 데이터 생성: 2=코어, 1=엣지, 0=투명
        pixels = []
        for y in range(IMG_H):
            for x in range(IMG_W):
                r, g, b, a = padded.getpixel((x, y))
                if a > 200:
                    pixels.append(2)
                elif a > 80:
                    pixels.append(1)
                else:
                    pixels.append(0)
        all_data.append(pixels)
        print(f"  GRADE: {img_num}.png -> index {gi}")

    # 전체 데이터를 하나의 배열로 합치기
    lines.append(f'/* 등급 이미지 {GRADE_COUNT}개, {IMG_W}x{IMG_H}, 8bpp (0=투명,1=엣지,2=코어) */')
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

def create_nav_text():
    """네비게이션 텍스트 "◀타이틀" / "재도전▶" 비트맵 생성 → data/nav_text.h"""
    dst_path = os.path.join(DATA, 'nav_text.h')

    NAV_W = 80
    NAV_H = 15
    texts = ['B 타이틀', '재도전 A']
    names = ['nav_title', 'nav_retry']

    # Windows 맑은고딕
    font = None
    font_paths = [
        'C:/Windows/Fonts/malgun.ttf',
        'C:/Windows/Fonts/malgunbd.ttf',
    ]
    for fp in font_paths:
        if os.path.exists(fp):
            font = ImageFont.truetype(fp, 12)
            break
    if font is None:
        font = ImageFont.load_default()
        print("  WARNING: malgun.ttf not found, using default font")

    lines = []
    lines.append('#ifndef NAV_TEXT_DATA_H')
    lines.append('#define NAV_TEXT_DATA_H')
    lines.append('')
    lines.append(f'#define NAV_TEXT_W {NAV_W}')
    lines.append(f'#define NAV_TEXT_HEIGHT {NAV_H}')
    lines.append('')

    for idx, (text, name) in enumerate(zip(texts, names)):
        im = Image.new('L', (NAV_W, NAV_H), 0)
        draw = ImageDraw.Draw(im)
        # 텍스트 크기 측정 후 중앙 정렬
        bbox = draw.textbbox((0, 0), text, font=font)
        tw = bbox[2] - bbox[0]
        th = bbox[3] - bbox[1]
        tx = (NAV_W - tw) // 2 - bbox[0]
        ty = (NAV_H - th) // 2 - bbox[1]
        draw.text((tx, ty), text, fill=255, font=font)

        # 2레벨 데이터 추출: 2=코어, 1=엣지, 0=투명
        pixels = []
        for y in range(NAV_H):
            for x in range(NAV_W):
                v = im.getpixel((x, y))
                if v > 180:
                    pixels.append(2)
                elif v > 60:
                    pixels.append(1)
                else:
                    pixels.append(0)

        lines.append(f'/* "{text}" {NAV_W}x{NAV_H}, 8bpp (0=투명,1=엣지,2=코어) */')
        lines.append(f'static const unsigned char {name}_data[{NAV_W * NAV_H}] = {{')
        for row in range(NAV_H):
            start = row * NAV_W
            row_data = pixels[start:start + NAV_W]
            hex_str = ','.join(f'{b}' for b in row_data)
            lines.append(f'    {hex_str},')
        lines.append('};')
        lines.append('')
        print(f"  NAV: \"{text}\" -> {name} ({NAV_W}x{NAV_H})")

    lines.append('#endif /* NAV_TEXT_DATA_H */')

    with open(dst_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')
    print(f"  -> {dst_path} ({os.path.getsize(dst_path)} bytes)")


def main():
    print("=== 에셋 변환 시작 ===\n")

    # 배경 (Mode 4, 8bpp)
    print("[배경]")
    convert_bg('8.jpg', 'bg_title.png')  # 타이틀: 전체 리사이즈 (UI 패널 없음)
    day_path = convert_bg('74.jpg', 'bg_day.png', crop_ui=True)   # 게임: UI 패널 크롭
    convert_bg('69.jpg', 'bg_night.png', crop_ui=True)
    convert_bg('71.jpg', 'bg_matrix.png', crop_ui=True)

    # 스프라이트 (OAM, 4bpp) — 원본 픽셀 사이즈 유지
    print("\n[스프라이트]")
    # 플레이어 걷기 3프레임(~30x53→32x64) + 사망(54x25→64x32) (공유 팔레트)
    convert_player_sprites()
    # 고양이 32x32 (48.png 원본 17x32, 50.png 원본 17x32)
    convert_sprite('48.png', 'spr_cat_white.png', (32, 32))
    convert_sprite('48.png', 'spr_cat_brown.png', (32, 32))
    convert_sprite('50.png', 'spr_cat_sit.png', (32, 32))
    # 아이템 32x32 (원본 각 20x22)
    convert_sprite('37.png', 'spr_item_hp.png', (32, 32))      # i1: HP 회복
    convert_sprite('39.png', 'spr_item_bomb.png', (32, 32))     # i2: 폭탄
    convert_sprite('41.png', 'spr_item_poison.png', (32, 32))   # i3: HP-1
    convert_sprite('43.png', 'spr_item_speed.png', (32, 32))    # i4: 스피드업
    # 폭발 32x32 (원본 29x26)
    convert_sprite('52.png', 'spr_explosion.png', (32, 32))
    # 캐릭터 얼굴 32x32 (HUD용, 원본 83x100 → OAM 최대 64x64이므로 축소)
    convert_sprite('28.png', 'spr_face_happy.png', (32, 32), use_original=False)
    convert_sprite('30.png', 'spr_face_normal.png', (32, 32), use_original=False)
    convert_sprite('32.png', 'spr_face_hurt.png', (32, 32), use_original=False)
    convert_sprite('34.png', 'spr_face_dead.png', (32, 32), use_original=False)

    # 폰트
    print("\n[폰트]")
    create_number_font()

    # 등급 이미지
    print("\n[등급 이미지]")
    convert_grade_images()

    # 네비게이션 텍스트
    print("\n[네비게이션 텍스트]")
    create_nav_text()

    print("\n=== 변환 완료 ===")
    print(f"graphics/ 에 저장됨. 이후 grit으로 data/*.c 생성 필요.")

if __name__ == '__main__':
    main()
