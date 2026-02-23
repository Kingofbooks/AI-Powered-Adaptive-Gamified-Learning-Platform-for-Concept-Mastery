#!/usr/bin/env python3
"""
🎮 EduEngine AI-to-Game Pipeline
================================

This script generates an EduEngine scene using the AI agent and launches
the game engine with that scene. It's the bridge between AI generation
and game execution.

Usage:
    python run_game.py "teach photosynthesis for grade 5"
    python run_game.py "space adventure for grade 3" --grade 3
    python run_game.py "biology cells" --save-only
"""

import sys
import json
import os
import subprocess
import argparse
from pathlib import Path
from typing import Optional

# Add AI-agents to path
SCRIPT_DIR = Path(__file__).parent
GAME_ENGINE_DIR = SCRIPT_DIR.parent / "GameEngine"
DATA_SCENES_DIR = GAME_ENGINE_DIR / "data" / "scenes"

# Ensure scenes directory exists
DATA_SCENES_DIR.mkdir(parents=True, exist_ok=True)

# Import the AI agent
sys.path.insert(0, str(SCRIPT_DIR))
from teacher_ai_agent import TeacherAIAgent

def generate_scene(prompt: str, grade: Optional[str] = None) -> dict:
    """Generate a scene using the AI agent"""
    print(f"\n{'='*80}")
    print(f"🤖 AI SCENE GENERATION")
    print(f"{'='*80}")
    print(f"📝 Prompt: {prompt}")
    if grade:
        print(f"📊 Grade: {grade}")
    print(f"\n⏳ Generating scene JSON...")
    
    agent = TeacherAIAgent()
    scene = agent.generate_scene(prompt)
    
    return scene

def save_scene(scene: dict) -> str:
    """Save scene JSON to file"""
    scene_id = scene.get("scene_meta", {}).get("id", "scene")
    filename = f"{scene_id}.json"
    filepath = DATA_SCENES_DIR / filename
    
    with open(filepath, 'w') as f:
        json.dump(scene, f, indent=2)
    
    print(f"✅ Scene saved to: {filepath}")
    return str(filepath)

def validate_scene(scene: dict) -> bool:
    """Validate scene structure"""
    print(f"\n{'='*80}")
    print(f"✅ SCENE VALIDATION")
    print(f"{'='*80}\n")
    
    errors = []
    warnings = []
    
    # Check required top-level keys
    if "scene_meta" not in scene:
        errors.append("❌ Missing 'scene_meta'")
    if "world_settings" not in scene:
        errors.append("❌ Missing 'world_settings'")
    if "entities" not in scene:
        errors.append("❌ Missing 'entities'")
    
    # Check scene_meta
    if "scene_meta" in scene:
        meta = scene["scene_meta"]
        for field in ["id", "title", "subject", "grade", "ai_prompt"]:
            if field not in meta:
                errors.append(f"❌ Missing scene_meta.{field}")
    
    # Check entities
    entities = scene.get("entities", [])
    
    # Count required entity types
    backgrounds = len([e for e in entities if e.get("tags") == ["background"]])
    players = len([e for e in entities if e.get("tags") == ["player"]])
    grounds = len([e for e in entities if "platform" in e.get("tags", []) and e.get("name") == "Ground"])
    goals = len([e for e in entities if e.get("tags") == ["goal"]])
    
    if backgrounds != 1:
        errors.append(f"❌ Expected 1 Background, found {backgrounds}")
    if players != 1:
        errors.append(f"❌ Expected 1 Player, found {players}")
    if grounds != 1:
        errors.append(f"❌ Expected 1 Ground, found {grounds}")
    if goals != 1:
        errors.append(f"❌ Expected 1 Goal, found {goals}")
    
    # Check entity bounds
    for entity in entities:
        if "components" in entity and "transform" in entity["components"]:
            transform = entity["components"]["transform"]
            x = transform.get("x", 0)
            y = transform.get("y", 0)
            if x < 0 or x > 1280:
                warnings.append(f"⚠️  Entity '{entity.get('name')}' has x={x} (out of bounds)")
            if y < 0 or y > 720:
                warnings.append(f"⚠️  Entity '{entity.get('name')}' has y={y} (out of bounds)")
    
    # Print results
    if errors:
        print("ERRORS:")
        for error in errors:
            print(f"  {error}")
        return False
    
    if warnings:
        print("WARNINGS:")
        for warning in warnings:
            print(f"  {warning}")
    
    print("✅ Scene structure: VALID")
    print(f"✅ Total entities: {len(entities)}")
    
    # Entity breakdown
    platforms = len([e for e in entities if "platform" in e.get("tags", [])])
    collectibles = len([e for e in entities if e.get("tags") == ["collectible"]])
    enemies = len([e for e in entities if e.get("tags") == ["enemy"]])
    info_signs = len([e for e in entities if e.get("tags") == ["info"]])
    
    print(f"\n📊 Entity Breakdown:")
    print(f"  • Platforms:    {platforms}")
    print(f"  • Collectibles: {collectibles}")
    print(f"  • Enemies:      {enemies}")
    print(f"  • Info Signs:   {info_signs}")
    
    return True

def launch_game(scene_path: str) -> bool:
    """Launch the game engine with the given scene"""
    print(f"\n{'='*80}")
    print(f"🎮 LAUNCHING GAME ENGINE")
    print(f"{'='*80}\n")
    
    engine_binary = GAME_ENGINE_DIR / "EduEngine"
    
    if not engine_binary.exists():
        print(f"❌ ERROR: Game engine binary not found at {engine_binary}")
        print(f"   Please compile the game engine first")
        return False
    
    print(f"🎮 Engine binary: {engine_binary}")
    print(f"📁 Scene file:   {scene_path}")
    print(f"\n▶️  Starting game...\n")
    
    try:
        # Run the game engine with the scene file as an argument
        result = subprocess.run(
            [str(engine_binary), scene_path],
            cwd=str(GAME_ENGINE_DIR),
            check=False
        )
        return result.returncode == 0
    except Exception as e:
        print(f"❌ ERROR: Failed to launch game: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(
        description="Generate an EduEngine scene with AI and play it",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python run_game.py "teach photosynthesis for grade 5"
  python run_game.py "space adventure" --grade 3
  python run_game.py "biology cells" --save-only
  python run_game.py "math algebra" --validate-only
        """
    )
    
    parser.add_argument(
        "prompt",
        help="Natural language description of the game (e.g., 'photosynthesis for grade 5')"
    )
    parser.add_argument(
        "--grade",
        type=str,
        default=None,
        help="Override detected grade level (1-12)"
    )
    parser.add_argument(
        "--save-only",
        action="store_true",
        help="Generate and save scene but don't launch game"
    )
    parser.add_argument(
        "--validate-only",
        action="store_true",
        help="Validate an already-generated scene without running"
    )
    parser.add_argument(
        "--no-validate",
        action="store_true",
        help="Skip validation before launching"
    )
    
    args = parser.parse_args()
    
    try:
        # Step 1: Generate scene
        scene = generate_scene(args.prompt, args.grade)
        
        # Step 2: Validate scene
        if not args.no_validate:
            if not validate_scene(scene):
                print("\n⚠️  Scene validation failed. Some features may not work correctly.")
                if not input("\nContinue anyway? (y/n): ").lower().startswith('y'):
                    return False
        
        # Step 3: Save scene
        scene_path = save_scene(scene)
        
        # Step 4: Launch game (unless --save-only)
        if not args.save_only:
            success = launch_game(scene_path)
            if success:
                print(f"\n✅ Game closed successfully!")
            else:
                print(f"\n⚠️  Game closed with errors")
                return False
        else:
            print(f"\n✅ Scene saved! To play, run:")
            print(f"   python run_game.py \"{args.prompt}\"")
        
        return True
        
    except KeyboardInterrupt:
        print(f"\n\n⚠️  Interrupted by user")
        return False
    except Exception as e:
        print(f"\n❌ ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
