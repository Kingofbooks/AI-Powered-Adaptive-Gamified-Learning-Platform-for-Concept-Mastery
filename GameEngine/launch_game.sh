#!/bin/bash
# Game Engine Launcher - Receives scene JSON and launches the game

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}================================================${NC}"
echo -e "${BLUE}   EduEngine Game Launcher${NC}"
echo -e "${BLUE}================================================${NC}"

# Get the script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
GAME_ENGINE_DIR="$SCRIPT_DIR"
GAME_BINARY="$GAME_ENGINE_DIR/EduEngine"
DATA_DIR="$GAME_ENGINE_DIR/data"
SCENES_DIR="$DATA_DIR/scenes"

echo -e "\n${YELLOW}Game Engine Path:${NC} $GAME_ENGINE_DIR"
echo -e "${YELLOW}Scenes Directory:${NC} $SCENES_DIR"

# Check if scene file is provided
if [ -z "$1" ]; then
    echo -e "\n${RED}ERROR: Scene file path not provided${NC}"
    echo -e "${YELLOW}Usage: ./launch_game.sh <scene_id_or_path>${NC}"
    echo -e "\nExample:"
    echo -e "  ./launch_game.sh photosynthesis_grade5"
    echo -e "  ./launch_game.sh data/scenes/photosynthesis_grade5.json"
    exit 1
fi

SCENE_INPUT="$1"

# Determine full scene path
if [[ "$SCENE_INPUT" == *.json ]]; then
    # Full path provided
    SCENE_PATH="$SCENE_INPUT"
else
    # Just scene ID provided, append .json
    SCENE_PATH="$SCENES_DIR/${SCENE_INPUT}.json"
fi

# Convert to absolute path
SCENE_PATH="$(cd "$(dirname "$SCENE_PATH")" && pwd)/$(basename "$SCENE_PATH")"

echo -e "\n${YELLOW}Checking requirements...${NC}"

# Check if game binary exists
if [ ! -f "$GAME_BINARY" ]; then
    echo -e "${RED}ERROR: Game engine binary not found at: $GAME_BINARY${NC}"
    echo -e "${YELLOW}Please compile the game engine first${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Game binary found${NC}"

# Check if scene file exists
if [ ! -f "$SCENE_PATH" ]; then
    echo -e "${RED}ERROR: Scene file not found at: $SCENE_PATH${NC}"
    echo -e "${YELLOW}Available games:${NC}"
    if [ -d "$SCENES_DIR" ]; then
        ls -1 "$SCENES_DIR"/*.json 2>/dev/null | xargs -I {} basename {} .json | sed 's/^/  - /' || echo "  (none)"
    fi
    exit 1
fi
echo -e "${GREEN}✓ Scene file found${NC}"

# Parse scene metadata for user info
echo -e "\n${YELLOW}Scene Information:${NC}"
TITLE=$(grep -o '"title"[[:space:]]*:[[:space:]]*"[^"]*"' "$SCENE_PATH" | head -1 | cut -d'"' -f4)
SUBJECT=$(grep -o '"subject"[[:space:]]*:[[:space:]]*"[^"]*"' "$SCENE_PATH" | head -1 | cut -d'"' -f4)
GRADE=$(grep -o '"grade"[[:space:]]*:[[:space:]]*"[^"]*"' "$SCENE_PATH" | head -1 | cut -d'"' -f4)

echo -e "  Title:  ${GREEN}$TITLE${NC}"
echo -e "  Subject: ${GREEN}$SUBJECT${NC}"
echo -e "  Grade:  ${GREEN}$GRADE${NC}"

# Launch the game
echo -e "\n${BLUE}================================================${NC}"
echo -e "${BLUE}   Launching Game Engine...${NC}"
echo -e "${BLUE}================================================${NC}\n"

cd "$GAME_ENGINE_DIR"
exec "$GAME_BINARY" "$SCENE_PATH"
