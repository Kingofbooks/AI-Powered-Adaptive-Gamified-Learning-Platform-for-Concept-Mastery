from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import Optional, List
import json
import os
from dotenv import load_dotenv
from teacher_ai_agent import TeacherAIAgent

# Load environment variables
load_dotenv()

# Initialize FastAPI app
app = FastAPI(
    title="Teacher AI Agent API",
    description="AI-powered agent that generates interactive mini-games for teaching concepts",
    version="1.0.0"
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Initialize AI Agent
ai_agent = TeacherAIAgent()

# Pydantic models for request/response
class ConceptRequest(BaseModel):
    concept: str
    level: str = "intermediate"

class GameRequest(BaseModel):
    concept: str
    game_type: str
    num_questions: int = 5

class LearningModuleRequest(BaseModel):
    concept: str
    level: str = "intermediate"

class GameInfoRequest(BaseModel):
    concept: str
    game_type: str  # "quiz", "puzzle", or "speed"
    level: str = "intermediate"
    num_questions: int = 5

class HealthResponse(BaseModel):
    status: str
    message: str

# Routes
@app.get("/health", response_model=HealthResponse)
async def health_check():
    """Check API health"""
    return {
        "status": "healthy",
        "message": "Teacher AI Agent API is running"
    }

@app.post("/summarize")
async def summarize_concept(request: ConceptRequest):
    """
    📚 **Summarize a Teaching Concept into Easy-to-Understand Key Points**
    
    Breaks down complex topics into 3-5 clear, concise learning goals perfect for students.
    
    **What it does:**
    - Creates a simple overview of the concept
    - Lists key learning points students should understand
    - Provides real-world examples
    - Suggests learning objectives
    
    **Parameters:**
    - `concept` (required): The topic to explain (e.g., "Photosynthesis", "Newton's Laws")
    - `level` (optional): Student level - "beginner", "intermediate", or "advanced" (default: intermediate)
    
    **Example Request:**
    ```json
    {
        "concept": "Photosynthesis",
        "level": "intermediate"
    }
    ```
    
    **Example Response:**
    ```json
    {
        "success": true,
        "data": {
            "title": "Photosynthesis",
            "level": "intermediate",
            "summary": "Plants convert sunlight into chemical energy through photosynthesis...",
            "key_points": [
                "Light reactions occur in thylakoid membranes",
                "Dark reactions (Calvin cycle) occur in stroma",
                "Produces glucose and oxygen from sunlight and water"
            ],
            "learning_objectives": [
                "Understand the two main stages of photosynthesis",
                "Explain the role of chlorophyll"
            ],
            "examples": [
                "Green plants, algae, some bacteria",
                "Crops convert sunlight to food energy"
            ]
        }
    }
    ```
    
    **Use for:** Introducing a topic, understanding key concepts, preparing lesson plans
    """
    try:
        summary = ai_agent.summarize_concept(
            concept=request.concept,
            level=request.level
        )
        return {
            "success": True,
            "data": summary
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/generate-quiz")
async def generate_quiz(request: GameRequest):
    """
    🎮 **Generate an Interactive Quiz Game**
    
    Creates multiple-choice questions to test and reinforce student understanding.
    Perfect for quick knowledge checks and immediate feedback.
    
    **What it generates:**
    - Multiple-choice questions with 4 answer options
    - Detailed explanations for each answer
    - Difficulty levels for each question
    - Scoring and streak rules
    
    **Parameters:**
    - `concept` (required): Topic for the quiz (e.g., "Photosynthesis")
    - `game_type`: "quiz"
    - `num_questions` (optional): 5-50 questions (default: 5)
    
    **Example Request:**
    ```json
    {
        "concept": "Photosynthesis",
        "game_type": "quiz",
        "num_questions": 5
    }
    ```
    
    **Example Response Contains:**
    ```json
    {
        "success": true,
        "data": {
            "game_type": "quiz",
            "title": "Photosynthesis Quiz",
            "description": "Test your knowledge of photosynthesis processes",
            "questions": [
                {
                    "id": 1,
                    "question": "What is the primary purpose of photosynthesis?",
                    "type": "multiple_choice",
                    "options": [
                        "To release energy",
                        "To produce glucose and oxygen",
                        "To absorb heat",
                        "To purify water"
                    ],
                    "correct_answer": 1,
                    "explanation": "Photosynthesis converts light energy into chemical energy (glucose) and produces oxygen as a byproduct.",
                    "difficulty": "easy"
                }
            ],
            "scoring_rules": {
                "correct_answer": 10,
                "incorrect_answer": 0,
                "time_bonus": 5,
                "streak_multiplier": 1.5
            },
            "estimated_duration": 300
        }
    }
    ```
    
    **How to use in your app:**
    1. Load questions from the response
    2. Display one question at a time
    3. Show 4 answer options
    4. Check answer against correct_answer index
    5. Show explanation immediately
    6. Track streak for multiplier bonus
    7. Calculate final score
    
    **Best for:** Knowledge assessment, concept checks, preparation for exams
    """
    try:
        quiz = ai_agent.generate_quiz_game(
            concept=request.concept,
            num_questions=request.num_questions
        )
        return {
            "success": True,
            "data": quiz
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/generate-puzzle")
async def generate_puzzle(request: ConceptRequest):
    """
    🧩 **Generate an Interactive Puzzle Game**
    
    Creates problem-solving challenges that encourage critical thinking and deeper concept understanding.
    Perfect for helping students apply knowledge, not just recall it.
    
    **What it generates:**
    - Logic puzzles or pattern-matching challenges
    - Progressive hints (3 levels) to help students without giving answers
    - Solution with explanation
    - Educational value description
    
    **Parameters:**
    - `concept` (required): Topic for the puzzle (e.g., "The Water Cycle")
    - `level` (optional): "beginner", "intermediate", or "advanced" (default: intermediate)
    
    **Example Request:**
    ```json
    {
        "concept": "The Water Cycle",
        "level": "intermediate"
    }
    ```
    
    **Example Response Contains:**
    ```json
    {
        "success": true,
        "data": {
            "game_type": "puzzle",
            "title": "Water Cycle Puzzle",
            "description": "Arrange the stages of the water cycle in correct order",
            "puzzle_type": "arrangement",
            "content": {
                "problem_statement": "Put these steps in the correct order: Condensation, Evaporation, Precipitation, Collection",
                "answer": "Evaporation, Condensation, Precipitation, Collection",
                "hints": [
                    "Think about where water starts: in oceans, lakes, rivers",
                    "Heat from sun causes water to become gas - that's first",
                    "Gas rises and cools in atmosphere - what comes next?"
                ]
            },
            "difficulty_level": "intermediate",
            "estimated_duration": 600,
            "educational_value": "Students learn the complete water cycle sequence and understand how each stage connects"
        }
    }
    ```
    
    **How to use in your app:**
    1. Display the problem_statement clearly
    2. Provide input method (text, drag-drop, or selection)
    3. Add "Show Hint" button with 3 progressive hints
    4. Check answer against the provided answer
    5. Show explanation and educational_value when correct
    6. Award points based on hints used and time taken
    
    **Best for:** Developing critical thinking, applying knowledge, deeper learning
    """
    try:
        puzzle = ai_agent.generate_puzzle_game(concept=request.concept)
        return {
            "success": True,
            "data": puzzle
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/generate-speed")
async def generate_speed(request: GameRequest):
    """
    ⚡ **Generate a Speed Challenge Game**
    
    Creates fast-paced, timed challenges that test rapid recall and build fluency.
    Perfect for practice, engagement, and keeping students focused.
    
    **What it generates:**
    - Rapid-fire questions with individual time limits
    - Quick answer prompts (3-10 seconds each)
    - Streak tracking for consecutive correct answers
    - Time-based bonus scoring
    
    **Parameters:**
    - `concept` (required): Topic for challenges (e.g., "Basic Multiplication")
    - `game_type`: "speed"
    - `num_questions` (optional): 10-100 challenges (default: 10)
    
    **Example Request:**
    ```json
    {
        "concept": "Basic Multiplication",
        "game_type": "speed",
        "num_questions": 10
    }
    ```
    
    **Example Response Contains:**
    ```json
    {
        "success": true,
        "data": {
            "game_type": "speed",
            "title": "Multiplication Speed Challenge",
            "description": "Answer multiplication questions as fast as possible",
            "difficulty": "easy",
            "challenges": [
                {
                    "id": 1,
                    "prompt": "What is 7 × 8?",
                    "answer": "56",
                    "time_limit": 5,
                    "difficulty": "easy"
                },
                {
                    "id": 2,
                    "prompt": "What is 9 × 12?",
                    "answer": "108",
                    "time_limit": 5,
                    "difficulty": "easy"
                }
            ],
            "scoring_rules": {
                "correct": 10,
                "time_bonus": "10 - (time_taken * 2)",
                "streak_multiplier": 2.0
            },
            "total_duration": 120
        }
    }
    ```
    
    **How to use in your app:**
    1. Load challenges in sequence
    2. Start countdown timer (time_limit seconds)
    3. Display prompt clearly and large
    4. Accept user input (text or selection)
    5. Check answer immediately when submitted
    6. Show quick feedback (✓ or ✗)
    7. Auto-advance when timer expires
    8. Track streaks of consecutive correct answers
    9. Display running score and time remaining
    10. Show final score + accuracy % + best streak
    
    **Scoring Formula:**
    - Base: 10 points for correct answer
    - Time Bonus: 10 - (seconds taken × 2) [max 10 bonus]
    - Streak Multiplier: Multiply total by 2.0 for each streak level
    
    **Best for:** Building fluency, rapid recall practice, keeping engagement high
    """
    try:
        # Use num_questions as num_challenges for speed game
        speed = ai_agent.generate_speed_game(
            concept=request.concept,
            num_challenges=request.num_questions
        )
        return {
            "success": True,
            "data": speed
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/generate-module")
async def generate_learning_module(request: LearningModuleRequest):
    """
    📚 **Generate a COMPLETE Learning Module (All-in-One)**
    
    The ultimate endpoint! Creates a full learning experience in one call:
    - Concept summary with key points
    - Quiz game for assessment
    - Puzzle game for critical thinking
    - Speed challenge for fluency building
    
    Perfect for creating complete lessons or study units.
    
    **Parameters:**
    - `concept` (required): Main topic (e.g., "Photosynthesis")
    - `level` (optional): "beginner", "intermediate", or "advanced" (default: intermediate)
    
    **Example Request:**
    ```json
    {
        "concept": "Photosynthesis",
        "level": "intermediate"
    }
    ```
    
    **What You Get:**
    1. **Summary** - Concept overview and key points
    2. **Quiz Game** - 5 multiple choice questions
    3. **Puzzle Game** - 1 logic puzzle
    4. **Speed Challenge** - 8 timed rapid-fire questions
    5. **Teaching Notes** - Recommendations on how to use the module
    6. **Metadata** - Total duration, difficulty, etc.
    
    **Recommended Usage Order:**
    1. Start with Quiz for knowledge check
    2. Move to Puzzle for deeper understanding
    3. End with Speed Challenge for fluency reinforcement
    
    **Time Investment:**
    - Total module: 30-40 seconds to generate
    - Total gameplay: 15-30 minutes
    - Can be split across multiple sessions
    
    **Best for:** Complete lessons, study units, comprehensive learning experiences
    """
    try:
        module = ai_agent.generate_full_learning_module(
            concept=request.concept,
            level=request.level
        )
        return {
            "success": True,
            "data": module
        }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/game")
async def generate_game_with_details(request: GameInfoRequest):
    """
    Generate a detailed game object with:
    1. What needs to be generated (objectives, content structure)
    2. Format of the game (JSON structure, data types)
    3. Instructions for executing/playing the game
    4. Complete game data ready to use
    
    This endpoint returns ALL information needed to display and run a game.
    
    Example Request:
    {
        "concept": "Photosynthesis",
        "game_type": "quiz",
        "level": "intermediate",
        "num_questions": 5
    }
    """
    try:
        game_data = None
        instructions = {}
        format_spec = {}
        objectives = []
        
        if request.game_type.lower() == "quiz":
            game_data = ai_agent.generate_quiz_game(
                concept=request.concept,
                num_questions=request.num_questions
            )
            objectives = [
                "Test student understanding of key concepts",
                "Identify knowledge gaps",
                "Provide immediate feedback with explanations"
            ]
            format_spec = {
                "type": "Multiple Choice Questions",
                "structure": {
                    "game_type": "quiz",
                    "title": "string",
                    "description": "string",
                    "questions": [
                        {
                            "id": "number",
                            "question": "string - the question text",
                            "type": "multiple_choice",
                            "options": ["string array of 4 options"],
                            "correct_answer": "number - index of correct option (0-3)",
                            "explanation": "string - why this answer is correct",
                            "difficulty": "easy|medium|hard"
                        }
                    ],
                    "scoring_rules": {
                        "correct_answer": "number - points per correct",
                        "incorrect_answer": "number - points per wrong (usually 0)",
                        "time_bonus": "number - bonus points for speed",
                        "streak_multiplier": "number - multiplier for consecutive correct"
                    },
                    "estimated_duration": "number - seconds"
                }
            }
            instructions = {
                "setup": "Display one question at a time",
                "interaction": "User selects one option from the 4 choices",
                "feedback": "Show immediately if answer is correct/incorrect with explanation",
                "scoring": "Award points based on correctness + time bonus + streak",
                "progress": "Track questions answered and score",
                "completion": "After all questions, show final score and summary"
            }
            
        elif request.game_type.lower() == "puzzle":
            game_data = ai_agent.generate_puzzle_game(concept=request.concept)
            objectives = [
                "Encourage critical thinking and problem-solving",
                "Reinforce conceptual understanding through application",
                "Build higher-order thinking skills"
            ]
            format_spec = {
                "type": "Logic Puzzle / Problem Solving",
                "structure": {
                    "game_type": "puzzle",
                    "title": "string",
                    "description": "string",
                    "puzzle_type": "pattern_matching|word_grid|arrangement|diagram",
                    "content": {
                        "problem_statement": "string - what the student needs to solve",
                        "answer": "string - the correct solution",
                        "hints": ["string array of progressive hints"]
                    },
                    "difficulty_level": "easy|medium|hard",
                    "estimated_duration": "number - seconds",
                    "educational_value": "string - what students learn from solving"
                }
            }
            instructions = {
                "setup": "Present the puzzle with clear problem statement",
                "display_hints": "Show hint button; reveal hints progressively (1, 2, 3)",
                "input": "Accept user answer (could be text, multiple choice, or arrangement)",
                "validation": "Check if answer matches (may need fuzzy matching)",
                "feedback": "Explain why the answer is correct and its educational value",
                "time_tracking": "Record time taken to solve for scoring",
                "completion": "Show score and explanation of the concept"
            }
            
        elif request.game_type.lower() == "speed":
            game_data = ai_agent.generate_speed_game(
                concept=request.concept,
                num_challenges=request.num_questions
            )
            objectives = [
                "Test rapid recall and quick thinking",
                "Build fluency with the concept",
                "Maintain focus and engagement under time pressure"
            ]
            format_spec = {
                "type": "Timed Rapid-Fire Challenges",
                "structure": {
                    "game_type": "speed",
                    "title": "string",
                    "description": "string",
                    "difficulty": "easy|medium|hard",
                    "challenges": [
                        {
                            "id": "number",
                            "prompt": "string - quick question/task",
                            "answer": "string - expected answer",
                            "time_limit": "number - seconds allowed",
                            "difficulty": "easy|medium|hard"
                        }
                    ],
                    "scoring_rules": {
                        "correct": "number - points for correct answer",
                        "time_bonus": "string - formula like '10 - (time_taken * 2)'",
                        "streak_multiplier": "number - multiplier for consecutive correct"
                    },
                    "total_duration": "number - seconds for entire game"
                }
            }
            instructions = {
                "setup": "Start timer and display first challenge",
                "display": "Show countdown timer and current challenge",
                "interaction": "User types or selects answer within time limit",
                "auto_advance": "Move to next question after time expires or answer submitted",
                "feedback": "Quick feedback (correct/incorrect) before moving to next",
                "streak": "Track consecutive correct answers for multiplier bonus",
                "progress": "Show current question number and running score",
                "completion": "Final score, accuracy %, best streak, time analysis"
            }
        else:
            raise HTTPException(status_code=400, detail="Invalid game_type. Must be 'quiz', 'puzzle', or 'speed'")
        
        # Build comprehensive response
        response = {
            "success": True,
            "concept": request.concept,
            "level": request.level,
            "game_type": request.game_type,
            
            # Part 1: What needs to be generated
            "objectives": {
                "learning_objectives": objectives,
                "game_purpose": f"Create an engaging {request.game_type} game to help students master {request.concept}"
            },
            
            # Part 2: Format specification
            "format": format_spec,
            
            # Part 3: Detailed instructions for execution
            "instructions": {
                "game_flow": instructions,
                "step_by_step": [
                    "1. Initialize game with the provided data",
                    "2. Display game title and brief description",
                    "3. Show learning objectives to student",
                    "4. Follow the game_flow instructions for interaction",
                    "5. Track scoring according to scoring_rules",
                    "6. Provide real-time feedback",
                    "7. Display final results and summary"
                ],
                "developer_notes": "Use the 'game_data' section below to populate all UI elements"
            },
            
            # Part 4: The actual game data ready to execute
            "game_data": game_data,
            
            # Additional helpful info
            "metadata": {
                "created_at": "2026-02-16",
                "estimated_duration_minutes": game_data.get("estimated_duration", game_data.get("total_duration", 0)) / 60,
                "difficulty": game_data.get("difficulty_level", game_data.get("difficulty", "intermediate")),
                "requires_internet": False,
                "can_be_cached": True
            }
        }
        
        return response
        
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/game-types")
async def get_game_types():
    """
    Get detailed information about all available game types
    """
    return {
        "available_games": [
            {
                "type": "quiz",
                "name": "Multiple Choice Quiz",
                "description": "Test knowledge with multiple choice questions",
                "best_for": "Quick assessment, concept checks, knowledge verification",
                "time_range": "5-15 minutes",
                "interaction": "Select answer from 4 options",
                "difficulty_levels": ["easy", "medium", "hard"],
                "parameters": {
                    "num_questions": "5-50 (default: 5)"
                }
            },
            {
                "type": "puzzle",
                "name": "Logic Puzzle",
                "description": "Solve problems using critical thinking",
                "best_for": "Deep understanding, problem-solving skills, application",
                "time_range": "10-20 minutes",
                "interaction": "Type answer, arrange items, or select arrangement",
                "difficulty_levels": ["easy", "medium", "hard"],
                "parameters": {
                    "hints_available": "3 progressive hints provided"
                }
            },
            {
                "type": "speed",
                "name": "Speed Challenge",
                "description": "Rapid-fire questions against the clock",
                "best_for": "Fluency building, rapid recall, engagement, practice",
                "time_range": "2-5 minutes",
                "interaction": "Quick typing or selection with timer",
                "difficulty_levels": ["easy", "medium", "hard"],
                "parameters": {
                    "num_challenges": "10-100 (default: 10)",
                    "time_per_question": "3-10 seconds"
                }
            }
        ]
    }

@app.get("/")
async def root():
    """API root endpoint with documentation"""
    return {
        "name": "Teacher AI Agent API",
        "version": "1.0.0",
        "description": "AI-powered agent that generates interactive educational mini-games",
        "endpoints": {
            "health": "/health",
            "summarize_concept": "/summarize (POST)",
            "generate_quiz": "/generate-quiz (POST)",
            "generate_puzzle": "/generate-puzzle (POST)",
            "generate_speed": "/generate-speed (POST)",
            "generate_full_module": "/generate-module (POST)",
            "docs": "/docs"
        },
        "features": [
            "Summarize teaching concepts",
            "Generate interactive quiz games",
            "Generate puzzle-based games",
            "Generate speed challenge games",
            "Create complete learning modules"
        ]
    }

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
