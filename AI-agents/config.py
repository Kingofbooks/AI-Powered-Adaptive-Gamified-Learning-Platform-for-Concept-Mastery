"""
Configuration Settings for Teacher AI Agent
Customize these settings to match your requirements
"""

# ============================================
# API Configuration
# ============================================

# LLM Model Selection
LLM_MODEL = "gpt-4o"  # Primary model: gpt-4, gpt-4o, gpt-3.5-turbo
BACKUP_MODEL = "gemini-2.0-flash"  # Secondary Backup: Gemini (free tier)
GROQ_MODEL = "llama-3.1-70b-versatile"  # Tertiary Fallback: Groq (free tier, fast)
OLLAMA_MODEL = "phi3"  # Quaternary: Local Ollama model (phi3, llama2, mistral, etc.)
OLLAMA_URL = "http://localhost:11434/api/generate"  # Ollama local endpoint
USE_GEMINI_BACKUP = True  # Enable Gemini fallback if OpenAI fails
USE_GROQ_BACKUP = True  # Enable Groq fallback if Gemini fails
USE_OLLAMA_BACKUP = False  # Enable Ollama fallback if Groq fails

# API Token Limits
MAX_TOKENS_SUMMARY = 1000      # Tokens for concept summarization
MAX_TOKENS_QUIZ = 2000         # Tokens for quiz generation
MAX_TOKENS_PUZZLE = 1500       # Tokens for puzzle generation
MAX_TOKENS_SPEED = 1500        # Tokens for speed challenge generation

# Request Timeout (seconds)
REQUEST_TIMEOUT = 120
OLLAMA_TIMEOUT = 300  # Ollama (local) needs more time - 5 minutes for phi3

# ============================================
# Game Configuration
# ============================================

# Default Game Parameters
DEFAULT_QUIZ_QUESTIONS = 5
DEFAULT_SPEED_CHALLENGES = 10
DEFAULT_STUDENT_LEVEL = "intermediate"

# Difficulty Levels
DIFFICULTY_LEVELS = ["easy", "medium", "hard"]
STUDENT_LEVELS = ["beginner", "intermediate", "advanced"]

# Time Estimates (in seconds)
TIME_ESTIMATE_QUIZ = 300          # 5 minutes
TIME_ESTIMATE_PUZZLE = 600        # 10 minutes
TIME_ESTIMATE_SPEED = 120         # 2 minutes

# ============================================
# Scoring Configuration
# ============================================

QUIZ_SCORING = {
    "correct_answer": 10,
    "incorrect_answer": 0,
    "time_bonus": 5,
    "streak_multiplier": 1.5
}

PUZZLE_SCORING = {
    "solve": 20,
    "hint_used": -5,
    "time_bonus": 10
}

SPEED_SCORING = {
    "correct": 10,
    "streak_multiplier": 2.0
}

# ============================================
# Server Configuration
# ============================================

SERVER_HOST = "0.0.0.0"
SERVER_PORT = 8001
DEBUG_MODE = False

# CORS Settings
CORS_ORIGINS = ["*"]  # Change to specific domains in production
CORS_ALLOW_CREDENTIALS = True
CORS_ALLOW_METHODS = ["*"]
CORS_ALLOW_HEADERS = ["*"]

# ============================================
# Prompt Templates
# ============================================

SUMMARIZATION_PROMPT_TEMPLATE = """You are an expert teacher. Summarize the following concept into 3-5 clear, 
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

QUIZ_PROMPT_TEMPLATE = """Create an educational quiz game for the following concept:

Topic: {concept}
Number of Questions: {num_questions}
Student Level: {level}

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
    "scoring_rules": {{...}},
    "estimated_duration": 300
}}

Make it engaging, educational, and appropriate for learners."""

# ============================================
# Content Validation
# ============================================

# Maximum concept length for processing
MAX_CONCEPT_LENGTH = 1000

# Minimum concept length
MIN_CONCEPT_LENGTH = 5

# Reasonable bounds for questions/challenges
MAX_QUIZ_QUESTIONS = 50
MIN_QUIZ_QUESTIONS = 1

MAX_SPEED_CHALLENGES = 100
MIN_SPEED_CHALLENGES = 1

# ============================================
# Caching Configuration (Optional)
# ============================================

ENABLE_CACHING = False
CACHE_TTL_SECONDS = 3600  # 1 hour

# ============================================
# Logging Configuration
# ============================================

LOG_LEVEL = "INFO"  # DEBUG, INFO, WARNING, ERROR
LOG_FORMAT = "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
LOG_FILE = "teacher_ai_agent.log"

# ============================================
# Rate Limiting (Optional)
# ============================================

ENABLE_RATE_LIMITING = False
RATE_LIMIT_REQUESTS = 100  # requests
RATE_LIMIT_PERIOD = 3600   # seconds (1 hour)

# ============================================
# Database Configuration (Optional for future)
# ============================================

DATABASE_ENABLED = False
DATABASE_URL = "sqlite:///teacher_ai_agent.db"
# DATABASE_URL = "postgresql://user:password@localhost/teacher_ai_agent"

# ============================================
# Feature Flags
# ============================================

ENABLE_QUIZ_GENERATION = True
ENABLE_PUZZLE_GENERATION = True
ENABLE_SPEED_CHALLENGE_GENERATION = True
ENABLE_FULL_MODULE_GENERATION = True

# ============================================
# Advanced LLM Settings
# ============================================

# Temperature: 0.0 = deterministic, 1.0 = random
LLM_TEMPERATURE = 0.7

# Top P: nucleus sampling (0.0-1.0)
LLM_TOP_P = 0.95

# Presence penalty
LLM_PRESENCE_PENALTY = 0

# Frequency penalty
LLM_FREQUENCY_PENALTY = 0
