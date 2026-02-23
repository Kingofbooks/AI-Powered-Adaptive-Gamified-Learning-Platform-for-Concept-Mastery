#!/usr/bin/env python3
"""
🧪 EduEngine AI-to-Game Pipeline - TEST SUITE
==============================================

Comprehensive tests to verify:
1. AI scene generation works
2. Generated JSON is valid
3. Scenes can be loaded by the game engine
4. Game engine executes successfully

Run:
    python test_pipeline.py              # Run all tests
    python test_pipeline.py --verbose    # Show detailed output
    python test_pipeline.py --theme space --grade 3  # Test specific theme/grade
"""

import sys
import json
import subprocess
from pathlib import Path
from typing import Dict, List, Tuple
import argparse

SCRIPT_DIR = Path(__file__).parent
GAME_ENGINE_DIR = SCRIPT_DIR.parent / "GameEngine"
DATA_SCENES_DIR = GAME_ENGINE_DIR / "data" / "scenes"

sys.path.insert(0, str(SCRIPT_DIR))
from teacher_ai_agent import TeacherAIAgent, THEME_CONFIG

class TestResult:
    def __init__(self, name: str):
        self.name = name
        self.passed = False
        self.errors = []
        self.warnings = []
        self.duration = 0
    
    def add_error(self, error: str):
        self.errors.append(error)
    
    def add_warning(self, warning: str):
        self.warnings.append(warning)
    
    def mark_passed(self):
        self.passed = True
    
    def __str__(self):
        status = "✅ PASS" if self.passed else "❌ FAIL"
        result = f"{status} - {self.name}\n"
        
        if self.errors:
            result += "  Errors:\n"
            for error in self.errors:
                result += f"    • {error}\n"
        
        if self.warnings:
            result += "  Warnings:\n"
            for warning in self.warnings:
                result += f"    ⚠️  {warning}\n"
        
        return result

class TestSuite:
    def __init__(self, verbose=False):
        self.verbose = verbose
        self.results: List[TestResult] = []
        self.agent = TeacherAIAgent()
    
    def log(self, msg: str):
        if self.verbose:
            print(msg)
    
    def test_ai_generation(self, prompt: str) -> Tuple[bool, dict]:
        """Test 1: AI can generate scene from prompt"""
        result = TestResult(f"Generate Scene: '{prompt}'")
        
        try:
            self.log(f"\n  Generating scene from: {prompt}")
            scene = self.agent.generate_scene(prompt)
            
            # Check basic structure
            if not isinstance(scene, dict):
                result.add_error("Scene is not a dictionary")
                self.results.append(result)
                return False, {}
            
            if "scene_meta" not in scene:
                result.add_error("Missing 'scene_meta'")
            if "world_settings" not in scene:
                result.add_error("Missing 'world_settings'")
            if "entities" not in scene:
                result.add_error("Missing 'entities'")
            
            if not result.errors:
                result.mark_passed()
                self.log(f"  ✅ Scene generated successfully")
            
            self.results.append(result)
            return not bool(result.errors), scene
        
        except Exception as e:
            result.add_error(f"Exception: {str(e)}")
            self.results.append(result)
            return False, {}
    
    def test_scene_structure(self, scene: dict) -> bool:
        """Test 2: Scene has valid EduEngine structure"""
        result = TestResult("Validate Scene Structure")
        
        try:
            entities = scene.get("entities", [])
            
            # Count entity types
            backgrounds = sum(1 for e in entities if e.get("tags") == ["background"])
            players = sum(1 for e in entities if e.get("tags") == ["player"])
            grounds = sum(1 for e in entities if "platform" in e.get("tags", []) and e.get("name") == "Ground")
            goals = sum(1 for e in entities if e.get("tags") == ["goal"])
            platforms = sum(1 for e in entities if "platform" in e.get("tags", []))
            collectibles = sum(1 for e in entities if e.get("tags") == ["collectible"])
            enemies = sum(1 for e in entities if e.get("tags") == ["enemy"])
            
            # Validate counts
            self.log(f"  Entity counts: BG={backgrounds}, Player={players}, Ground={grounds}, Goal={goals}")
            self.log(f"  Platforms={platforms}, Collectibles={collectibles}, Enemies={enemies}")
            
            if backgrounds != 1:
                result.add_error(f"Expected 1 Background, found {backgrounds}")
            if players != 1:
                result.add_error(f"Expected 1 Player, found {players}")
            if grounds != 1:
                result.add_error(f"Expected 1 Ground, found {grounds}")
            if goals != 1:
                result.add_error(f"Expected 1 Goal, found {goals}")
            
            if platforms < 3:
                result.add_warning(f"Expected at least 3 platforms, found {platforms}")
            if collectibles < 3:
                result.add_warning(f"Expected at least 3 collectibles, found {collectibles}")
            
            # Check entity bounds
            for entity in entities:
                if "components" in entity and "transform" in entity["components"]:
                    t = entity["components"]["transform"]
                    x, y = t.get("x", 0), t.get("y", 0)
                    
                    if x < 0 or x > 1280:
                        result.add_warning(f"Entity {entity.get('name')} x={x} out of bounds")
                    if y < 0 or y > 720:
                        result.add_warning(f"Entity {entity.get('name')} y={y} out of bounds")
            
            if not result.errors:
                result.mark_passed()
                self.log(f"  ✅ Scene structure valid")
            
            self.results.append(result)
            return not bool(result.errors)
        
        except Exception as e:
            result.add_error(f"Exception: {str(e)}")
            self.results.append(result)
            return False
    
    def test_json_serialization(self, scene: dict) -> bool:
        """Test 3: Scene can be serialized to JSON"""
        result = TestResult("JSON Serialization")
        
        try:
            json_str = json.dumps(scene, indent=2)
            
            if not json_str:
                result.add_error("Failed to serialize to JSON")
            else:
                self.log(f"  JSON size: {len(json_str)} bytes")
                result.mark_passed()
                self.log(f"  ✅ Scene serializes to valid JSON")
            
            self.results.append(result)
            return not bool(result.errors)
        
        except Exception as e:
            result.add_error(f"Exception: {str(e)}")
            self.results.append(result)
            return False
    
    def test_file_save(self, scene: dict) -> Tuple[bool, str]:
        """Test 4: Scene can be saved to file"""
        result = TestResult("Save Scene to File")
        
        try:
            DATA_SCENES_DIR.mkdir(parents=True, exist_ok=True)
            
            scene_id = scene.get("scene_meta", {}).get("id", "test_scene")
            filepath = DATA_SCENES_DIR / f"{scene_id}.json"
            
            with open(filepath, 'w') as f:
                json.dump(scene, f, indent=2)
            
            if not filepath.exists():
                result.add_error(f"File not created: {filepath}")
            else:
                file_size = filepath.stat().st_size
                self.log(f"  Saved to: {filepath}")
                self.log(f"  File size: {file_size} bytes")
                result.mark_passed()
                self.log(f"  ✅ Scene saved successfully")
            
            self.results.append(result)
            return not bool(result.errors), str(filepath)
        
        except Exception as e:
            result.add_error(f"Exception: {str(e)}")
            self.results.append(result)
            return False, ""
    
    def test_file_load(self, filepath: str) -> Tuple[bool, dict]:
        """Test 5: Saved scene can be loaded back"""
        result = TestResult("Load Scene from File")
        
        try:
            with open(filepath, 'r') as f:
                loaded_scene = json.load(f)
            
            if not isinstance(loaded_scene, dict):
                result.add_error("Loaded object is not a dictionary")
            elif "scene_meta" not in loaded_scene:
                result.add_error("Loaded scene missing 'scene_meta'")
            else:
                result.mark_passed()
                self.log(f"  ✅ Scene loaded successfully")
            
            self.results.append(result)
            return not bool(result.errors), loaded_scene
        
        except Exception as e:
            result.add_error(f"Exception: {str(e)}")
            self.results.append(result)
            return False, {}
    
    def test_engine_binary(self) -> bool:
        """Test 6: Game engine binary exists"""
        result = TestResult("Game Engine Binary")
        
        engine_binary = GAME_ENGINE_DIR / "EduEngine"
        
        if not engine_binary.exists():
            result.add_error(f"Engine binary not found: {engine_binary}")
        elif not os.access(engine_binary, os.X_OK):
            result.add_error(f"Engine binary not executable: {engine_binary}")
        else:
            result.mark_passed()
            self.log(f"  ✅ Engine binary found and executable")
        
        self.results.append(result)
        return not bool(result.errors)
    
    def test_theme_detection(self, prompts: List[str]) -> bool:
        """Test 7: Theme detection from prompts"""
        result = TestResult("Theme Detection")
        
        try:
            for prompt in prompts:
                theme = self.agent.detect_theme(prompt)
                grade = self.agent.detect_grade(prompt)
                
                self.log(f"  '{prompt}'")
                self.log(f"    → Theme: {theme}, Grade: {grade}")
                
                if theme not in THEME_CONFIG:
                    result.add_error(f"Detected theme '{theme}' not in THEME_CONFIG")
                
                try:
                    grade_int = int(grade)
                    if grade_int < 1 or grade_int > 12:
                        result.add_warning(f"Grade {grade} outside valid range [1-12]")
                except ValueError:
                    result.add_error(f"Failed to parse grade: {grade}")
            
            if not result.errors:
                result.mark_passed()
                self.log(f"  ✅ All themes detected correctly")
            
            self.results.append(result)
            return not bool(result.errors)
        
        except Exception as e:
            result.add_error(f"Exception: {str(e)}")
            self.results.append(result)
            return False
    
    def run_full_pipeline(self, prompt: str) -> bool:
        """Test 8: Full pipeline - generate and save"""
        result = TestResult(f"Full Pipeline: {prompt}")
        
        try:
            self.log(f"\n  Running full pipeline for: {prompt}")
            
            # Generate
            self.log(f"  1. Generating scene...")
            scene = self.agent.generate_scene(prompt)
            
            # Validate
            self.log(f"  2. Validating structure...")
            entities = scene.get("entities", [])
            if len(entities) < 5:
                result.add_warning(f"Scene has only {len(entities)} entities")
            
            # Save
            self.log(f"  3. Saving to file...")
            scene_id = scene.get("scene_meta", {}).get("id", "test")
            filepath = DATA_SCENES_DIR / f"{scene_id}.json"
            with open(filepath, 'w') as f:
                json.dump(scene, f, indent=2)
            
            if filepath.exists():
                result.mark_passed()
                self.log(f"  ✅ Full pipeline successful")
            else:
                result.add_error(f"Failed to save scene")
            
            self.results.append(result)
            return not bool(result.errors)
        
        except Exception as e:
            result.add_error(f"Exception: {str(e)}")
            self.results.append(result)
            return False
    
    def print_summary(self):
        """Print test summary"""
        print(f"\n{'='*80}")
        print(f"TEST SUMMARY")
        print(f"{'='*80}\n")
        
        total = len(self.results)
        passed = sum(1 for r in self.results if r.passed)
        failed = total - passed
        
        for result in self.results:
            print(result)
        
        print(f"\n{'='*80}")
        print(f"RESULTS: {passed}/{total} PASSED")
        print(f"{'='*80}\n")
        
        if failed == 0:
            print("✅ All tests passed!")
            return True
        else:
            print(f"❌ {failed} test(s) failed")
            return False

def main():
    parser = argparse.ArgumentParser(description="Test the AI-to-Game pipeline")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    parser.add_argument("--theme", type=str, default=None, help="Test specific theme")
    parser.add_argument("--grade", type=str, default=None, help="Test specific grade")
    parser.add_argument("--prompt", type=str, default=None, help="Test specific prompt")
    
    args = parser.parse_args()
    
    suite = TestSuite(verbose=args.verbose)
    
    # Test basic operations
    print(f"\n{'='*80}")
    print(f"EduEngine AI-to-Game Pipeline - Test Suite")
    print(f"{'='*80}")
    
    # Test 1: Theme detection
    test_prompts = [
        "teach photosynthesis for grade 5",
        "space adventure for year 3",
        "biology cells for grade 6",
        "water cycle",
    ]
    if args.prompt:
        test_prompts = [args.prompt]
    
    suite.test_theme_detection(test_prompts)
    
    # Test 2-6: Full pipeline
    main_prompt = args.prompt or "teach photosynthesis for grade 5"
    if args.theme:
        main_prompt = f"{args.theme} for grade {args.grade or 5}"
    
    print(f"\n⏳ Running full pipeline tests...")
    scene_valid, scene = suite.test_ai_generation(main_prompt)
    
    if scene_valid:
        suite.test_scene_structure(scene)
        suite.test_json_serialization(scene)
        file_ok, filepath = suite.test_file_save(scene)
        
        if file_ok:
            suite.test_file_load(filepath)
    
    suite.test_engine_binary()
    
    # Print summary
    all_passed = suite.print_summary()
    
    return 0 if all_passed else 1

if __name__ == "__main__":
    import os
    exit_code = main()
    sys.exit(exit_code)
