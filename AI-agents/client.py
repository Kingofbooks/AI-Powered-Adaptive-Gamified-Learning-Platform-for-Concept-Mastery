"""
Client SDK for EduEngine AI Agent API
This module provides a Python client for interacting with the EduEngine AI Agent API
"""

import requests
from typing import Optional, Dict, Any
import json

class EduEngineAIClient:
    """Client for interacting with EduEngine AI Agent API"""
    
    def __init__(self, base_url: str = "http://localhost:8001"):
        self.base_url = base_url
        self.session = requests.Session()
        
    def health_check(self) -> bool:
        """Check if the API is healthy"""
        try:
            response = self.session.get(f"{self.base_url}/health")
            return response.status_code == 200
        except Exception as e:
            print(f"Health check failed: {e}")
            return False
    
    def generate_scene(self, prompt: str, grade: Optional[str] = None, 
                      subject: Optional[str] = None) -> Dict[str, Any]:
        """
        Generate an EduEngine scene from a natural language prompt
        
        Args:
            prompt: Natural language description (e.g., "teach photosynthesis for grade 5")
            grade: Optional override for grade level (1-12)
            subject: Optional override for subject/theme
            
        Returns:
            Dict containing success status and scene data
        """
        payload = {
            "prompt": prompt,
            "grade": grade,
            "subject": subject
        }
        response = self.session.post(
            f"{self.base_url}/generate-scene",
            json=payload
        )
        response.raise_for_status()
        return response.json()
    
    def generate_scene_and_save(self, prompt: str, grade: Optional[str] = None,
                               subject: Optional[str] = None) -> Dict[str, Any]:
        """
        Generate an EduEngine scene and save it as a JSON file
        
        Args:
            prompt: Natural language description
            grade: Optional grade override
            subject: Optional subject override
            
        Returns:
            Dict containing success status, filepath, and scene data
        """
        payload = {
            "prompt": prompt,
            "grade": grade,
            "subject": subject
        }
        response = self.session.post(
            f"{self.base_url}/generate-scene-file",
            json=payload
        )
        response.raise_for_status()
        return response.json()
    
    def list_themes(self) -> Dict[str, Any]:
        """Get list of all available educational themes"""
        response = self.session.get(f"{self.base_url}/themes")
        response.raise_for_status()
        return response.json()
    
    def get_theme_details(self, theme_name: str) -> Dict[str, Any]:
        """Get detailed information about a specific theme"""
        response = self.session.get(f"{self.base_url}/theme/{theme_name}")
        response.raise_for_status()
        return response.json()
    
    def validate_scene(self, scene: Dict[str, Any]) -> Dict[str, Any]:
        """
        Validate a scene JSON against the EduEngine specification
        
        Args:
            scene: The scene JSON object to validate
            
        Returns:
            Dict containing validation results
        """
        response = self.session.post(
            f"{self.base_url}/validate-scene",
            json=scene
        )
        response.raise_for_status()
        return response.json()


# Backward compatibility alias
class TeacherAIClient(EduEngineAIClient):
    """Backward compatibility alias for EduEngineAIClient"""
    pass


# Example usage
if __name__ == "__main__":
    client = EduEngineAIClient()
    
    # Check if API is running
    if client.health_check():
        print("✅ API is healthy")
        
        # Example 1: Generate a scene
        print("\n🎮 Generating EduEngine scene for 'Photosynthesis Grade 5'...")
        result = client.generate_scene(prompt="teach photosynthesis for grade 5 students")
        
        if result["success"]:
            scene = result["data"]
            print(f"✅ Scene generated successfully!")
            print(f"   ID: {scene['scene_meta']['id']}")
            print(f"   Title: {scene['scene_meta']['title']}")
            print(f"   Grade: {scene['scene_meta']['grade']}")
            print(f"   Entities: {len(scene['entities'])}")
        else:
            print("❌ Failed to generate scene")
        
        # Example 2: List themes
        print("\n📋 Available themes:")
        themes = client.list_themes()
        if themes["success"]:
            for theme_name in list(themes["themes"].keys())[:5]:
                print(f"   - {theme_name}")
        
        # Example 3: Validate a scene
        print("\n✅ Validating scene...")
        if result["success"]:
            validation = client.validate_scene(scene)
            if validation["valid"]:
                print(f"✅ Scene is valid!")
            else:
                print(f"❌ Scene has {validation['error_count']} errors")
                for error in validation["errors"]:
                    print(f"   - {error}")
    else:
        print("❌ API is not healthy. Make sure the server is running on http://localhost:8000")
