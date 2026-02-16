"""
Client SDK for Teacher AI Agent API
This module provides a Python client for interacting with the Teacher AI Agent API
"""

import requests
from typing import Optional, Dict, Any
import json

class TeacherAIClient:
    """Client for interacting with Teacher AI Agent API"""
    
    def __init__(self, base_url: str = "http://localhost:8000"):
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
            
    def summarize_concept(self, concept: str, level: str = "intermediate") -> Dict[str, Any]:
        """Summarize a teaching concept"""
        payload = {
            "concept": concept,
            "level": level
        }
        response = self.session.post(
            f"{self.base_url}/summarize",
            json=payload
        )
        response.raise_for_status()
        return response.json()
        
    def generate_quiz(self, concept: str, num_questions: int = 5) -> Dict[str, Any]:
        """Generate a quiz game"""
        payload = {
            "concept": concept,
            "game_type": "quiz",
            "num_questions": num_questions
        }
        response = self.session.post(
            f"{self.base_url}/generate-quiz",
            json=payload
        )
        response.raise_for_status()
        return response.json()
        
    def generate_puzzle(self, concept: str) -> Dict[str, Any]:
        """Generate a puzzle game"""
        payload = {
            "concept": concept
        }
        response = self.session.post(
            f"{self.base_url}/generate-puzzle",
            json=payload
        )
        response.raise_for_status()
        return response.json()
        
    def generate_speed_game(self, concept: str, num_challenges: int = 10) -> Dict[str, Any]:
        """Generate a speed challenge game"""
        payload = {
            "concept": concept,
            "game_type": "speed",
            "num_questions": num_challenges
        }
        response = self.session.post(
            f"{self.base_url}/generate-speed",
            json=payload
        )
        response.raise_for_status()
        return response.json()
        
    def generate_learning_module(self, concept: str, level: str = "intermediate") -> Dict[str, Any]:
        """Generate a complete learning module with all games"""
        payload = {
            "concept": concept,
            "level": level
        }
        response = self.session.post(
            f"{self.base_url}/generate-module",
            json=payload
        )
        response.raise_for_status()
        return response.json()

# Example usage
if __name__ == "__main__":
    client = TeacherAIClient()
    
    # Check if API is running
    if client.health_check():
        print("✅ API is healthy")
        
        # Example: Generate a learning module
        print("\n📚 Generating learning module for 'Photosynthesis'...")
        result = client.generate_learning_module(
            concept="Photosynthesis",
            level="intermediate"
        )
        
        if result["success"]:
            module = result["data"]
            print(f"✅ Module generated successfully!")
            print(f"   Title: {module['summary']['title']}")
            print(f"   Games: {list(module['games'].keys())}")
            print(f"   Total duration: {module['estimated_total_duration']} seconds")
        else:
            print("❌ Failed to generate module")
    else:
        print("❌ API is not healthy. Make sure the server is running on http://localhost:8000")
