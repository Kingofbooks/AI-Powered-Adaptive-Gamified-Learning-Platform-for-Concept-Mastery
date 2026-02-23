#!/usr/bin/env python3
"""
generate_assets.py — EduEngine Procedural Sprite Generator
===========================================================
Generates themed PNG sprites for every educational subject using Pillow.
No internet required — everything is drawn programmatically.

Usage:
    python3 generate_assets.py --theme photosynthesis
    python3 generate_assets.py --all          # all 10 themes
    python3 generate_assets.py --list         # show available themes
    python3 generate_assets.py --theme space --out assets/images

Output layout:
    assets/images/<theme>/
        background.png      1280 × 720  – scene backdrop
        player.png            24 ×  32  – player character
        platform.png          32 ×  20  – tileable platform block
        enemy.png             30 ×  32  – patrol enemy
        goal.png              40 ×  60  – level-end flag
        collectible_0.png     28 ×  28  – pickup item (one per theme collectible)
        collectible_1.png     ...
"""

import argparse
import math
import os
import sys

try:
    from PIL import Image, ImageDraw, ImageFilter
except ImportError:
    sys.exit(
        "Pillow not installed.\n"
        "Run:  pip3 install Pillow\n"
        "Or:   sudo apt install python3-pil"
    )


# =============================================================================
# Colour helpers
# =============================================================================

def _c(v):
    """Clamp to 0-255."""
    return max(0, min(255, int(v)))

def lighter(rgb, by=50):
    return (_c(rgb[0] + by), _c(rgb[1] + by), _c(rgb[2] + by))

def darker(rgb, by=50):
    return (_c(rgb[0] - by), _c(rgb[1] - by), _c(rgb[2] - by))

def rgba(rgb, a=255):
    return (rgb[0], rgb[1], rgb[2], a)

def blend_row(top, bottom, t):
    """Linear interpolate two RGB colours."""
    return (_c(top[0] + (bottom[0] - top[0]) * t),
            _c(top[1] + (bottom[1] - top[1]) * t),
            _c(top[2] + (bottom[2] - top[2]) * t))


# =============================================================================
# Theme table  (colours mirror PromptSystem.cpp DetectTheme())
# =============================================================================

class Theme:
    def __init__(self, name, bg_top, bg_bottom, player,
                 platform, enemy, collectibles, bg_style="plain"):
        self.name         = name
        self.bg_top       = bg_top        # (r,g,b) sky
        self.bg_bottom    = bg_bottom     # (r,g,b) ground horizon
        self.player       = player        # (r,g,b) body suit
        self.platform     = platform      # (r,g,b) platform block
        self.enemy        = enemy         # (r,g,b) enemy body
        self.collectibles = collectibles  # list[(r,g,b)] one per pickup slot
        self.bg_style     = bg_style      # "stars" | "trees" | "clouds" | "plain"


THEMES = {
    "photosynthesis": Theme(
        "photosynthesis",
        bg_top=(8, 40, 12),    bg_bottom=(20, 80, 20),
        player=(60, 180, 80),  platform=(30, 90, 30),  enemy=(180, 60, 60),
        collectibles=[
            (255, 220,  50),   # Sunlight   – yellow
            (100, 180, 255),   # H₂O        – blue
            (180, 220, 255),   # CO₂        – pale blue
            (255, 140,  40),   # Glucose    – orange
            ( 80, 240, 100),   # O₂         – bright green
            ( 40, 200,  80),   # Chlorophyll– deep green
        ],
        bg_style="trees",
    ),
    "space": Theme(
        "space",
        bg_top=(2, 2, 20),      bg_bottom=(5, 5, 40),
        player=(180, 210, 255), platform=(40, 40, 100), enemy=(200, 80, 200),
        collectibles=[
            (200, 200, 200),   # Mercury – grey
            (255, 200,  80),   # Venus   – amber
            ( 60, 140, 255),   # Earth   – blue
            (220,  80,  60),   # Mars    – red
            (220, 160,  80),   # Jupiter – orange
            (255, 230, 120),   # Star    – gold
        ],
        bg_style="stars",
    ),
    "water_cycle": Theme(
        "water_cycle",
        bg_top=(10, 20, 50),   bg_bottom=(30, 60, 120),
        player=(100, 180, 255), platform=(30, 60, 120), enemy=(40, 80, 160),
        collectibles=[
            (200, 230, 255),   # Evaporation   – pale blue
            (160, 200, 240),   # Condensation  – mid blue
            ( 80, 140, 220),   # Precipitation – blue
            ( 40, 100, 200),   # Runoff        – deep blue
            (120, 210, 200),   # Transpiration – teal
        ],
        bg_style="clouds",
    ),
    "mathematics": Theme(
        "mathematics",
        bg_top=(20, 15, 30),   bg_bottom=(40, 25, 60),
        player=(200, 160, 255), platform=(60, 40, 100), enemy=(255, 100, 60),
        collectibles=[
            (255, 200,  50),   # π  – gold
            (180, 120, 255),   # ∑  – purple
            ( 80, 220, 180),   # √  – teal
            (255, 160,  80),   # 2² – orange
            (100, 200, 255),   # Δ  – sky blue
            (255, 100, 200),   # ∫  – pink
        ],
        bg_style="plain",
    ),
    "history": Theme(
        "history",
        bg_top=(40, 25, 10),   bg_bottom=(80, 50, 20),
        player=(220, 180, 120), platform=(80, 55, 25), enemy=(160, 60, 40),
        collectibles=[
            (255, 200,  40),   # Coin   – gold
            (200, 160,  80),   # Crown  – bronze
            (220, 200, 160),   # Scroll – parchment
            (140, 100,  60),   # Shield – dark gold
            (160,  60,  40),   # Sword  – red-brown
        ],
        bg_style="plain",
    ),
    "chemistry": Theme(
        "chemistry",
        bg_top=(10, 5, 25),    bg_bottom=(20, 10, 50),
        player=(80, 220, 200), platform=(60, 20, 80),  enemy=(220, 80, 80),
        collectibles=[
            (100, 220, 255),   # H₂O – cyan
            (160, 255, 200),   # O₂  – mint
            (255, 200,  80),   # NaCl– yellow
            (200, 100, 255),   # CO₂ – violet
            ( 80, 255, 160),   # Fe  – green
            (255, 240, 120),   # Au  – gold
        ],
        bg_style="plain",
    ),
    "physics": Theme(
        "physics",
        bg_top=(5, 10, 30),    bg_bottom=(15, 25, 60),
        player=(255, 180, 80), platform=(20, 50, 120), enemy=(80, 200, 255),
        collectibles=[
            (255, 160,  40),   # Force    – orange
            (255, 220,  40),   # Energy   – yellow
            ( 60, 200, 255),   # Velocity – electric blue
            (200, 200, 200),   # Mass     – grey
            (100, 255, 160),   # Wave     – green
        ],
        bg_style="plain",
    ),
    "biology": Theme(
        "biology",
        bg_top=(10, 25, 15),   bg_bottom=(20, 60, 25),
        player=(120, 220, 120), platform=(25, 70, 35), enemy=(180, 60, 80),
        collectibles=[
            (100, 255, 130),   # Cell    – bright green
            ( 60, 200, 120),   # DNA     – forest green
            (255, 180, 100),   # Protein – orange
            (180, 255, 200),   # Gene    – pale green
            ( 80, 220, 200),   # Enzyme  – teal
        ],
        bg_style="trees",
    ),
    "geography": Theme(
        "geography",
        bg_top=(5, 20, 40),    bg_bottom=(20, 80, 40),
        player=(180, 220, 160), platform=(20, 80, 40), enemy=(180, 100, 40),
        collectibles=[
            ( 60, 180, 100),   # Africa   – green
            ( 40, 120, 200),   # Ocean    – blue
            (220,  80,  40),   # Volcano  – red
            (200, 230, 255),   # Glacier  – ice white
            (180, 160, 100),   # Mountain – stone
        ],
        bg_style="clouds",
    ),
    "default": Theme(
        "default",
        bg_top=(18, 12, 30),   bg_bottom=(35, 25, 60),
        player=(100, 160, 255), platform=(55, 35, 90), enemy=(220, 80, 80),
        collectibles=[
            (255, 220,  80),   # A – gold
            ( 80, 220, 255),   # B – cyan
            (220,  80, 255),   # C – violet
            ( 80, 255, 160),   # D – green
        ],
        bg_style="plain",
    ),
}


# =============================================================================
# Sprite generators
# =============================================================================

def make_background(theme: Theme, w=1280, h=720) -> Image.Image:
    """Full-scene backdrop with gradient + theme-specific decorations."""
    img  = Image.new("RGB", (w, h))
    draw = ImageDraw.Draw(img)

    # ── Vertical gradient ────────────────────────────────────────────────────
    for y in range(h):
        t   = y / (h - 1)
        col = blend_row(theme.bg_top, theme.bg_bottom, t)
        draw.line([(0, y), (w, y)], fill=col)

    # ── Stars ─────────────────────────────────────────────────────────────────
    if theme.bg_style == "stars":
        import random
        rng = random.Random(99)
        for _ in range(420):
            sx = rng.randint(0, w - 1)
            sy = rng.randint(0, int(h * 0.70))
            br = rng.randint(140, 255)
            sz = rng.choice([1, 1, 1, 2])
            draw.ellipse([sx - sz, sy - sz, sx + sz, sy + sz], fill=(br, br, br))
        # A few bright coloured stars
        for cx, cy, cr, col in [(220, 80, 3, (255, 210, 80)),
                                 (640, 45, 4, (200, 220, 255)),
                                 (1050, 110, 3, (255, 180, 180))]:
            draw.ellipse([cx - cr, cy - cr, cx + cr, cy + cr], fill=col)

    # ── Clouds ────────────────────────────────────────────────────────────────
    elif theme.bg_style == "clouds":
        cc = (220, 235, 255)
        for (cx, cy, cr) in [(180, 80, 50), (460, 60, 65), (800, 100, 48), (1110, 78, 55)]:
            for (dx, dy, dr) in [(-cr//2, 0, cr//2+2), (0, -cr//3, cr//2+4),
                                  (cr//2, 0, cr//2+2), (0, 6, cr//2+7)]:
                draw.ellipse([cx+dx-dr, cy+dy-dr, cx+dx+dr, cy+dy+dr], fill=cc)

    # ── Trees ─────────────────────────────────────────────────────────────────
    elif theme.bg_style == "trees":
        tc = darker(theme.bg_bottom, 22)
        for tx in range(0, w, 130):
            th  = 70 + (tx * 41 % 52)
            ty  = h - 108 - (tx * 17 % 36)
            trx = tx + 12
            # trunk
            draw.rectangle([trx, ty + th - 18, trx + 8, ty + th], fill=tc)
            # canopy triangle
            draw.polygon([(tx, ty + th - 14), (tx + 16, ty), (tx + 32, ty + th - 14)], fill=tc)

    return img


def make_player(theme: Theme, w=24, h=32) -> Image.Image:
    """Humanoid character in theme body-colour, skin face."""
    img  = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    pc   = theme.player
    dark = darker(pc, 45)
    lit  = lighter(pc, 45)
    skin = (240, 195, 155)

    # Legs
    draw.rectangle([ 2, 24,  8, 31], fill=dark)
    draw.rectangle([15, 24, 21, 31], fill=dark)

    # Body
    draw.rounded_rectangle([3, 12, 20, 24], radius=3, fill=pc)
    draw.rectangle([3, 12, 9, 24], fill=lit)   # left highlight

    # Arms
    draw.rectangle([ 0, 13,  3, 21], fill=dark)
    draw.rectangle([20, 13, 23, 21], fill=dark)

    # Head
    draw.ellipse([4, 1, 20, 14], fill=skin)

    # Eyes
    draw.ellipse([ 6, 5, 10,  9], fill=(255, 255, 255))
    draw.ellipse([14, 5, 18,  9], fill=(255, 255, 255))
    draw.ellipse([ 7, 6,  9,  8], fill=(40, 40, 90))
    draw.ellipse([15, 6, 17,  8], fill=(40, 40, 90))

    # Smile
    draw.arc([8, 8, 16, 13], start=10, end=170, fill=(140, 80, 60), width=1)

    return img


def make_platform(theme: Theme, w=32, h=20) -> Image.Image:
    """Tileable platform block with top highlight and surface texture."""
    img  = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    pc   = theme.platform
    dark = darker(pc, 40)
    lit  = lighter(pc, 40)

    # Main body
    draw.rectangle([0, 3, w - 1, h - 1], fill=pc)

    # Top surface / grass strip
    draw.rectangle([0, 3, w - 1, 5], fill=lit)

    # Bottom shadow
    draw.rectangle([0, h - 2, w - 1, h - 1], fill=dark)

    # Vertical brick dividers
    for x in range(8, w, 8):
        draw.line([(x, 6), (x, h - 2)], fill=dark, width=1)

    # Left edge highlight
    draw.line([(0, 3), (0, h - 1)], fill=lit, width=1)

    return img


def make_collectible(color, idx: int, size=28) -> Image.Image:
    """Glowing orb with glow layer + inner highlight + dot index marker."""
    # Glow layer
    glow_img  = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    glow_draw = ImageDraw.Draw(glow_img)
    glow_draw.ellipse([1, 1, size - 1, size - 1], fill=rgba(color, 90))
    glow_img  = glow_img.filter(ImageFilter.GaussianBlur(3))

    img  = Image.alpha_composite(Image.new("RGBA", (size, size), (0, 0, 0, 0)), glow_img)
    draw = ImageDraw.Draw(img)

    m = 4
    # Main circle
    draw.ellipse([m, m, size - m, size - m], fill=rgba(color, 230))

    # Inner highlight
    hl_col = rgba(lighter(color, 70), 180)
    draw.ellipse([m + 3, m + 3, size - m - 4, size - m - 4], fill=hl_col)

    # Specular dot
    draw.ellipse([m + 3, m + 3, m + 7, m + 7], fill=(255, 255, 255, 200))

    # Index dot marker (1-5 dots so collectibles are visually distinct)
    dots     = (idx % 5) + 1
    cx, cy   = size // 2, size // 2 + 4
    spacing  = 4
    start_x  = cx - (dots - 1) * spacing // 2
    for d in range(dots):
        dx = start_x + d * spacing
        draw.ellipse([dx - 1, cy - 1, dx + 1, cy + 1], fill=(255, 255, 255, 160))

    return img


def make_enemy(theme: Theme, w=30, h=32) -> Image.Image:
    """Angular diamond-shaped enemy with menacing eyes."""
    img  = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    ec   = theme.enemy
    dark = darker(ec, 50)
    lit  = lighter(ec, 40)
    cx, cy = w // 2, h // 2

    # Outer diamond
    draw.polygon([(cx, 3), (w - 3, cy), (cx, h - 3), (3, cy)], fill=ec)

    # Upper lighter half
    draw.polygon([(cx, 6), (w - 7, cy), (cx, cy), (7, cy)], fill=lit)

    # Lower darker half
    draw.polygon([(cx, cy), (w - 7, cy), (cx, h - 6), (7, cy)], fill=dark)

    # Eyes
    for ex, ey in [(cx - 6, cy - 5), (cx + 6, cy - 5)]:
        draw.ellipse([ex - 3, ey - 3, ex + 3, ey + 3], fill=(255, 255, 255))
        draw.ellipse([ex - 1, ey - 1, ex + 1, ey + 1], fill=(20, 20, 20))

    # Angry brows
    draw.line([(cx - 9, cy - 9), (cx - 3, cy - 6)], fill=dark, width=2)
    draw.line([(cx + 3, cy - 6), (cx + 9, cy - 9)], fill=dark, width=2)

    return img


def make_goal(theme: Theme, w=40, h=60) -> Image.Image:
    """Flag on a pole with a star — marks the level exit."""
    img  = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Pole
    draw.rectangle([4, 6, 7, h - 1], fill=(200, 200, 200, 255))

    # Flag body
    flag_col = rgba(lighter(theme.platform, 80), 255)
    draw.polygon([(7, 6), (36, 18), (7, 30)], fill=flag_col)

    # Star on the flag
    cx, cy = 20, 18
    r1, r2 = 8, 4
    pts = []
    for i in range(10):
        angle = math.pi * i / 5 - math.pi / 2
        r     = r1 if i % 2 == 0 else r2
        pts.append((cx + r * math.cos(angle), cy + r * math.sin(angle)))
    draw.polygon(pts, fill=(255, 240, 40, 255))

    # Base block
    draw.rectangle([0, h - 6, 12, h - 1], fill=(120, 90, 60, 255))

    return img


# =============================================================================
# Theme generator — writes all sprites for one theme
# =============================================================================

def generate_theme(theme: Theme, out_base: str = "assets/images") -> str:
    theme_dir = os.path.join(out_base, theme.name)
    os.makedirs(theme_dir, exist_ok=True)

    files_written = []

    def save(img: Image.Image, name: str):
        img.save(os.path.join(theme_dir, name))
        files_written.append(name)

    save(make_background(theme), "background.png")
    save(make_player(theme),     "player.png")
    save(make_platform(theme),   "platform.png")
    save(make_enemy(theme),      "enemy.png")
    save(make_goal(theme),       "goal.png")

    for i, col in enumerate(theme.collectibles):
        save(make_collectible(col, i), f"collectible_{i}.png")

    total = len(files_written)
    print(f"[AssetGen] {theme.name:22s} → {theme_dir}/  ({total} files)")
    return theme_dir


# =============================================================================
# CLI
# =============================================================================

def main():
    ap = argparse.ArgumentParser(
        description="EduEngine procedural sprite generator",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""\
Examples:
  python3 generate_assets.py --theme photosynthesis
  python3 generate_assets.py --theme space
  python3 generate_assets.py --all
  python3 generate_assets.py --list
""",
    )
    ap.add_argument("--theme", metavar="NAME",  help="Generate one theme")
    ap.add_argument("--all",   action="store_true", help="Generate all themes")
    ap.add_argument("--list",  action="store_true", help="List available themes")
    ap.add_argument("--out",   default="assets/images", metavar="DIR",
                    help="Output base directory (default: assets/images)")
    args = ap.parse_args()

    if args.list:
        print("Available themes:")
        for name, t in THEMES.items():
            print(f"  {name:22s}  {len(t.collectibles)} collectibles  bg:{t.bg_style}")
        return

    if args.all:
        for t in THEMES.values():
            generate_theme(t, args.out)
        total_sprites = sum(len(t.collectibles) + 5 for t in THEMES.values())
        print(f"\nDone — {total_sprites} sprites across {len(THEMES)} themes → {args.out}/")
        return

    if args.theme:
        key = args.theme.lower().replace(" ", "_").replace("-", "_")
        if key not in THEMES:
            matches = [k for k in THEMES if key in k or k.startswith(key[:4])]
            if matches:
                key = matches[0]
                print(f"[AssetGen] Matched theme '{key}'")
            else:
                print(f"Unknown theme '{args.theme}'.")
                print(f"Available: {', '.join(THEMES)}")
                sys.exit(1)
        generate_theme(THEMES[key], args.out)
        return

    ap.print_help()


if __name__ == "__main__":
    main()
