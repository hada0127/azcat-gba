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

def convert_bg(src_name, dst_name, size=(240, 160)):
    """배경 이미지 변환 (240x160, 8bpp)"""
    src_path = os.path.join(SRC, src_name)
    dst_path = os.path.join(GFX, dst_name)
    im = Image.open(src_path).convert('RGB')
    im = im.resize(size, Image.LANCZOS)
    # RGB 모드 그대로 저장 (grit이 직접 256색 양자화)
    im.save(dst_path)
    print(f"  BG: {src_name} -> {dst_name} ({size[0]}x{size[1]})")
    return dst_path

def make_night_bg(day_path, dst_name):
    """낮 배경에서 밤 배경 생성 (어둡게 + 푸른색 톤)"""
    dst_path = os.path.join(GFX, dst_name)
    im = Image.open(day_path).convert('RGB')
    # 밝기 줄이기
    enhancer = ImageEnhance.Brightness(im)
    im = enhancer.enhance(0.4)
    # 푸른색 톤 추가
    r, g, b = im.split()
    from PIL import ImageChops
    r = r.point(lambda x: int(x * 0.6))
    g = g.point(lambda x: int(x * 0.7))
    b = b.point(lambda x: min(255, int(x * 1.3)))
    im = Image.merge('RGB', (r, g, b))
    # RGB 모드 그대로 저장 (grit이 직접 256색 양자화)
    im.save(dst_path)
    print(f"  BG: night from day -> {dst_name}")
    return dst_path

def convert_sprite(src_name, dst_name, size, transparent_bg=True):
    """스프라이트 변환 (4bpp, 16색)"""
    src_path = os.path.join(SRC, src_name)
    dst_path = os.path.join(GFX, dst_name)
    im = Image.open(src_path).convert('RGBA')
    im = im.resize(size, Image.LANCZOS)

    if transparent_bg:
        # 투명 배경을 마젠타(#FF00FF)로 교체 (GBA 투명색)
        bg = Image.new('RGBA', im.size, (255, 0, 255, 255))
        # 알파 합성
        result = Image.new('RGBA', im.size, (255, 0, 255, 255))
        result.paste(im, (0, 0), im)
        im = result.convert('RGB')
    else:
        im = im.convert('RGB')

    # 16색으로 양자화 (마젠타 유지)
    im = im.quantize(colors=16, method=Image.MEDIANCUT)
    im.save(dst_path)
    print(f"  SPR: {src_name} -> {dst_name} ({size[0]}x{size[1]})")
    return dst_path

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

def main():
    print("=== 에셋 변환 시작 ===\n")

    # 배경 (Mode 4, 8bpp)
    print("[배경]")
    convert_bg('8.jpg', 'bg_title.png')
    day_path = convert_bg('69.jpg', 'bg_day.png')
    make_night_bg(day_path, 'bg_night.png')
    convert_bg('71.jpg', 'bg_matrix.png')

    # 스프라이트 (OAM, 4bpp)
    print("\n[스프라이트]")
    # 플레이어 32x32
    convert_sprite('56.png', 'spr_player.png', (32, 32))
    # 고양이 16x16
    convert_sprite('37.png', 'spr_cat_white.png', (16, 16))
    convert_sprite('39.png', 'spr_cat_brown.png', (16, 16))
    # 아이템 16x16
    convert_sprite('41.png', 'spr_item_hp.png', (16, 16))
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

    print("\n=== 변환 완료 ===")
    print(f"graphics/ 에 저장됨. 이후 grit으로 data/*.c 생성 필요.")

if __name__ == '__main__':
    main()
