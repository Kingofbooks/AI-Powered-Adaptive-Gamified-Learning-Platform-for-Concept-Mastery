"""
Game Generation Service - Integrates AI-Agents, GameEngine, and Backend
Orchestrates the complete workflow: Prompt → JSON → Game
"""

import json
import os
import subprocess
import shutil
from typing import Dict, Any, Optional
from pathlib import Path
import requests
import logging
from datetime import datetime

logger = logging.getLogger(__name__)

# Configuration
AI_AGENTS_SERVICE_URL = os.getenv("AI_AGENTS_URL", "http://localhost:8001")
GAME_ENGINE_PATH = os.getenv("GAME_ENGINE_PATH", "../GameEngine")
GAME_ENGINE_BINARY = "EduEngine"
SCENES_DATA_DIR = os.path.join(GAME_ENGINE_PATH, "data", "scenes")
GAME_EXPORTS_DIR = os.path.join(GAME_ENGINE_PATH, "exports")

class GameGenerationService:
    """Service to orchestrate AI generation and game engine execution"""
    
    @staticmethod
    def ensure_directories():
        """Ensure all required directories exist"""
        os.makedirs(SCENES_DATA_DIR, exist_ok=True)
        os.makedirs(GAME_EXPORTS_DIR, exist_ok=True)
        logger.info(f"✓ Directories ensured: {SCENES_DATA_DIR}")
    
    @staticmethod
    def call_ai_agents(prompt: str, grade: Optional[str] = None, 
                       subject: Optional[str] = None) -> Dict[str, Any]:
        """
        Call AI-Agents service to generate scene JSON
        
        Args:
            prompt: Natural language prompt (e.g., "photosynthesis for grade 5")
            grade: Optional grade override
            subject: Optional subject override
            
        Returns:
            Generated scene JSON
        """
        try:
            logger.info(f"🤖 Calling AI-Agents: {prompt}")
            
            payload = {
                "prompt": prompt,
                "grade": grade,
                "subject": subject
            }
            
            # Call AI-Agents service
            response = requests.post(
                f"{AI_AGENTS_SERVICE_URL}/generate-scene",
                json=payload,
                timeout=60
            )
            
            if response.status_code != 200:
                logger.error(f"AI-Agents error: {response.text}")
                raise Exception(f"AI-Agents service returned {response.status_code}")
            
            result = response.json()
            if not result.get("success"):
                raise Exception(f"AI-Agents failed: {result.get('message')}")
            
            scene = result["data"]
            logger.info(f"✓ Scene generated: {scene['scene_meta']['id']}")
            
            return scene
            
        except requests.exceptions.ConnectionError:
            logger.error("❌ Cannot reach AI-Agents service")
            raise Exception(f"AI-Agents service not running at {AI_AGENTS_SERVICE_URL}")
        except Exception as e:
            logger.error(f"❌ AI generation failed: {str(e)}")
            raise
    
    @staticmethod
    def save_scene_json(scene: Dict[str, Any]) -> str:
        """
        Save scene JSON to GameEngine data directory
        
        Args:
            scene: Scene data dictionary
            
        Returns:
            Path to saved scene file
        """
        try:
            GameGenerationService.ensure_directories()
            
            scene_id = scene["scene_meta"]["id"]
            scene_file = os.path.join(SCENES_DATA_DIR, f"{scene_id}.json")
            
            with open(scene_file, 'w') as f:
                json.dump(scene, f, indent=2)
            
            logger.info(f"✓ Scene saved: {scene_file}")
            return scene_file
            
        except Exception as e:
            logger.error(f"❌ Failed to save scene: {str(e)}")
            raise
    
    @staticmethod
    def generate_game(prompt: str, grade: Optional[str] = None, 
                      subject: Optional[str] = None) -> Dict[str, Any]:
        """
        Complete workflow: Generate scene and prepare for game engine
        
        Args:
            prompt: User prompt
            grade: Optional grade
            subject: Optional subject
            
        Returns:
            Game info with scene ID and metadata
        """
        try:
            logger.info("=" * 60)
            logger.info("🎮 GAME GENERATION WORKFLOW STARTED")
            logger.info("=" * 60)
            
            # Step 1: Call AI to generate scene
            logger.info("\n📝 STEP 1: Generating scene JSON from AI...")
            scene = GameGenerationService.call_ai_agents(prompt, grade, subject)
            
            # Step 2: Save scene to GameEngine directory
            logger.info("\n💾 STEP 2: Saving scene to GameEngine...")
            scene_file = GameGenerationService.save_scene_json(scene)
            
            # Step 3: Prepare game launch info
            scene_meta = scene["scene_meta"]
            game_info = {
                "success": True,
                "scene_id": scene_meta["id"],
                "title": scene_meta["title"],
                "subject": scene_meta["subject"],
                "grade": scene_meta["grade"],
                "scene_file": scene_file,
                "entities_count": len(scene["entities"]),
                "generated_at": datetime.now().isoformat(),
                "message": f"✓ Game ready: {scene_meta['title']}"
            }
            
            logger.info("\n" + "=" * 60)
            logger.info("✓ GAME GENERATION COMPLETE")
            logger.info("=" * 60)
            logger.info(f"  Scene ID: {game_info['scene_id']}")
            logger.info(f"  Title: {game_info['title']}")
            logger.info(f"  Entities: {game_info['entities_count']}")
            logger.info(f"  File: {game_info['scene_file']}")
            
            return game_info
            
        except Exception as e:
            logger.error(f"\n❌ GAME GENERATION FAILED: {str(e)}")
            return {
                "success": False,
                "error": str(e),
                "message": f"Failed to generate game: {str(e)}"
            }
    
    @staticmethod
    def launch_game(scene_id: str) -> Dict[str, Any]:
        """
        Launch the game engine with generated scene
        
        Args:
            scene_id: ID of scene to load
            
        Returns:
            Launch status
        """
        try:
            # Check if binary exists
            engine_binary = os.path.join(GAME_ENGINE_PATH, GAME_ENGINE_BINARY)
            if not os.path.exists(engine_binary):
                raise FileNotFoundError(f"Game engine binary not found: {engine_binary}")
            
            # Check if scene file exists
            scene_file = os.path.join(SCENES_DATA_DIR, f"{scene_id}.json")
            if not os.path.exists(scene_file):
                raise FileNotFoundError(f"Scene file not found: {scene_file}")
            
            logger.info(f"\n🚀 Launching game engine with scene: {scene_id}")
            logger.info(f"  Binary: {engine_binary}")
            logger.info(f"  Scene: {scene_file}")
            
            # Launch game engine in background
            process = subprocess.Popen(
                [engine_binary, scene_file],
                cwd=GAME_ENGINE_PATH,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE
            )
            
            return {
                "success": True,
                "scene_id": scene_id,
                "pid": process.pid,
                "message": f"Game engine launched with PID {process.pid}"
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to launch game: {str(e)}")
            return {
                "success": False,
                "error": str(e),
                "message": f"Failed to launch game: {str(e)}"
            }
    
    @staticmethod
    def get_game_info(scene_id: str) -> Dict[str, Any]:
        """Get information about a generated game"""
        try:
            scene_file = os.path.join(SCENES_DATA_DIR, f"{scene_id}.json")
            
            if not os.path.exists(scene_file):
                return {
                    "success": False,
                    "error": "Scene not found",
                    "scene_id": scene_id
                }
            
            with open(scene_file, 'r') as f:
                scene = json.load(f)
            
            return {
                "success": True,
                "scene_id": scene_id,
                "metadata": scene["scene_meta"],
                "world_settings": scene["world_settings"],
                "entity_count": len(scene["entities"]),
                "file_path": scene_file,
                "file_size": os.path.getsize(scene_file)
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to get game info: {str(e)}")
            return {
                "success": False,
                "error": str(e)
            }
    
    @staticmethod
    def list_generated_games() -> Dict[str, Any]:
        """List all previously generated games"""
        try:
            GameGenerationService.ensure_directories()
            
            games = []
            for file in os.listdir(SCENES_DATA_DIR):
                if file.endswith(".json"):
                    scene_id = file.replace(".json", "")
                    scene_file = os.path.join(SCENES_DATA_DIR, file)
                    file_stats = os.stat(scene_file)
                    
                    with open(scene_file, 'r') as f:
                        scene = json.load(f)
                    
                    games.append({
                        "scene_id": scene_id,
                        "title": scene["scene_meta"]["title"],
                        "subject": scene["scene_meta"]["subject"],
                        "grade": scene["scene_meta"]["grade"],
                        "entities": len(scene["entities"]),
                        "file_size": file_stats.st_size,
                        "modified": datetime.fromtimestamp(file_stats.st_mtime).isoformat()
                    })
            
            return {
                "success": True,
                "total_games": len(games),
                "games": games
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to list games: {str(e)}")
            return {
                "success": False,
                "error": str(e)
            }
