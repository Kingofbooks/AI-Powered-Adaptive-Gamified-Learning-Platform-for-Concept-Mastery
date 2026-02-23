#!/usr/bin/env python3
"""
Smart Asset Manager
Handles asset extraction, validation, and auto-generation for any scene
Integrates with AI-generated scene JSONs
"""

import json
import subprocess
import sys
from pathlib import Path
from collections import defaultdict

class SmartAssetManager:
    def __init__(self):
        self.engine_path = Path(__file__).parent
        self.assets_path = self.engine_path / "assets" / "images"
        self.generate_script = self.engine_path / "generate_assets.py"
        
        # Map possible theme names to canonical names
        self.theme_aliases = {
            'photosynthesis': 'photosynthesis',
            'photo': 'photosynthesis',
            'plants': 'photosynthesis',
            'biology': 'biology',
            'bio': 'biology',
            'space': 'space',
            'solar_system': 'space',
            'solarsystem': 'space',
            'astronomy': 'space',
            'water_cycle': 'water_cycle',
            'watercycle': 'water_cycle',
            'water': 'water_cycle',
            'hydro': 'water_cycle',
            'mathematics': 'mathematics',
            'math': 'mathematics',
            'algebra': 'mathematics',
            'geometry': 'mathematics',
            'history': 'history',
            'ancient': 'history',
            'chemistry': 'chemistry',
            'chem': 'chemistry',
            'reactions': 'chemistry',
            'physics': 'physics',
            'mechanics': 'physics',
            'motion': 'physics',
            'geography': 'geography',
            'geo': 'geography',
            'earth': 'geography',
            'animals': 'animals',
            'zoology': 'animals',
            'ecology': 'animals',
            'human_body': 'human_body',
            'anatomy': 'human_body',
            'body': 'human_body',
        }
    
    def normalize_theme(self, theme_name):
        """Convert any theme name to canonical name"""
        normalized = theme_name.lower().replace(" ", "_").replace("-", "_")
        return self.theme_aliases.get(normalized, normalized)
    
    def extract_assets_from_scene(self, scene_path):
        """Extract all asset requirements from scene JSON"""
        with open(scene_path, 'r') as f:
            scene = json.load(f)
        
        # Get theme from scene metadata
        theme = scene.get('scene_meta', {}).get('subject', 'default')
        theme = self.normalize_theme(theme)
        
        # Extract all textures
        textures = {}
        for entity in scene.get('entities', []):
            if 'components' in entity and 'sprite' in entity['components']:
                sprite = entity['components']['sprite']
                if 'texture' in sprite:
                    texture_path = sprite['texture']
                    textures[texture_path] = entity.get('name')
        
        return {
            'theme': theme,
            'scene_id': scene.get('scene_meta', {}).get('id'),
            'textures': textures,
            'scene_data': scene
        }
    
    def check_texture_exists(self, texture_path):
        """Check if a texture file exists"""
        # Convert "assets/images/space/background.png" to filesystem path
        relative_path = texture_path.replace("assets/images/", "")
        full_path = self.assets_path / relative_path
        return full_path.exists()
    
    def get_missing_textures(self, texture_dict):
        """Get list of missing texture files"""
        missing = {}
        for texture_path, entity_name in texture_dict.items():
            if not self.check_texture_exists(texture_path):
                missing[texture_path] = entity_name
        return missing
    
    def generate_missing_assets(self, theme, missing_count=0):
        """Generate assets for a theme using the existing generator"""
        print(f"\n📝 Generating assets for theme: {theme}")
        print(f"   This will create {10 + missing_count} sprite images...")
        
        try:
            result = subprocess.run(
                ["python3", "generate_assets.py", "--theme", theme],
                cwd=self.engine_path,
                capture_output=True,
                text=True,
                timeout=30
            )
            
            if result.returncode == 0:
                print(f"✅ Assets generated successfully for '{theme}'")
                # Parse output to show what was created
                for line in result.stdout.split('\n'):
                    if 'PNG' in line or 'saved' in line.upper():
                        print(f"   {line}")
                return True
            else:
                print(f"❌ Generation failed: {result.stderr}")
                return False
        
        except subprocess.TimeoutExpired:
            print(f"⏱️  Generation timed out")
            return False
        except Exception as e:
            print(f"❌ Error: {e}")
            return False
    
    def normalize_texture_paths(self, scene_path, theme):
        """Fix scene JSON to use correct asset names that actually exist"""
        with open(scene_path, 'r') as f:
            scene = json.load(f)
        
        modified = False
        collectible_idx = 0
        
        for entity in scene.get('entities', []):
            if 'components' not in entity or 'sprite' not in entity['components']:
                continue
            
            sprite = entity['components']['sprite']
            if 'texture' not in sprite:
                continue
            
            texture = sprite['texture']
            entity_tags = entity.get('tags', [])
            
            # Fix collectible paths
            if 'collectible' in entity_tags and 'collectible' in texture:
                if texture.endswith('collectible.png'):
                    new_texture = f"assets/images/{theme}/collectible_{collectible_idx}.png"
                    sprite['texture'] = new_texture
                    collectible_idx += 1
                    modified = True
            
            # Fix info_sign paths
            if 'info' in entity_tags and 'info' in texture:
                if texture.endswith('info_sign.png') or texture.endswith('info.png'):
                    new_texture = f"assets/images/{theme}/goal.png"  # Use goal as placeholder
                    sprite['texture'] = new_texture
                    modified = True
        
        if modified:
            with open(scene_path, 'w') as f:
                json.dump(scene, f, indent=2)
            return True
        
        return False
    
    def validate_and_prepare(self, scene_path):

        """Full validation and preparation pipeline"""
        print("\n" + "="*70)
        print("ASSET MANAGEMENT PIPELINE")
        print("="*70)
        
        # Step 0: Normalize texture paths
        print("\n[0/4] Normalizing asset references...")
        info = self.extract_assets_from_scene(scene_path)
        theme = info['theme']
        normalized = self.normalize_texture_paths(scene_path, theme)
        if normalized:
            print(f"  ✅ Scene JSON normalized for theme '{theme}'")
            # Re-extract after normalization
            info = self.extract_assets_from_scene(scene_path)
        
        # Step 1: Extract
        print("\n[1/4] Extracting asset requirements...")
        textures = info['textures']
        
        print(f"✅ Scene: {info['scene_id']}")
        print(f"✅ Theme: {theme}")
        print(f"✅ Textures required: {len(textures)}")
        
        # Step 2: Check
        print("\n[2/4] Checking asset availability...")
        missing = self.get_missing_textures(textures)
        
        if missing:
            print(f"⚠️  Missing {len(missing)} textures:")
            for texture, entity in list(missing.items())[:5]:
                print(f"   • {texture}")
                print(f"     └─ Used by: {entity}")
            if len(missing) > 5:
                print(f"   ... and {len(missing)-5} more")
        else:
            print(f"✅ All textures available!")
            return True
        
        # Step 3: Generate
        print("\n[3/4] Auto-generating missing assets...")
        success = self.generate_missing_assets(theme, len(missing))
        
        if not success:
            print("⚠️  Asset generation failed, trying alternative...")
            # Try generating all themes as fallback
            print("📝 Generating all default themes as fallback...")
            result = subprocess.run(
                ["python3", "generate_assets.py", "--all"],
                cwd=self.engine_path,
                capture_output=True,
                text=True,
                timeout=60
            )
            if result.returncode != 0:
                print("❌ Fallback also failed")
                return False
            print("✅ Fallback generation complete")
        
        # Step 4: Verify
        print("\n[4/4] Verifying assets...")
        missing_after = self.get_missing_textures(textures)
        
        if missing_after:
            print(f"⚠️  Still missing {len(missing_after)} textures")
            for texture in list(missing_after.keys())[:3]:
                print(f"   • {texture}")
            return False
        else:
            print(f"✅ All assets ready to use!")
            print(f"   Theme: {theme}")
            print(f"   Total textures: {len(textures)}")
            print(f"\n✅ Scene is ready for EduEngine!")
            return True

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 smart_assets.py <scene_json_path>")
        print("\nExample:")
        print("  python3 smart_assets.py data/scenes/newtons_laws_grade7.json")
        sys.exit(1)
    
    scene_path = sys.argv[1]
    path = Path(scene_path)
    
    if not path.exists():
        print(f"❌ Scene file not found: {scene_path}")
        sys.exit(1)
    
    manager = SmartAssetManager()
    success = manager.validate_and_prepare(scene_path)
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
