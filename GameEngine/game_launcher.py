#!/usr/bin/env python3
"""
🎮 EDUENGINE GAME LAUNCHER - Complete Instructions
Run educational game scenes with physics, collectibles, and learning content

For Judges/Demo:
- No API keys needed to run pre-generated scenes
- All scenes are stored in GameEngine/data/scenes/
- Ready-to-play immediately
"""

import os
import sys
import json
import subprocess
from pathlib import Path

def print_banner(title):
    """Print a formatted banner"""
    print("\n" + "="*80)
    print(f"  🎮 {title}")
    print("="*80)

def print_section(title):
    """Print a section header"""
    print(f"\n{'─'*80}")
    print(f"  📌 {title}")
    print(f"{'─'*80}\n")

def list_available_scenes(scenes_dir):
    """List all available scene files"""
    scenes = []
    if scenes_dir.exists():
        for scene_file in sorted(scenes_dir.glob("*.json")):
            with open(scene_file, 'r') as f:
                scene = json.load(f)
                meta = scene.get("scene_meta", {})
                scenes.append({
                    "file": scene_file.name,
                    "path": str(scene_file),
                    "title": meta.get("title", "Unknown"),
                    "grade": meta.get("grade", "?"),
                    "entities": len(scene.get("entities", []))
                })
    return scenes

def show_scene_info(scene_path):
    """Display detailed info about a scene"""
    with open(scene_path, 'r') as f:
        scene = json.load(f)
    
    meta = scene.get("scene_meta", {})
    world = scene.get("world_settings", {})
    entities = scene.get("entities", [])
    
    print(f"\n  📋 Scene Details:")
    print(f"     Title: {meta.get('title', 'Unknown')}")
    print(f"     Subject: {meta.get('subject', 'Unknown')}")
    print(f"     Grade: {meta.get('grade', 'Unknown')}")
    print(f"     Total Entities: {len(entities)}")
    print(f"     Gravity: {world.get('gravity', 980)} units/s²")
    
    # Count entity types
    entity_counts = {}
    collectibles = []
    for entity in entities:
        tags = entity.get("tags", [])
        if tags:
            tag = tags[0]
            entity_counts[tag] = entity_counts.get(tag, 0) + 1
            if tag == "collectible":
                collectibles.append(entity.get("name", "Unknown"))
    
    print(f"\n  📊 Entity Breakdown:")
    for tag, count in sorted(entity_counts.items()):
        print(f"     {tag.capitalize()}s: {count}")
    
    if collectibles:
        print(f"\n  ✨ Collectibles to gather:")
        for coll in collectibles:
            print(f"     • {coll}")
    
    print()

def get_engine_path():
    """Find the GameEngine executable"""
    base_dir = Path(__file__).parent
    
    # Possible locations for compiled engine
    possible_paths = [
        base_dir / "build" / "Debug" / "EduEngine",
        base_dir / "build" / "Release" / "EduEngine",
        base_dir / "build" / "EduEngine",
        base_dir / "EduEngine",
    ]
    
    for path in possible_paths:
        if path.exists():
            return path
    
    return None

def run_game_with_scene(scene_path, engine_path=None):
    """Launch the game with a specific scene"""
    if not Path(scene_path).exists():
        print(f"❌ Scene file not found: {scene_path}")
        return False
    
    # Find engine
    if engine_path is None:
        engine_path = get_engine_path()
    
    if engine_path is None:
        print(f"""
❌ GameEngine executable not found!

Possible locations checked:
  • GameEngine/build/Debug/EduEngine
  • GameEngine/build/Release/EduEngine
  • GameEngine/build/EduEngine
  • GameEngine/EduEngine

BUILD INSTRUCTIONS:
  1. Navigate to GameEngine directory
  2. mkdir build && cd build
  3. cmake .. -DCMAKE_BUILD_TYPE=Release
  4. make -j4

Or use quickstart script:
  cd GameEngine && bash setup.sh
        """)
        return False
    
    print(f"\n🚀 Launching EduEngine...")
    print(f"   Engine: {engine_path}")
    print(f"   Scene: {Path(scene_path).name}")
    print(f"\n   ▶ Starting game (press ESC to exit)...\n")
    
    try:
        env = os.environ.copy()
        env['SCENE_FILE'] = str(scene_path)
        
        # Launch the game engine
        result = subprocess.run(
            [str(engine_path), str(scene_path)],
            env=env,
            cwd=str(Path(engine_path).parent)
        )
        
        return result.returncode == 0
    
    except Exception as e:
        print(f"❌ Failed to launch game: {e}")
        return False

def interactive_menu():
    """Interactive scene selection menu"""
    base_dir = Path(__file__).parent
    scenes_dir = base_dir / "data" / "scenes"
    
    print_banner("EDUENGINE SCENE LAUNCHER - DEMO MODE (No API Keys Needed!)")
    
    # List available scenes
    scenes = list_available_scenes(scenes_dir)
    
    if not scenes:
        print(f"❌ No scenes found in {scenes_dir}")
        return
    
    print_section("Available Learning Games")
    for i, scene in enumerate(scenes, 1):
        print(f"  {i}. {scene['title']}")
        print(f"     File: {scene['file']} | Grade: {scene['grade']} | Entities: {scene['entities']}")
    
    print(f"\n  0. Exit")
    
    # Get user selection
    while True:
        try:
            choice = input("\n  Select scene (0-{}): ".format(len(scenes)))
            choice = int(choice)
            
            if choice == 0:
                print("  👋 Goodbye!")
                return
            
            if 1 <= choice <= len(scenes):
                break
            else:
                print(f"  ❌ Invalid choice. Enter 0-{len(scenes)}")
        except ValueError:
            print(f"  ❌ Invalid input. Enter a number 0-{len(scenes)}")
    
    selected_scene = scenes[choice - 1]
    
    print_section(f"📖 {selected_scene['title']}")
    show_scene_info(selected_scene['path'])
    
    # Confirm launch
    confirm = input("  Launch this game? (yes/no): ").strip().lower()
    if confirm in ['yes', 'y']:
        success = run_game_with_scene(selected_scene['path'])
        if success:
            print("\n✅ Game closed successfully!")
        else:
            print("\n⚠️ Game closed. Check error messages above.")
    else:
        print("  ❌ Cancelled.")

def direct_launch(scene_name_or_path):
    """Launch a specific scene directly"""
    base_dir = Path(__file__).parent
    scenes_dir = base_dir / "data" / "scenes"
    
    # Try as filename first
    scene_path = scenes_dir / scene_name_or_path
    if not scene_path.exists():
        # Try as full path
        scene_path = Path(scene_name_or_path)
    
    if not scene_path.exists():
        print(f"❌ Scene not found: {scene_name_or_path}")
        print(f"\nAvailable scenes:")
        scenes = list_available_scenes(scenes_dir)
        for scene in scenes:
            print(f"  • {scene['file']}")
        return
    
    print_banner("LAUNCHING GAME")
    show_scene_info(str(scene_path))
    run_game_with_scene(str(scene_path))

def show_instructions():
    """Show detailed instructions"""
    print_banner("EDUENGINE - COMPLETE INSTRUCTIONS")
    
    print("""
╔════════════════════════════════════════════════════════════════════════════╗
║                        🎮 GAME FEATURES                                     ║
╚════════════════════════════════════════════════════════════════════════════╝

✅ NO API Keys Required!
  - All scenes pre-generated and stored locally
  - Perfect for offline demo to judges
  - No cloud dependencies

🎯 Game Mechanics:
  • Platform Jumping - Navigate through platforms
  • Collectibles - Gather learning tokens (planets, laws, concepts, etc.)
  • Enemies - Avoid obstacles that represent resistance/friction/challenges
  • Info Zones - Interactive ? signs with educational facts
  • Goal Flag - Reach the end to complete the level

📚 Learning Features:
  • Theme-based scenes (Space, Photosynthesis, Physics, etc.)
  • Grade-appropriate difficulty scaling
  • Interactive educational facts
  • Progress tracking through collectibles

🎮 Controls:
  ← → or A/D   - Move left/right
  SPACE or W   - Jump
  ESC          - Exit game

╔════════════════════════════════════════════════════════════════════════════╗
║                    📁 AVAILABLE SCENE FILES                               ║
╚════════════════════════════════════════════════════════════════════════════╝
""")
    
    base_dir = Path(__file__).parent
    scenes_dir = base_dir / "data" / "scenes"
    scenes = list_available_scenes(scenes_dir)
    
    for scene in scenes:
        print(f"  📄 {scene['file']:<40} | {scene['title']}")
    
    print(f"""
╔════════════════════════════════════════════════════════════════════════════╗
║                       ⚙️ HOW TO RUN                                        ║
╚════════════════════════════════════════════════════════════════════════════╝

METHOD 1: Interactive Menu (Recommended)
  cd GameEngine/
  python3 game_launcher.py

  Then select a scene from the menu.

METHOD 2: Direct Launch
  python3 game_launcher.py <scene_filename>
  
  Example:
  python3 game_launcher.py newtons_laws_grade7.json
  python3 game_launcher.py space___solar_system_grade8.json

METHOD 3: Direct Engine Launch (if compiled)
  ./build/Release/EduEngine data/scenes/<scene_name>.json

╔════════════════════════════════════════════════════════════════════════════╗
║                    📋 JSON SCENE FORMAT (API Output)                      ║
╚════════════════════════════════════════════════════════════════════════════╝

All scenes follow this exact structure. This is what the AI-Agent generates:

{{
  "scene_meta": {{
    "id": "unique_scene_id",
    "title": "Scene Title — Grade X",
    "subject": "topic_name",
    "grade": "5",
    "ai_prompt": "original request"
  }},
  
  "world_settings": {{
    "gravity": 980.0,
    "background_color": [R, G, B, A]
  }},
  
  "entities": [
    {{
      "name": "EntityName",
      "tags": ["entity_type"],
      "components": {{
        "transform": {{ "x": 0, "y": 0, "scale_x": 1.0, "scale_y": 1.0 }},
        "sprite": {{ "texture": "path/to/image.png", "color": [R,G,B,A], "layer": 0 }},
        ... (other components as needed)
      }}
    }},
    ... more entities
  ]
}}

Entity Types:
  • background    - Static background image
  • player        - Controllable character (1 per scene)
  • platform      - Solid/one-way platforms
  • collectible   - Tokens to gather (is_trigger: true)
  • info          - Info signs with facts (is_trigger: true)
  • enemy         - Patrolling obstacles (has patrol_min_x, patrol_max_x)
  • goal          - Level completion flag (is_trigger: true)

Components:
  • transform     - Position and scale (x, y, scale_x, scale_y)
  • sprite        - Visual representation (texture, color, layer)
  • rigid_body    - Physics simulation (mass, gravity_scale)
  • collider      - Hit detection (width, height, is_trigger)
  • player_tag    - Player controls (move_speed, jump_force, max_jumps)
  • platform      - Platform behavior (is_one_way)
  • collectible   - Collectible properties (point_value, label)
  • info_zone     - Educational content (text, repeat flag)
  • enemy         - Enemy AI (patrol_min_x, patrol_max_x, speed)
  • goal          - Goal marker

╔════════════════════════════════════════════════════════════════════════════╗
║                    🔧 BUILDING THE GAME ENGINE                           ║
╚════════════════════════════════════════════════════════════════════════════╝

First, compile the C++ game engine:

  cd GameEngine/
  mkdir -p build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Release
  make -j4

Or use the automated setup:
  cd GameEngine/
  bash setup.sh

The compiled binary will be at: GameEngine/build/Release/EduEngine

╔════════════════════════════════════════════════════════════════════════════╗
║                    🌟 DEMO FOR JUDGES (NO API KEYS)                      ║
╚════════════════════════════════════════════════════════════════════════════╝

STEP 1: Navigate to GameEngine
  cd GameEngine/

STEP 2: Run the game launcher
  python3 game_launcher.py

STEP 3: Select a scene
  Choose from available educational games (Space, Physics, Photosynthesis, etc.)

STEP 4: Play the game
  Use arrow keys and space to play
  Collect all tokens and reach the goal!

NO INTERNET REQUIRED
NO API KEYS NEEDED
FULLY OFFLINE DEMO

Perfect for judges, presentations, and competitions!

╔════════════════════════════════════════════════════════════════════════════╗
║                    ❓ FAQ                                                   ║
╚════════════════════════════════════════════════════════════════════════════╝

Q: Do I need to run the AI-Agent or Backend to play?
A: NO! Scenes are pre-generated. Just launch the game with game_launcher.py

Q: Can I play offline?
A: YES! All scenes are stored locally in GameEngine/data/scenes/

Q: How do I generate new scenes?
A: Use the Backend API or orchestrator.py:
   python3 orchestrator.py workflow "teach [topic] for grade [X]"

Q: What does each game teach?
A: See the scene_meta.subject and description in each JSON file.

Q: Can I modify scenes?
A: YES! Edit the JSON files directly in GameEngine/data/scenes/

Q: How do I add custom assets?
A: Place images in GameEngine/assets/images/[theme]/ folder
   Update texture paths in scene JSON files

Q: Game won't start. What to do?
A: 1. Check if compiled: GameEngine/build/Release/EduEngine exists
   2. If not, run: cd GameEngine && bash setup.sh
   3. Verify scene file exists in GameEngine/data/scenes/

╔════════════════════════════════════════════════════════════════════════════╗
║                    📞 SUPPORT                                              ║
╚════════════════════════════════════════════════════════════════════════════╝

For technical details, see:
  • GameEngine/README.md
  • GameEngine/PROJECT_OVERVIEW.txt
  • GameEngine/SCENE_JSON_SPECIFICATION.txt

For AI scene generation:
  • Check AI-agents/QUICKSTART.md
  • Review orchestrator.py for workflow examples
""")

if __name__ == "__main__":
    base_dir = Path(__file__).parent
    
    if len(sys.argv) > 1:
        if sys.argv[1] == "help":
            show_instructions()
        elif sys.argv[1] == "list":
            scenes = list_available_scenes(base_dir / "data" / "scenes")
            print("\n📋 Available Scenes:\n")
            for scene in scenes:
                print(f"  • {scene['file']:<40} | {scene['title']}")
            print()
        else:
            # Try to launch specific scene
            direct_launch(sys.argv[1])
    else:
        # Show interactive menu
        interactive_menu()
