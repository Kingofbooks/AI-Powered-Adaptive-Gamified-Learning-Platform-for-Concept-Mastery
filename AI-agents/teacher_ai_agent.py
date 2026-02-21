import json
import os
from typing import Optional
from dotenv import load_dotenv
from openai import OpenAI
from enum import Enum

# Load environment variables from .env file
load_dotenv()

class GameType(str, Enum):
    QUIZ = "quiz"
    PUZZLE = "puzzle"
    SPEED = "speed"

class TeacherAIAgent:
    """AI Agent that helps teachers create interactive mini-games for concept mastery"""
    
    def __init__(self, api_key: Optional[str] = None):
        self.api_key = api_key or os.getenv("OPENAI_API_KEY")
        self.client = OpenAI(api_key=self.api_key)
        self.conversation_history = []
        
    def reset_conversation(self):
        """Reset conversation history for a new teaching concept"""
        self.conversation_history = []
        
    def summarize_concept(self, concept: str, level: str = "intermediate") -> dict:
        """
        Summarize a teaching concept into simplified key points
        
        Args:
            concept: The teaching concept to summarize
            level: Student level - "beginner", "intermediate", or "advanced"
        """
        prompt = f"""You are an expert teacher. Summarize the following concept into 3-5 clear, 
        concise key points suitable for a {level}-level student.
        
        Concept: {concept}
        
        Provide the summary in JSON format with:
        {{
            "title": "concept name",
            "level": "{level}",
            "summary": "2-3 sentence overview",
            "key_points": ["point 1", "point 2", "point 3"],
            "learning_objectives": ["objective 1", "objective 2"],
            "examples": ["example 1", "example 2"]
        }}"""
        
        response = self.client.chat.completions.create(
            model="gpt-4o",
            max_tokens=1000,
            messages=[{"role": "user", "content": prompt}]
        )
        
        response_text = response.choices[0].message.content
        
        # Extract JSON from response
        try:
            # Try to parse the response directly
            summary_data = json.loads(response_text)
        except json.JSONDecodeError:
            # If that fails, try to extract JSON from the response
            import re
            json_match = re.search(r'\{.*\}', response_text, re.DOTALL)
            if json_match:
                summary_data = json.loads(json_match.group())
            else:
                summary_data = {
                    "title": concept,
                    "level": level,
                    "summary": response_text,
                    "key_points": [],
                    "learning_objectives": [],
                    "examples": []
                }
        
        self.conversation_history.append({
            "role": "user",
            "content": prompt
        })
        self.conversation_history.append({
            "role": "assistant",
            "content": response_text
        })
        
        return summary_data
        
    def generate_quiz_game(self, concept: str, num_questions: int = 5) -> dict:
        """Generate an interactive quiz game based on a concept"""
        prompt = f"""Create an educational quiz game for the following concept:
        
        Topic: {concept}
        Number of Questions: {num_questions}
        
        Return ONLY a valid JSON object (no markdown, no extra text) with this exact structure:
        {{
            "game_type": "quiz",
            "title": "Quiz Title",
            "description": "Brief description",
            "learning_objectives": ["objective 1", "objective 2"],
            "questions": [
                {{
                    "id": 1,
                    "question": "What is...?",
                    "type": "multiple_choice",
                    "options": ["A", "B", "C", "D"],
                    "correct_answer": 0,
                    "explanation": "Why this is correct...",
                    "difficulty": "easy"
                }}
            ],
            "scoring_rules": {{
                "correct_answer": 10,
                "incorrect_answer": 0,
                "time_bonus": 5,
                "streak_multiplier": 1.5
            }},
            "estimated_duration": 300
        }}
        
        Make it engaging, educational, and appropriate for learners. Focus on conceptual understanding.
        IMPORTANT: Return ONLY valid JSON, nothing else."""
        
        response = self.client.chat.completions.create(
            model="gpt-4o",
            max_tokens=2000,
            messages=[{"role": "user", "content": prompt}]
        )
        
        response_text = response.choices[0].message.content.strip()
        
        try:
            game_data = json.loads(response_text)
        except json.JSONDecodeError:
            import re
            json_match = re.search(r'\{.*\}', response_text, re.DOTALL)
            if json_match:
                game_data = json.loads(json_match.group())
            else:
                game_data = {
                    "game_type": "quiz",
                    "title": f"Quiz: {concept}",
                    "description": response_text,
                    "questions": [],
                    "estimated_duration": 300
                }
        
        return game_data
        
    def generate_puzzle_game(self, concept: str) -> dict:
        """Generate an interactive puzzle game based on a concept"""
        prompt = f"""Create an educational puzzle game for the following concept:
        
        Topic: {concept}
        
        Return ONLY a valid JSON object (no markdown, no extra text) with this exact structure:
        {{
            "game_type": "puzzle",
            "title": "Puzzle Title",
            "description": "Brief description",
            "learning_objectives": ["objective 1", "objective 2"],
            "puzzle_type": "pattern_matching|word_grid|arrangement|diagram",
            "content": {{
                "problem_statement": "What students need to solve",
                "answer": "The correct answer/solution",
                "hints": ["hint 1", "hint 2", "hint 3"]
            }},
            "difficulty_level": "easy",
            "estimated_duration": 600,
            "educational_value": "How this puzzle reinforces learning"
        }}
        
        Make it interactive, encourage critical thinking, and reinforce the concept.
        IMPORTANT: Return ONLY valid JSON, nothing else."""
        
        response = self.client.chat.completions.create(
            model="gpt-4o",
            messages=[{"role": "user", "content": prompt}]
        )
        
        response_text = response.choices[0].message.content.strip()
        
        try:
            game_data = json.loads(response_text)
        except json.JSONDecodeError:
            import re
            json_match = re.search(r'\{.*\}', response_text, re.DOTALL)
            if json_match:
                game_data = json.loads(json_match.group())
            else:
                game_data = {
                    "game_type": "puzzle",
                    "title": f"Puzzle: {concept}",
                    "description": response_text,
                    "estimated_duration": 600
                }
        
        return game_data
        
    def generate_speed_game(self, concept: str, num_challenges: int = 10) -> dict:
        """Generate a speed challenge game based on a concept"""
        prompt = f"""Create a fast-paced speed challenge game for the following concept:
        
        Topic: {concept}
        Number of Challenges: {num_challenges}
        
        Return ONLY a valid JSON object (no markdown, no extra text) with this exact structure:
        {{
            "game_type": "speed",
            "title": "Speed Challenge Title",
            "description": "Brief description",
            "learning_objectives": ["objective 1", "objective 2"],
            "difficulty": "easy",
            "challenges": [
                {{
                    "id": 1,
                    "prompt": "Quick question or task",
                    "answer": "Expected answer",
                    "time_limit": 5,
                    "difficulty": "easy"
                }}
            ],
            "scoring_rules": {{
                "correct": 10,
                "time_bonus": "10 - (time_taken * 2)",
                "streak_multiplier": 2.0
            }},
            "total_duration": 120
        }}
        
        Focus on rapid recall and quick thinking while reinforcing the concept.
        IMPORTANT: Return ONLY valid JSON, nothing else."""
        
        response = self.client.chat.completions.create(
            model="gpt-4o",
            messages=[{"role": "user", "content": prompt}]
        )
        
        response_text = response.choices[0].message.content.strip()
        
        try:
            game_data = json.loads(response_text)
        except json.JSONDecodeError:
            import re
            json_match = re.search(r'\{.*\}', response_text, re.DOTALL)
            if json_match:
                game_data = json.loads(json_match.group())
            else:
                game_data = {
                    "game_type": "speed",
                    "title": f"Speed Challenge: {concept}",
                    "description": response_text,
                    "challenges": [],
                    "total_duration": 120
                }
        
        return game_data
        
    def generate_full_learning_module(self, concept: str, level: str = "intermediate") -> dict:
        """
        Generate a complete learning module with summary and multiple game types
        
        Args:
            concept: The teaching concept
            level: Student level
        """
        print(f"Generating learning module for: {concept}")
        
        # Step 1: Summarize the concept
        print("Step 1: Summarizing concept...")
        summary = self.summarize_concept(concept, level)
        
        # Step 2: Generate different game types
        print("Step 2: Generating quiz game...")
        quiz = self.generate_quiz_game(concept, num_questions=5)
        
        print("Step 3: Generating puzzle game...")
        puzzle = self.generate_puzzle_game(concept)
        
        print("Step 4: Generating speed game...")
        speed = self.generate_speed_game(concept, num_challenges=8)
        
        # Combine into a complete module
        module = {
            "concept": concept,
            "level": level,
            "summary": summary,
            "games": {
                "quiz": quiz,
                "puzzle": puzzle,
                "speed": speed
            },
            "estimated_total_duration": 
                quiz.get("estimated_duration", 300) + 
                puzzle.get("estimated_duration", 600) + 
                speed.get("total_duration", 120),
            "teaching_notes": f"Module created to help students master: {concept}. "
                             f"Recommended to use games in sequence: Quiz → Puzzle → Speed Challenge"
        }
        
        return module
