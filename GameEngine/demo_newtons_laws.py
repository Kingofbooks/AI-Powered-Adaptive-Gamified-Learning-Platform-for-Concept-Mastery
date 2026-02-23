#!/usr/bin/env python3
"""
Newton's 3 Laws of Motion - Scene Demo
Displays the scene structure and learning content
"""

import json
import sys
from pathlib import Path

# Add the backend path for helper functions
sys.path.insert(0, str(Path(__file__).parent.parent.parent / "Backend"))

def load_scene(scene_path):
    """Load and parse the scene JSON"""
    with open(scene_path, 'r') as f:
        return json.load(f)

def print_header(text):
    """Print a formatted header"""
    print("\n" + "="*70)
    print(f"  {text}")
    print("="*70)

def print_subheader(text):
    """Print a formatted subheader"""
    print(f"\n--- {text} ---")

def demo_scene(scene_path):
    """Run the demo"""
    scene = load_scene(scene_path)
    
    # Scene Meta
    meta = scene["scene_meta"]
    print_header(f"🎮 {meta['title']}")
    print(f"ID: {meta['id']}")
    print(f"Subject: {meta['subject']}")
    print(f"Grade: {meta['grade']}")
    print(f"Prompt: {meta['ai_prompt']}")
    
    # Learning Objectives
    print_subheader("📚 Learning Objectives")
    for i, objective in enumerate(meta['learning_objectives'], 1):
        print(f"  {i}. {objective}")
    
    # World Settings
    print_subheader("🌍 World Settings")
    world = scene["world_settings"]
    print(f"  Gravity: {world['gravity']} units/s²")
    print(f"  Theme: {world['theme']}")
    bg_color = world['background_color']
    print(f"  Background Color: RGBA({bg_color[0]}, {bg_color[1]}, {bg_color[2]}, {bg_color[3]})")
    
    # Entities Overview
    print_subheader("🎯 Entities")
    entities = scene["entities"]
    
    # Count entity types
    entity_types = {}
    for entity in entities:
        name = entity['name']
        tags = entity.get('tags', [])
        primary_tag = tags[0] if tags else 'unknown'
        entity_types[primary_tag] = entity_types.get(primary_tag, 0) + 1
        
        print(f"\n  • {name}")
        print(f"    Tags: {', '.join(tags)}")
        if 'components' in entity:
            transform = entity['components'].get('transform', {})
            if transform:
                print(f"    Position: ({transform.get('x', 0)}, {transform.get('y', 0)})")
            
            # Show special info
            if 'text_label' in entity['components']:
                text = entity['components']['text_label'].get('text', '')
                print(f"    Label: {text}")
            
            if 'info_zone' in entity['components']:
                info = entity['components']['info_zone']
                print(f"    💡 Fact: {info.get('fact', '')[:60]}...")
            
            if 'player_tag' in entity['components']:
                player = entity['components']['player_tag']
                print(f"    Speed: {player.get('move_speed', 0)} units/s")
                print(f"    Jump Force: {player.get('jump_force', 0)} units/s")
    
    # Entity Summary
    print_subheader("📊 Entity Summary")
    for entity_type, count in sorted(entity_types.items()):
        print(f"  {entity_type.capitalize()}s: {count}")
    
    # Game Rules
    print_subheader("📋 Game Rules")
    rules = scene["game_rules"]
    print(f"  Objective: {rules['objective']}")
    print(f"  Collectibles Required: {rules['collectibles_required']}")
    print(f"  Difficulty: {rules['difficulty']}")
    print(f"  Bonus: {rules['bonus_info']}")
    
    # Collectibles Detail
    print_subheader("✨ Collectibles (Learning Tokens)")
    collectibles = [e for e in entities if 'collectible' in e.get('tags', [])]
    for collectible in collectibles:
        name = collectible['name']
        label = collectible['components'].get('text_label', {}).get('text', 'N/A')
        print(f"  • {name}: {label}")
    
    # Info Zones Detail
    print_subheader("📖 Info Zones (Learn Facts)")
    info_zones = [e for e in entities if 'info' in e.get('tags', [])]
    for i, info in enumerate(info_zones, 1):
        info_data = info['components'].get('info_zone', {})
        print(f"\n  {i}. {info_data.get('title', 'Unknown')}")
        print(f"     → {info_data.get('fact', 'No fact provided')}")
    
    # Summary
    print_subheader("✅ Scene Ready")
    print(f"  Total Entities: {len(entities)}")
    print(f"  Scene file saved to: {scene_path}")
    print(f"  Ready to be loaded into EduEngine!")
    
    print("\n" + "="*70 + "\n")

if __name__ == "__main__":
    scene_file = Path(__file__).parent / "data" / "scenes" / "newtons_laws_grade7.json"
    
    if scene_file.exists():
        print("\n🚀 NEWTON'S 3 LAWS OF MOTION - SCENE DEMO")
        demo_scene(scene_file)
    else:
        print(f"❌ Scene file not found: {scene_file}")
        sys.exit(1)
