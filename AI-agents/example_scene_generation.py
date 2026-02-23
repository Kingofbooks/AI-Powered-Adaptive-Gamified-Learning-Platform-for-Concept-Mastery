#!/usr/bin/env python3
"""
Example: Generate EduEngine Scene JSON using the AI Agent

This script demonstrates how to use the updated TeacherAIAgent to generate
complete EduEngine scene JSON files matching the specification.
"""

from teacher_ai_agent import TeacherAIAgent
import json

def main():
    # Initialize the AI agent
    agent = TeacherAIAgent()
    
    # Example 1: Generate a scene from a simple prompt
    print("=" * 80)
    print("EXAMPLE 1: Generate Photosynthesis Scene for Grade 5")
    print("=" * 80)
    
    prompt = "teach photosynthesis for grade 5 students"
    print(f"\n📝 Prompt: {prompt}\n")
    
    scene = agent.generate_scene(prompt)
    
    # Display scene metadata
    print(f"✅ Scene Generated Successfully!\n")
    meta = scene["scene_meta"]
    print(f"  Scene ID:     {meta['id']}")
    print(f"  Title:        {meta['title']}")
    print(f"  Subject:      {meta['subject']}")
    print(f"  Grade:        {meta['grade']}")
    print(f"  Total Entities: {len(scene['entities'])}")
    
    # Count entity types
    backgrounds = [e for e in scene["entities"] if e.get("tags") == ["background"]]
    players = [e for e in scene["entities"] if e.get("tags") == ["player"]]
    platforms = [e for e in scene["entities"] if "platform" in e.get("tags", [])]
    collectibles = [e for e in scene["entities"] if e.get("tags") == ["collectible"]]
    enemies = [e for e in scene["entities"] if e.get("tags") == ["enemy"]]
    info_signs = [e for e in scene["entities"] if e.get("tags") == ["info"]]
    goals = [e for e in scene["entities"] if e.get("tags") == ["goal"]]
    
    print(f"\n📊 Entity Breakdown:")
    print(f"  Backgrounds:   {len(backgrounds)}")
    print(f"  Players:       {len(players)}")
    print(f"  Platforms:     {len(platforms)}")
    print(f"  Collectibles:  {len(collectibles)}")
    print(f"  Enemies:       {len(enemies)}")
    print(f"  Info Signs:    {len(info_signs)}")
    print(f"  Goals:         {len(goals)}")
    
    # Display world settings
    world = scene["world_settings"]
    print(f"\n🌍 World Settings:")
    print(f"  Gravity:           {world['gravity']}")
    print(f"  Background Color:  {world['background_color']}")
    
    # Display sample collectibles
    if collectibles:
        print(f"\n🎁 Sample Collectibles:")
        for i, collectible in enumerate(collectibles[:3]):
            name = collectible.get("name", "Unknown")
            components = collectible.get("components", {})
            if "collectible" in components:
                label = components["collectible"].get("label", "Unknown")
                print(f"  {i+1}. {name} (Label: {label})")
    
    # Save scene to file
    scene_id = meta["id"]
    filename = f"{scene_id}_output.json"
    with open(filename, 'w') as f:
        json.dump(scene, f, indent=2)
    print(f"\n💾 Scene saved to: {filename}")
    
    # Example 2: Generate different subject
    print("\n" + "=" * 80)
    print("EXAMPLE 2: Generate Space Scene for Grade 7")
    print("=" * 80)
    
    prompt2 = "create a space adventure game for grade 7"
    print(f"\n📝 Prompt: {prompt2}\n")
    
    scene2 = agent.generate_scene(prompt2)
    meta2 = scene2["scene_meta"]
    
    print(f"✅ Scene Generated Successfully!")
    print(f"  Scene ID:     {meta2['id']}")
    print(f"  Title:        {meta2['title']}")
    print(f"  Subject:      {meta2['subject']}")
    print(f"  Grade:        {meta2['grade']}")
    print(f"  Total Entities: {len(scene2['entities'])}")
    
    # Count platform variations
    platforms2 = [e for e in scene2["entities"] if "platform" in e.get("tags", [])]
    print(f"  Platform Count: {len(platforms2)}")
    
    # Example 3: Show theme detection
    print("\n" + "=" * 80)
    print("EXAMPLE 3: Automatic Theme & Grade Detection")
    print("=" * 80)
    
    test_prompts = [
        "biology cells and DNA for year 6",
        "mathematics algebra for grade 8",
        "history of ancient Rome",
        "water cycle lesson"
    ]
    
    for test_prompt in test_prompts:
        detected_theme = agent.detect_theme(test_prompt)
        detected_grade = agent.detect_grade(test_prompt)
        print(f"\n📝 Prompt: '{test_prompt}'")
        print(f"  → Detected Theme: {detected_theme}")
        print(f"  → Detected Grade: {detected_grade}")
    
    # Example 4: Available themes
    print("\n" + "=" * 80)
    print("EXAMPLE 4: Available Themes")
    print("=" * 80)
    
    from teacher_ai_agent import THEME_CONFIG
    
    print("\n🎨 Available Educational Themes:\n")
    for theme_name, config in THEME_CONFIG.items():
        print(f"  • {theme_name.upper()}")
        print(f"    Collectibles: {', '.join(config['collectibles'][:3])}...")
        print(f"    Example Facts: {config['facts'][0]}")
        print()

if __name__ == "__main__":
    main()
