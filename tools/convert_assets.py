"""
에셋 변환 스크립트
원본 이미지 → GBA용 리사이즈 → graphics/ 폴더에 저장
그 후 grit으로 data/*.c, data/*.h 생성
"""
import os
import sys
from PIL import Image, ImageDraw, ImageFont, ImageEnhance, ImageFilter

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

def resize_preserve_aspect(im, target_size, valign='bottom'):
    """종횡비 보존 리사이즈 → target_size 내에 센터링, 나머지 투명"""
    tw, th = target_size
    ow, oh = im.size
    scale = min(tw / ow, th / oh)
    nw = max(1, int(ow * scale))
    nh = max(1, int(oh * scale))
    resized = im.resize((nw, nh), Image.LANCZOS)
    result = Image.new('RGBA', target_size, (0, 0, 0, 0))
    ox = (tw - nw) // 2
    if valign == 'top':
        oy = 0
    elif valign == 'center':
        oy = (th - nh) // 2
    else:  # bottom
        oy = th - nh
    result.paste(resized, (ox, oy))
    return result

def pad_original(im, target_size, valign='bottom'):
    """원본 픽셀 사이즈 유지, OAM 캔버스에 패딩만 (스케일링 없음)."""
    tw, th = target_size
    ow, oh = im.size
    # 원본이 타겟보다 크면 종횡비 보존 리사이즈 (fallback)
    if ow > tw or oh > th:
        return resize_preserve_aspect(im, target_size, valign=valign)
    result = Image.new('RGBA', target_size, (0, 0, 0, 0))
    ox = (tw - ow) // 2
    if valign == 'top':
        oy = 0
    elif valign == 'center':
        oy = (th - oh) // 2
    else:  # bottom
        oy = th - oh
    result.paste(im, (ox, oy))
    return result

def convert_sprite(src_name, dst_name, size, transparent_bg=True, use_original=True, valign='bottom', crop_bottom=0):
    """스프라이트 변환 (4bpp, 16색). use_original=True면 원본 사이즈 유지(패딩만)"""
    src_path = os.path.join(SRC, src_name)
    dst_path = os.path.join(GFX, dst_name)
    im = Image.open(src_path).convert('RGBA')
    if crop_bottom > 0:
        im = im.crop((0, 0, im.width, im.height - crop_bottom))
    if use_original:
        im = pad_original(im, size, valign=valign)
    else:
        im = resize_preserve_aspect(im, size, valign=valign)

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
    """숫자 0~9 폰트 타일 생성 (16x16 흰색+검정테두리, 160x16 이미지)"""
    dst_path = os.path.join(GFX, 'font_numbers.png')
    DIGIT_W = 16
    DIGIT_H = 16

    font = None
    font_paths = [
        'C:/Windows/Fonts/malgunbd.ttf',
        'C:/Windows/Fonts/malgun.ttf',
    ]
    for fp in font_paths:
        if os.path.exists(fp):
            font = ImageFont.truetype(fp, 13)
            break
    if font is None:
        font = ImageFont.load_default()

    # 마젠타 배경 (grit 투명)
    result = Image.new('RGB', (DIGIT_W * 10, DIGIT_H), (255, 0, 255))

    for d in range(10):
        im = Image.new('L', (DIGIT_W, DIGIT_H), 0)
        draw = ImageDraw.Draw(im)
        text = str(d)
        bbox = draw.textbbox((0, 0), text, font=font)
        tw = bbox[2] - bbox[0]
        th = bbox[3] - bbox[1]
        tx = (DIGIT_W - tw) // 2 - bbox[0]
        ty = (DIGIT_H - th) // 2 - bbox[1]
        draw.text((tx, ty), text, fill=255, font=font)

        dilated = im.filter(ImageFilter.MaxFilter(3))

        for y in range(DIGIT_H):
            for x in range(DIGIT_W):
                v = im.getpixel((x, y))
                dd = dilated.getpixel((x, y))
                px = d * DIGIT_W + x
                if v > 128:
                    result.putpixel((px, y), (255, 255, 255))  # 흰색 코어
                elif dd > 64:
                    result.putpixel((px, y), (0, 0, 0))  # 검정 테두리

    result.save(dst_path)
    print(f"  FONT: font_numbers.png ({DIGIT_W * 10}x{DIGIT_H})")
    return dst_path

def convert_grade_images():
    """등급 이미지 21개를 원본 색상으로 변환 → data/grade_images.h
    알파 채널로 투명 구분, 불투명 픽셀은 흰색/검정 2색 분류"""
    dst_path = os.path.join(DATA, 'grade_images.h')

    GRADE_COUNT = 21
    IMG_W = 72  # 71 → 72 (짝수 정렬)
    IMG_H = 15
    PAL_COLORS = 2  # 흰색 + 검정

    # GBA RGB15: 0=흰색, 1=검정
    pal_rgb15 = [0x7FFF, 0x0000]

    all_data = []
    for gi in range(GRADE_COUNT):
        img_num = 123 - gi * 2
        src_path = os.path.join(SRC, f'{img_num}.png')
        im = Image.open(src_path).convert('RGBA')

        # IMG_W x IMG_H 캔버스에 패딩 (원본 71x15 → 72x15)
        canvas = Image.new('RGBA', (IMG_W, IMG_H), (0, 0, 0, 0))
        canvas.paste(im, (0, 0))

        # 알파 기반 분류: 0=투명, 1=흰색, 2=검정
        pixels = []
        for r, g, b, a in canvas.getdata():
            if a <= 64:
                pixels.append(0)  # 투명
            elif r + g + b > 384:  # 밝은 색 (흰색 계열)
                pixels.append(1)  # 팔레트 0 = 흰색
            else:
                pixels.append(2)  # 팔레트 1 = 검정
        all_data.append(pixels)
        print(f"  GRADE: {img_num}.png -> index {gi}")

    # C 헤더 출력
    lines = []
    lines.append('#ifndef GRADE_IMAGES_H')
    lines.append('#define GRADE_IMAGES_H')
    lines.append('')
    lines.append(f'#define GRADE_IMG_W {IMG_W}')
    lines.append(f'#define GRADE_IMG_H {IMG_H}')
    lines.append(f'#define GRADE_PAL_COUNT {PAL_COLORS}')
    lines.append('')

    # 공유 팔레트 (GBA RGB15)
    lines.append('/* 등급 이미지 팔레트: 0=흰색, 1=검정 */')
    pal_str = ', '.join(f'0x{v:04X}' for v in pal_rgb15)
    lines.append(f'static const unsigned short grade_palette[{PAL_COLORS}] = {{ {pal_str} }};')
    lines.append('')

    # 픽셀 데이터 (0=투명, 1=흰색, 2=검정)
    lines.append(f'/* 등급 이미지 {GRADE_COUNT}개, {IMG_W}x{IMG_H} (0=투명, 1=흰색, 2=검정) */')
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
    """네비게이션 텍스트 비트맵 생성 → data/nav_text.h
    흰색 글씨 + 검정 테두리 (등급 이미지와 동일 스타일)"""
    dst_path = os.path.join(DATA, 'nav_text.h')

    NAV_W = 80
    NAV_H = 15
    texts = ['B 타이틀', '재도전 A']
    names = ['nav_title', 'nav_retry']

    font = None
    font_paths = [
        'C:/Windows/Fonts/malgunbd.ttf',
        'C:/Windows/Fonts/malgun.ttf',
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
        bbox = draw.textbbox((0, 0), text, font=font)
        tw = bbox[2] - bbox[0]
        th = bbox[3] - bbox[1]
        tx = (NAV_W - tw) // 2 - bbox[0]
        ty = (NAV_H - th) // 2 - bbox[1]
        draw.text((tx, ty), text, fill=255, font=font)

        # 팽창으로 검정 테두리 생성
        dilated = im.filter(ImageFilter.MaxFilter(3))

        # 0=투명, 1=흰색(코어), 2=검정(테두리)
        pixels = []
        for y in range(NAV_H):
            for x in range(NAV_W):
                v = im.getpixel((x, y))
                d = dilated.getpixel((x, y))
                if v > 128:
                    pixels.append(1)  # 흰색 코어
                elif d > 64:
                    pixels.append(2)  # 검정 테두리
                else:
                    pixels.append(0)  # 투명

        lines.append(f'/* "{text}" {NAV_W}x{NAV_H} (0=투명,1=흰색,2=검정) */')
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


def create_score_digits():
    """점수 숫자 0~9 (큰 사이즈, 흰색 + 검정 테두리) → data/score_digits.h"""
    dst_path = os.path.join(DATA, 'score_digits.h')

    DIGIT_W = 12  # 짝수 정렬 필수
    DIGIT_H = 18

    font = None
    font_paths = [
        'C:/Windows/Fonts/malgunbd.ttf',
        'C:/Windows/Fonts/malgun.ttf',
    ]
    for fp in font_paths:
        if os.path.exists(fp):
            font = ImageFont.truetype(fp, 16)
            break
    if font is None:
        font = ImageFont.load_default()

    lines = []
    lines.append('#ifndef SCORE_DIGITS_H')
    lines.append('#define SCORE_DIGITS_H')
    lines.append('')
    lines.append(f'#define SCORE_DIGIT_W {DIGIT_W}')
    lines.append(f'#define SCORE_DIGIT_H {DIGIT_H}')
    lines.append('')
    lines.append(f'/* 숫자 0~9 (0=투명,1=흰색,2=검정) */')
    lines.append(f'static const unsigned char score_digit_data[10][{DIGIT_W * DIGIT_H}] = {{')

    for d in range(10):
        text = str(d)
        im = Image.new('L', (DIGIT_W, DIGIT_H), 0)
        draw = ImageDraw.Draw(im)
        bbox = draw.textbbox((0, 0), text, font=font)
        tw = bbox[2] - bbox[0]
        th = bbox[3] - bbox[1]
        tx = (DIGIT_W - tw) // 2 - bbox[0]
        ty = (DIGIT_H - th) // 2 - bbox[1]
        draw.text((tx, ty), text, fill=255, font=font)

        dilated = im.filter(ImageFilter.MaxFilter(3))

        pixels = []
        for y in range(DIGIT_H):
            for x in range(DIGIT_W):
                v = im.getpixel((x, y))
                dd = dilated.getpixel((x, y))
                if v > 128:
                    pixels.append(1)  # 흰색 코어
                elif dd > 64:
                    pixels.append(2)  # 검정 테두리
                else:
                    pixels.append(0)  # 투명

        lines.append(f'    {{ /* {d} */')
        for row in range(DIGIT_H):
            start = row * DIGIT_W
            row_data = pixels[start:start + DIGIT_W]
            hex_str = ','.join(f'{b}' for b in row_data)
            lines.append(f'        {hex_str},')
        lines.append('    },')
        print(f"  DIGIT: {d} ({DIGIT_W}x{DIGIT_H})")

    lines.append('};')
    lines.append('')
    lines.append('#endif /* SCORE_DIGITS_H */')

    with open(dst_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(lines) + '\n')
    print(f"  -> {dst_path} ({os.path.getsize(dst_path)} bytes)")


def main():
    print("=== 에셋 변환 시작 ===\n")

    # 배경 (Mode 4, 8bpp)
    print("[배경]")
    convert_bg('8.jpg', 'bg_title.png')  # 타이틀: 전체 리사이즈 (UI 패널 없음)
    day_path = convert_bg('74.jpg', 'bg_day.png', crop_ui=True)   # 게임: UI 패널 크롭
    convert_bg('10.jpg', 'bg_bomb.png', crop_ui=True)            # 폭탄 전체화면 이펙트
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
    # 캐릭터 얼굴 32x64 TALL (HUD용, 원본 83x100 → 하단 20px 크롭 → 상단 정렬)
    convert_sprite('28.png', 'spr_face_happy.png', (32, 64), use_original=False, valign='top', crop_bottom=20)
    convert_sprite('30.png', 'spr_face_normal.png', (32, 64), use_original=False, valign='top', crop_bottom=20)
    convert_sprite('32.png', 'spr_face_hurt.png', (32, 64), use_original=False, valign='top', crop_bottom=20)
    convert_sprite('34.png', 'spr_face_dead.png', (32, 64), use_original=False, valign='top', crop_bottom=20)

    # 폰트
    print("\n[폰트]")
    create_number_font()

    # 등급 이미지
    print("\n[등급 이미지]")
    convert_grade_images()

    # 점수 숫자 (게임오버용 큰 사이즈)
    print("\n[점수 숫자]")
    create_score_digits()

    # 네비게이션 텍스트
    print("\n[네비게이션 텍스트]")
    create_nav_text()

    print("\n=== 변환 완료 ===")
    print(f"graphics/ 에 저장됨. 이후 grit으로 data/*.c 생성 필요.")

if __name__ == '__main__':
    main()
