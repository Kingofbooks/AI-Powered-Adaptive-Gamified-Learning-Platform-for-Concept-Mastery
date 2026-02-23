#!/usr/bin/env python3
"""
Scene Preparation Pipeline
Analyzes scene, generates missing assets, and prepares game for launch
Runs automatically after scene generation from AI-Agents
"""

import json
import os
import sys
import subprocess
from pathlib import Path

class ScenePrepper:
    def __init__(self, scene_path):
        self.scene_path = Path(scene_path)
        self.game_engine_path = Path(__file__).parent
        self.assets_path = self.game_engine_path / "assets" / "images"
        
    def step_1_analyze_assets(self):
        """Step 1: Analyze what assets the scene needs"""
        print("\n" + "="*70)
        print("STEP 1️⃣  ANALYZING SCENE ASSETS")
        print("="*70)
        
        try:
            result = subprocess.run(
                ["python3", "analyze_scene_assets.py", str(self.scene_path)],
                cwd=self.game_engine_path,
                capture_output=True,
                text=True
            )
            
            print(result.stdout)
            
            # Return True if all assets exist, False if missing
            return result.returncode == 0
        
        except Exception as e:
            print(f"❌ Analysis failed: {e}")
            return False
    
    def step_2_extract_missing_assets(self):
        """Step 2: Extract list of missing assets"""
        print("\n" + "="*70)
        print("STEP 2️⃣  EXTRACTING MISSING ASSETS")
        print("="*70)
        
        with open(self.scene_path, 'r') as f:
            scene = json.load(f)
        
        missing = []
        for entity in scene.get('entities', []):
            if 'components' in entity and 'sprite' in entity['components']:
                sprite = entity['components']['sprite']
                if 'texture' in sprite:
                    texture_path = sprite['texture']
                    asset_path = self.assets_path / texture_path.replace("assets/images/", "")
                    
                    if not asset_path.exists():
                        missing.append({
                            'texture': texture_path,
                            'entity': entity.get('name')
                        })
        
        if missing:
            print(f"\n📝 Missing Assets ({len(missing)}):")
            for asset in missing:
                print(f"   • {asset['texture']}")
                print(f"     └─ Used by: {asset['entity']}")
            return missing
        else:
            print("✅ All assets exist!")
            return []
    
    def step_3_generate_missing_assets(self, missing_assets):
        """Step 3: Generate missing assets"""
        if not missing_assets:
            print("⏭️  Skipping generation (all assets present)")
            return True
        
        print("\n" + "="*70)
        print("STEP 3️⃣  GENERATING MISSING ASSETS")
        print("="*70)
        
        try:
            result = subprocess.run(
                ["python3", "generate_assets.py", "--scene", str(self.scene_path)],
                cwd=self.game_engine_path,
                capture_output=True,
                text=True
            )
            
            print(result.stdout)
            return result.returncode == 0
        
        except Exception as e:
            print(f"❌ Generation failed: {e}")
            return False
    
    def step_4_validate_scene_json(self):
        """Step 4: Validate scene JSON structure"""
        print("\n" + "="*70)
        print("STEP 4️⃣  VALIDATING SCENE JSON")
        print("="*70)
        
        try:
            with open(self.scene_path, 'r') as f:
                scene = json.load(f)
            
            # Check required fields
            required = ['scene_meta', 'world_settings', 'entities']
            missing_fields = [f for f in required if f not in scene]
            
            if missing_fields:
                print(f"❌ Missing required fields: {missing_fields}")
                return False
            
            # Validate metadata
            meta = scene['scene_meta']
            meta_fields = ['id', 'title', 'subject', 'grade']
            missing_meta = [f for f in meta_fields if f not in meta]
            
            if missing_meta:
                print(f"⚠️  Incomplete metadata: {missing_meta}")
            
            # Validate entities
            entity_count = len(scene['entities'])
            print(f"✅ Scene JSON valid")
            print(f"   Title: {meta.get('title', 'Unknown')}")
            print(f"   Entities: {entity_count}")
            print(f"   Grade: {meta.get('grade', 'Unknown')}")
            
            return True
        
        except json.JSONDecodeError as e:
            print(f"❌ JSON parsing error: {e}")
            return False
        except Exception as e:
            print(f"❌ Validation error: {e}")
            return False
    
    def step_5_report_ready_to_play(self):
        """Step 5: Final report and ready to play"""
        print("\n" + "="*70)
        print("STEP 5️⃣  READY TO PLAY!")
        print("="*70)
        
        with open(self.scene_path, 'r') as f:
            scene = json.load(f)
        
        meta = scene['scene_meta']
        
        print(f"\n✅ SCENE PREPARATION COMPLETE!")
        print(f"\n📋 Scene Details:")
        print(f"   ID: {meta['id']}")
        print(f"   Title: {meta['title']}")
        print(f"   Subject: {meta.get('subject', 'General')}")
        print(f"   Grade: {meta.get('grade', 'N/A')}")
        print(f"   Entities: {len(scene['entities'])}")
        
        print(f"\n🎮 NEXT STEPS TO RUN THE GAME:")
        print(f"   1. cd {self.game_engine_path}")
        print(f"   2. ./EduEngine  # Launch the game")
        print(f"   3. Load scene: {self.scene_path.name}")
        print(f"\n   Or use: python3 game_launcher.py {self.scene_path.name}")
        
        return True
    
    def run_full_pipeline(self):
        """Run the complete scene preparation pipeline"""
        print("\n" + "🎮"*35)
        print("EDUENGINE SCENE PREPARATION PIPELINE")
        print("🎮"*35)
        
        # Step 1: Analyze
        all_assets_exist = self.step_1_analyze_assets()
        
        # Step 2: Extract missing
        missing_assets = self.step_2_extract_missing_assets()
        
        # Step 3: Generate if needed
        if missing_assets:
            generation_success = self.step_3_generate_missing_assets(missing_assets)
            if not generation_success:
                print("\n⚠️  Asset generation had issues, but continuing...")
        
        # Step 4: Validate
        json_valid = self.step_4_validate_scene_json()
        
        if not json_valid:
            print("\n❌ Scene validation failed!")
            return False
        
        # Step 5: Success
        self.step_5_report_ready_to_play()
        
        print("\n" + "✅"*35)
        print("PIPELINE COMPLETE - READY TO PLAY!")
        print("✅"*35 + "\n")
        
        return True

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 prepare_scene.py <scene_json_path>")
        print("Example: python3 prepare_scene.py data/scenes/newtons_laws_grade7.json")
        sys.exit(1)
    
    scene_path = sys.argv[1]
    
    if not os.path.exists(scene_path):
        print(f"❌ Scene file not found: {scene_path}")
        sys.exit(1)
    
    prepper = ScenePrepper(scene_path)
    success = prepper.run_full_pipeline()
    
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
