#!/bin/bash
# 🎮 EduEngine - Quick Start Runner
# Generates a scene with AI and launches the game

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
GAME_ENGINE_DIR="$PROJECT_ROOT/GameEngine"

echo "🎮 ═══════════════════════════════════════════════════════════════════"
echo "   EduEngine - AI Scene Generator & Game Launcher"
echo "═══════════════════════════════════════════════════════════════════"
echo ""

# Check if prompt is provided
if [ -z "$1" ]; then
    echo "Usage: ./run_game.sh \"your prompt here\""
    echo ""
    echo "Examples:"
    echo "  ./run_game.sh \"teach photosynthesis for grade 5\""
    echo "  ./run_game.sh \"space adventure for grade 3\""
    echo "  ./run_game.sh \"biology cells\""
    echo ""
    exit 1
fi

PROMPT="$1"

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "❌ ERROR: Python 3 is not installed"
    exit 1
fi

# Check if game engine exists
if [ ! -f "$GAME_ENGINE_DIR/EduEngine" ]; then
    echo "❌ ERROR: Game engine binary not found at $GAME_ENGINE_DIR/EduEngine"
    echo "   Please compile the game engine first"
    exit 1
fi

# Run the pipeline
echo "Starting pipeline with prompt: \"$PROMPT\""
echo ""
cd "$SCRIPT_DIR"
python3 run_game.py "$PROMPT"

exit_code=$?
exit $exit_code
