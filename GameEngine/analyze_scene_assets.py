#!/usr/bin/env python3
"""
Scene Asset Analyzer & Validator
Extracts all assets needed for a scene from the JSON and checks availability
"""

import json
import os
import sys
from pathlib import Path
from collections import defaultdict

def extract_texture_paths(entity):
    """Recursively extract all texture paths from entity components"""
    textures = []
    
    if 'components' in entity:
        components = entity['components']
        
        # Check sprite component
        if 'sprite' in components and 'texture' in components['sprite']:
            textures.append(components['sprite']['texture'])
    
    return textures

def analyze_scene(scene_path):
    """Analyze scene and extract all asset requirements"""
    with open(scene_path, 'r') as f:
        scene = json.load(f)
    
    assets_needed = {
        'textures': set(),
        'themes': set(),
        'entities_by_texture': defaultdict(list)
    }
    
    # Extract scene info
    scene_id = scene.get('scene_meta', {}).get('id', 'unknown')
    subject = scene.get('scene_meta', {}).get('subject', 'unknown')
    print(f"\n📊 Scene Analysis: {scene_id} ({subject})")
    print("="*70)
    
    # Extract all textures
    for entity in scene.get('entities', []):
        entity_name = entity.get('name', 'Unknown')
        entity_tags = entity.get('tags', [])
        
        textures = extract_texture_paths(entity)
        for texture in textures:
            assets_needed['textures'].add(texture)
            assets_needed['entities_by_texture'][texture].append({
                'name': entity_name,
                'tags': entity_tags
            })
            
            # Extract theme from path (e.g., "assets/images/space/background.png" -> "space")
            if texture.startswith("assets/images/"):
                theme = texture.split('/')[2]
                assets_needed['themes'].add(theme)
    
    return scene, assets_needed

def check_asset_availability(assets_needed):
    """Check which assets exist and which are missing"""
    base_path = Path(__file__).parent / "assets" / "images"
    
    available = []
    missing = []
    
    for texture in sorted(assets_needed['textures']):
        # Convert "assets/images/space/background.png" to full path
        asset_path = base_path / texture.replace("assets/images/", "")
        
        if asset_path.exists():
            available.append((texture, asset_path))
        else:
            missing.append((texture, asset_path))
    
    return available, missing

def generate_asset_report(scene_path):
    """Generate comprehensive asset report"""
    scene, assets_needed = analyze_scene(scene_path)
    available, missing = check_asset_availability(assets_needed)
    
    print(f"\n📦 Themes Required: {', '.join(sorted(assets_needed['themes']))}")
    print(f"🖼️  Total Textures: {len(assets_needed['textures'])}")
    
    # Available Assets
    print(f"\n✅ AVAILABLE ASSETS ({len(available)}):")
    for texture, path in available:
        file_size = os.path.getsize(path) / 1024 if path.exists() else 0
        print(f"   ✓ {texture} ({file_size:.1f} KB)")
    
    # Missing Assets
    if missing:
        print(f"\n❌ MISSING ASSETS ({len(missing)}):")
        for texture, path in missing:
            print(f"   ✗ {texture}")
            print(f"     Expected at: {path}")
            
            # Show which entities need this asset
            entities = assets_needed['entities_by_texture'][texture]
            for entity in entities[:2]:  # Show first 2
                print(f"       - Used by: {entity['name']} ({', '.join(entity['tags'])})")
            if len(entities) > 2:
                print(f"       - And {len(entities)-2} more entities")
    else:
        print(f"\n✅ ALL ASSETS AVAILABLE!")
    
    print("\n" + "="*70)
    
    # Summary
    print(f"\n📈 SUMMARY:")
    print(f"   Available: {len(available)}/{len(assets_needed['textures'])} ({100*len(available)/len(assets_needed['textures']):.1f}%)")
    print(f"   Missing: {len(missing)}/{len(assets_needed['textures'])}")
    
    if missing:
        print(f"\n💡 ACTION NEEDED:")
        print(f"   Run: python3 generate_assets.py --scene {Path(scene_path).name}")
        print(f"   This will automatically generate {len(missing)} missing assets")
    
    return {
        'scene_id': scene.get('scene_meta', {}).get('id'),
        'available': len(available),
        'missing': len(missing),
        'total': len(assets_needed['textures']),
        'missing_textures': [t[0] for t in missing]
    }

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 analyze_scene_assets.py <scene_json_path>")
        print("Example: python3 analyze_scene_assets.py data/scenes/newtons_laws_grade7.json")
        sys.exit(1)
    
    scene_path = sys.argv[1]
    
    if not os.path.exists(scene_path):
        print(f"❌ Scene file not found: {scene_path}")
        sys.exit(1)
    
    report = generate_asset_report(scene_path)
    
    # Exit with error code if assets are missing
    if report['missing'] > 0:
        sys.exit(1)  # Return error so calling script knows to generate assets
    else:
        sys.exit(0)  # All good
