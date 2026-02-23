#!/usr/bin/env bash
# =============================================================================
# setup.sh — One-shot project setup for EduEngine
# =============================================================================
# Usage:
#   chmod +x setup.sh
#   ./setup.sh
# =============================================================================

set -e

echo "=== EduEngine Setup ==="

# --- 1. System packages -------------------------------------------------------
echo "[1/5] Installing system packages..."
sudo apt-get update -qq
sudo apt-get install -y \
    build-essential \
    pkg-config \
    libsdl2-dev \
    libsdl2-ttf-dev \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    python3 \
    python3-pip \
    python3-pil

# --- 2. Pillow (Python sprite generator) -------------------------------------
echo "[2/5] Checking Pillow..."
if python3 -c "from PIL import Image" 2>/dev/null; then
    echo "    Pillow already available."
else
    # Try pip with --break-system-packages for modern Debian/Ubuntu
    pip3 install Pillow --quiet --break-system-packages 2>/dev/null || \
    pip3 install Pillow --quiet 2>/dev/null || \
    echo "    WARNING: Could not install Pillow via pip. Coloured shapes will be used instead."
fi

# --- 3. Font ------------------------------------------------------------------
echo "[3/5] Setting up font..."
mkdir -p assets/fonts
if [ ! -f "assets/fonts/OpenSans-Regular.ttf" ]; then
    FONT_SRC=""
    for f in \
        /usr/share/fonts/truetype/ibm-plex/IBMPlexSans-Regular.ttf \
        /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf \
        /usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf \
        /usr/share/fonts/truetype/freefont/FreeSans.ttf; do
        if [ -f "$f" ]; then FONT_SRC="$f"; break; fi
    done

    if [ -n "$FONT_SRC" ]; then
        cp "$FONT_SRC" assets/fonts/OpenSans-Regular.ttf
        echo "    Copied: $FONT_SRC"
    else
        echo "    WARNING: No system TTF found — HUD text disabled."
        echo "    Copy any .ttf to assets/fonts/OpenSans-Regular.ttf to enable it."
    fi
else
    echo "    Font already present."
fi

# --- 4. Generate sprites + audio for all themes -------------------------------
echo "[4/5] Generating sprites and audio (all 10 themes)..."
if python3 -c "from PIL import Image" 2>/dev/null; then
    python3 generate_assets.py --all
else
    echo "    Sprites skipped (Pillow not available). Run later: python3 generate_assets.py --all"
fi
python3 generate_audio.py --all
echo "    Audio done (uses only built-in Python — no extra packages needed)."

# --- 5. Build -----------------------------------------------------------------
echo "[5/5] Building EduEngine..."
IMGUI_DIR="vendor/imgui"
SDL_FLAGS=$(pkg-config --cflags sdl2 SDL2_ttf SDL2_image SDL2_mixer)
SDL_LIBS=$(pkg-config --libs sdl2 SDL2_ttf SDL2_image SDL2_mixer)

g++ -std=c++17 -O2 -Wall -Wno-unused-parameter \
    $SDL_FLAGS \
    -Isrc -Ivendor \
    -I${IMGUI_DIR} -I${IMGUI_DIR}/backends \
    src/main.cpp \
    src/core/Engine.cpp \
    src/scene/SceneLoader.cpp \
    src/editor/EditorTools.cpp \
    src/prompt/PromptSystem.cpp \
    src/systems/TriggerSystem.cpp \
    src/audio/AudioPlayer.cpp \
    ${IMGUI_DIR}/imgui.cpp \
    ${IMGUI_DIR}/imgui_draw.cpp \
    ${IMGUI_DIR}/imgui_widgets.cpp \
    ${IMGUI_DIR}/imgui_tables.cpp \
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp \
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp \
    -o EduEngine $SDL_LIBS -lpthread

echo ""
echo "=== Setup complete! ==="
echo ""
echo "Run:  ./EduEngine"
echo ""
echo "Controls:"
echo "  A / D or Arrow keys  — Move player"
echo "  Space / W / Up       — Jump (double-jump)"
echo "  F5                   — Hot-reload scene"
echo ""
echo "Tip: in Prompt Mode, type a subject description and click Generate Game."
echo "     New sprites are generated automatically for each theme."
