# azcat-gba

**Ah! Cat's manga daioh!** (아! 고양이를 피해라 대왕) — a Game Boy Advance port of a 2003 Flash game.

## About

Back in 2003, this game was originally made as a **Flash game** (GP32/PC) inspired by **Azumanga Daioh** (아즈망가 대왕). Players dodge cats raining from the sky in an increasingly frantic avoidance game.

In 2026, it has been rebuilt from scratch in C for the **Game Boy Advance**, faithfully recreating the original gameplay on real GBA hardware using devkitARM and libtonc.

## How to Play

### Objective

Dodge the cats falling from the sky. Each cat that lands on the ground scores you a point. Get hit and you lose a life. Survive as long as you can and aim for the highest rank!

### Controls

| Button | Action |
|--------|--------|
| D-Pad Left/Right | Move |
| R Button | Use bomb (if available) |
| START | Start game / Pause |
| A Button | Start game / Retry |
| B Button | Return to title (game over) |

### Items

Four types of items fall from the sky during gameplay:

| Item | Effect |
|------|--------|
| Health | Restores 1 HP (max 3). Bonus +5 score if already full. |
| Bomb | Grants a bomb. Press R to wipe all falling cats off screen. |
| Poison | Reduces 1 HP (won't kill you). -5 score if at minimum. |
| Speed Up | Increases movement speed. Bonus +5 score if maxed out. |

### Lives & HUD

You start with 3 lives. The character portrait in the top-right corner reflects your current health. The score is displayed at the top of the screen, along with a bomb icon when one is held.

### Difficulty Progression

The game gets harder as your score increases:

- More cats spawn simultaneously (up to 33 active at once)
- Cats fall faster (13 acceleration tiers)
- The background shifts from **day** to **night** (500+) to **matrix** (900+)

### Ranking System

When the game ends, you receive a rank based on your score — ranging from Grade 10 (beginner) all the way up to Grand Master (3000+ points). High scores are saved to SRAM.

## Building

### Requirements

- [devkitPro](https://devkitpro.org/) with devkitARM and libtonc
- grit (included with devkitPro)

### Build

```bash
make
```

This produces `azcat.gba`.

### Running

Use any GBA emulator (e.g., [mGBA](https://mgba.io/)) or flash to a GBA cartridge.

## Technical Details

- **Platform**: GBA (ARM7TDMI, 240x160)
- **Graphics**: Mode 4 double-buffered bitmap backgrounds + OAM hardware sprites
- **Sprites**: 46 OAM slots (33 cats + player + items + HUD + effects)
- **Audio**: Direct Sound double-buffered DMA (10512 Hz, BGM + 4 SFX channels)
- **Math**: Fixed-point arithmetic (.8 shift)
- **Save**: SRAM high score persistence
- **Language**: C (devkitARM + libtonc)

## Credits

- **Original Game** (2003, Flash/GP32): hada0127(taRu)
- **GBA Port** (2026): hada0127(taRu)

## License

This is a personal hobby project porting an original 2003 game to GBA hardware.
