#!/bin/bash
# EduEngine — Exported Game: learning_adventure_grade6
# Run this script to play the game.

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"
DISPLAY="${DISPLAY:-:0}" ./EduEngine "data/scenes/learning_adventure_grade6.json"
