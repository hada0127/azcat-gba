"""
grit으로 graphics/ PNG → data/*.c, data/*.h 변환
"""
import os
import subprocess
import sys

GRIT = r'C:\devkitPro\tools\bin\grit.exe'
GFX = os.path.join(os.path.dirname(__file__), '..', 'graphics')
DATA = os.path.join(os.path.dirname(__file__), '..', 'data')

os.makedirs(DATA, exist_ok=True)

def run_grit(src, name, opts):
    src_path = os.path.join(GFX, src)
    if not os.path.exists(src_path):
        print(f"  SKIP: {src} not found")
        return

    cmd = [GRIT, src_path] + opts + ['-o', os.path.join(DATA, name)]
    print(f"  grit {src} -> {name}")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"  ERROR: {result.stderr}")
    else:
        # grit 출력 확인
        for ext in ['.c', '.h']:
            p = os.path.join(DATA, name + ext)
            if os.path.exists(p):
                sz = os.path.getsize(p)
                print(f"    {name}{ext} ({sz} bytes)")

def main():
    print("=== grit 변환 시작 ===\n")

    # 배경 (Mode 4, 8bpp 비트맵)
    # -gb: bitmap mode, -gB8: 8bpp, -p: palette
    # -ftc: output C array
    print("[배경 8bpp]")
    bg_opts = ['-gb', '-gB8', '-p', '-ftc']
    run_grit('bg_title.png', 'bg_title', bg_opts)
    run_grit('bg_day.png', 'bg_day', bg_opts)
    run_grit('bg_night.png', 'bg_night', bg_opts)
    run_grit('bg_matrix.png', 'bg_matrix', bg_opts)

    # 스프라이트 (OAM, 4bpp 타일)
    # -gB4: 4bpp, -Mh4: metatile 4 high (32x32→4 tiles high)
    # -gt: tile mode, -gT!: transparent color = first pixel (or magenta)
    # -p: palette, -ftc: C output
    print("\n[스프라이트 4bpp]")
    # 플레이어 걷기 32x32 + 사망 64x32
    spr32_opts = ['-gB4', '-gt', '-gTFF00FF', '-pn16', '-p', '-ftc']
    run_grit('spr_player_walk0.png', 'spr_player_walk0', spr32_opts)
    run_grit('spr_player_walk1.png', 'spr_player_walk1', spr32_opts)
    run_grit('spr_player_walk2.png', 'spr_player_walk2', spr32_opts)
    run_grit('spr_player_dead.png', 'spr_player_dead', spr32_opts)
    # 폭발 32x32
    run_grit('spr_explosion.png', 'spr_explosion', spr32_opts)

    # 16x16 스프라이트
    spr16_opts = ['-gB4', '-gt', '-gTFF00FF', '-pn16', '-p', '-ftc']
    run_grit('spr_cat_white.png', 'spr_cat_white', spr16_opts)
    run_grit('spr_cat_brown.png', 'spr_cat_brown', spr16_opts)
    run_grit('spr_item_hp.png', 'spr_item_hp', spr16_opts)
    run_grit('spr_item_bomb.png', 'spr_item_bomb', spr16_opts)
    run_grit('spr_face_happy.png', 'spr_face_happy', spr16_opts)
    run_grit('spr_face_normal.png', 'spr_face_normal', spr16_opts)
    run_grit('spr_face_hurt.png', 'spr_face_hurt', spr16_opts)
    run_grit('spr_face_dead.png', 'spr_face_dead', spr16_opts)

    # 폰트 (4bpp 타일, 8x8 per char)
    print("\n[폰트 4bpp]")
    font_opts = ['-gB4', '-gt', '-gTFF00FF', '-pn16', '-Mw1', '-Mh1', '-p', '-ftc']
    run_grit('font_numbers.png', 'font_numbers', font_opts)

    print("\n=== grit 변환 완료 ===")

if __name__ == '__main__':
    main()
