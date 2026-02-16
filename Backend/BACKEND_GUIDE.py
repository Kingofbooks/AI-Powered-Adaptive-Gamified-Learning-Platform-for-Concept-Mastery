#!/usr/bin/env python3
"""
INTEGRATED BACKEND SETUP & USAGE GUIDE

This backend connects the AI engine, database, and frontend into one unified system.
"""

ARCHITECTURE = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                    INTEGRATED BACKEND ARCHITECTURE                           ║
╚══════════════════════════════════════════════════════════════════════════════╝

                              FRONTEND (React)
                                    ↓
                            [HTTP REST API]
                                    ↓
          ╔═════════════════════════════════════╗
          │                                     │
          │        INTEGRATED BACKEND (api.py)  │
          │                                     │
          ├─────────────────────────────────────┤
          │  API Endpoints:                     │
          │  ✓ /users/* - User management      │
          │  ✓ /games/* - Game generation      │
          │  ✓ /progress/* - Progress tracking │
          │  ✓ /dashboard/* - Dashboard data   │
          │  ✓ /concepts/* - Concept info      │
          └─────────────────────────────────────┘
                 ↓              ↓              ↓
          ┌─────────────┐  ┌──────────┐  ┌──────────┐
          │  Services   │  │ Teacher  │  │Database  │
          │  (services) │  │ AI Agent │  │(SQLite)  │
          └─────────────┘  └──────────┘  └──────────┘
                  ↓              ↓
          [Business Logic]  [Game Generation]
"""

FILE_STRUCTURE = """
Backend/
├── api.py                  ← Main integrated API server
├── services.py             ← Business logic layer
├── database.py             ← Database models & setup
├── schemas.py              ← Pydantic request/response models
├── teacher_ai_agent.py     ← AI game generation engine
├── config.py               ← Configuration
├── requirements.txt        ← Python dependencies
├── .env                    ← Environment variables (OPENAI_API_KEY)
├── game_platform.db        ← SQLite database (auto-created)
├── test_api.py             ← API tests
└── README_BACKEND.md       ← Backend documentation
"""

SETUP_STEPS = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                         SETUP INSTRUCTIONS                                   ║
╚══════════════════════════════════════════════════════════════════════════════╝

STEP 1: Install Dependencies
   Command: pip install -r Backend/requirements.txt
   
   What it installs:
   ✓ FastAPI & Uvicorn (web framework)
   ✓ SQLAlchemy (ORM for database)
   ✓ OpenAI (AI engine)
   ✓ Pydantic (data validation)
   ✓ Python-dotenv (environment variables)
   ✓ Testing utilities

STEP 2: Configure Environment
   1. Copy your OPENAI_API_KEY to Backend/.env
      OPENAI_API_KEY=sk-proj-xxxxxxxxxxxx
   
   2. (Optional) Set DATABASE_URL for PostgreSQL
      Default uses SQLite (game_platform.db)

STEP 3: Initialize Database
   Command: python -c "from Backend.database import init_db; init_db()"
   
   Creates tables:
   ✓ users (student accounts)
   ✓ concepts (topics to learn)
   ✓ game_results (game scores & results)
   ✓ progress (mastery tracking)
   ✓ learning_paths (course sequences)
   ✓ cache_data (game caching)

STEP 4: Start the Backend
   Command: python -m uvicorn Backend.api:app --reload --port 8000
   
   Expected output:
   ✓ INFO:     Uvicorn running on http://127.0.0.1:8000
   ✓ INFO:     Application startup complete
   
   Access interactive docs: http://localhost:8000/docs

STEP 5: Test Connection
   In browser or terminal:
   curl http://localhost:8000/health
   
   Expected response:
   {
       "status": "healthy",
       "message": "AI Learning Platform Backend is running",
       "timestamp": "2024-01-15T..."
   }
"""

ENDPOINTS_OVERVIEW = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                          API ENDPOINTS OVERVIEW                              ║
╚══════════════════════════════════════════════════════════════════════════════╝

🏥 SYSTEM
  GET  /health                          System status
  GET  /                                API information

👤 USERS
  POST /users/register                  Create new student account
  POST /users/login                     Student login
  GET  /users/{user_id}                 Get user profile
  GET  /users/{user_id}/stats           Get user statistics

🎮 GAMES
  POST /games/generate                  Generate a game
  POST /games/quiz/submit               Submit quiz answers
  POST /games/puzzle/submit             Submit puzzle solution
  POST /games/speed/submit              Submit speed challenge
  GET  /games/results/{user_id}         Get game result history

📊 PROGRESS
  GET  /progress/{user_id}              Get all progress data
  GET  /progress/{user_id}/concept/{name}  Get concept progress

📚 CONCEPTS
  GET  /concepts                        List all concepts
  GET  /concepts/{name}                 Get concept details

🎯 DASHBOARD
  GET  /dashboard/{user_id}             Get complete dashboard data

🎓 RECOMMENDATIONS
  GET  /recommendations/{user_id}       Get recommended next concepts

📖 LEARNING PATHS
  POST /learning-paths                  Create learning path
  GET  /learning-paths/{user_id}        Get user's learning paths
"""

QUICK_START_EXAMPLE = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                        QUICK START EXAMPLE                                   ║
╚══════════════════════════════════════════════════════════════════════════════╝

Example workflow to test the backend:

1. Register a user:
   curl -X POST http://localhost:8000/users/register \\
     -H "Content-Type: application/json" \\
     -d '{
       "username": "john_doe",
       "email": "john@example.com",
       "password": "password123",
       "level": "beginner"
     }'
   
   Response:
   {
     "id": 1,
     "username": "john_doe",
     "email": "john@example.com",
     "total_score": 0,
     "total_games_played": 0,
     "created_at": "2024-01-15T..."
   }

2. Generate a quiz game:
   curl -X POST http://localhost:8000/games/generate \\
     -H "Content-Type: application/json" \\
     -d '{
       "concept": "Photosynthesis",
       "game_type": "quiz",
       "difficulty": "intermediate",
       "num_questions": 5,
       "user_id": 1
     }'
   
   Response: Complete game with questions, objectives, format, instructions

3. Submit quiz answers:
   curl -X POST http://localhost:8000/games/quiz/submit \\
     -H "Content-Type: application/json" \\
     -d '{
       "game_id": "Photosynthesis_quiz_...",
       "user_id": 1,
       "concept": "Photosynthesis",
       "answers": [0, 1, 2, 0, 1],
       "time_spent": 180
     }'
   
   Response: Game result with score and accuracy

4. Get dashboard data:
   curl http://localhost:8000/dashboard/1
   
   Response: User stats, progress, recent games, recommendations

5. Get recommendations:
   curl http://localhost:8000/recommendations/1
   
   Response: Next concepts to learn based on progress
"""

FRONTEND_INTEGRATION = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                    FRONTEND INTEGRATION GUIDE                                ║
╚══════════════════════════════════════════════════════════════════════════════╝

In your React frontend, use this pattern:

1. Initialize API client:
   const API_BASE = "http://localhost:8000"
   
   async function registerUser(username, email, password) {
       const response = await fetch(`${API_BASE}/users/register`, {
           method: "POST",
           headers: { "Content-Type": "application/json" },
           body: JSON.stringify({ username, email, password, level: "beginner" })
       })
       return response.json()
   }

2. Generate and play a game:
   async function playGame(userId, concept, gameType) {
       // Generate game
       const gameRes = await fetch(`${API_BASE}/games/generate`, {
           method: "POST",
           headers: { "Content-Type": "application/json" },
           body: JSON.stringify({
               concept: concept,
               game_type: gameType,
               difficulty: "intermediate",
               num_questions: 5,
               user_id: userId
           })
       })
       const game = await gameRes.json()
       
       // Display game to user...
       // User plays game...
       
       // Submit results
       const submitRes = await fetch(`${API_BASE}/games/quiz/submit`, {
           method: "POST",
           headers: { "Content-Type": "application/json" },
           body: JSON.stringify({
               game_id: game.id,
               user_id: userId,
               concept: concept,
               answers: userAnswers,
               time_spent: elapsedTime
           })
       })
       return submitRes.json()
   }

3. Load dashboard:
   async function loadDashboard(userId) {
       const response = await fetch(`${API_BASE}/dashboard/${userId}`)
       return response.json()
   }

See ../Frontend/src/api/BackendClient.js for complete client library
"""

DATABASE_MODELS = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                      DATABASE MODELS                                         ║
╚══════════════════════════════════════════════════════════════════════════════╝

Users
├── id (primary key)
├── username (unique)
├── email (unique)
├── level (beginner/intermediate/advanced)
├── total_score
├── total_games_played
└── created_at, last_login

Concepts
├── id (primary key)
├── name (unique)
├── subject
├── description
├── difficulty
└── created_at

GameResults
├── id (primary key)
├── user_id (foreign key → Users)
├── concept_id (foreign key → Concepts)
├── game_type (quiz/puzzle/speed)
├── score
├── max_score
├── accuracy
├── time_spent
├── answers (JSON)
└── created_at

Progress (Mastery Tracking)
├── id (primary key)
├── user_id (foreign key → Users)
├── concept_id (foreign key → Concepts)
├── times_attempted
├── best_score
├── average_score
├── average_accuracy
├── mastery_level (0-100)
└── last_attempted

LearningPaths (Course Sequences)
├── id (primary key)
├── user_id (foreign key → Users)
├── custom_name
├── concepts (JSON array)
├── completed_concepts
├── is_completed
└── created_at

CacheData (Performance)
├── id (primary key)
├── concept
├── game_type
├── content (JSON - cached game)
├── expires_at
└── created_at
"""

SERVICES_LAYER = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                    SERVICES LAYER (Business Logic)                           ║
╚══════════════════════════════════════════════════════════════════════════════╝

UserService
├── create_user() - Create new student account
├── get_user() - Retrieve user by ID
├── get_user_stats() - Calculate user statistics
└── ... more user operations

GameService
├── generate_or_get_cached_game() - Generate or cache game
├── save_game_result() - Store result in database
└── get_game_results() - Retrieve past results

ProgressService
├── update_progress() - Update mastery tracking
├── get_user_progress() - Get all progress data
└── get_recommended_next_concepts() - AI recommendations

ConceptService
├── get_or_create_concept() - Concept management
├── get_all_concepts() - List all topics
└── get_concept_by_name() - Lookup concept

DashboardService
└── get_dashboard_data() - Comprehensive dashboard

LearningPathService
├── create_learning_path() - Create course sequence
└── update_learning_path_progress() - Track completion
"""

DEPLOYMENT_OPTIONS = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                    DEPLOYMENT OPTIONS                                        ║
╚══════════════════════════════════════════════════════════════════════════════╝

LOCAL DEVELOPMENT
├── Database: SQLite (included)
├── Server: python -m uvicorn Backend.api:app --reload
└── Time: Immediate

PRODUCTION - HEROKU
1. Add Procfile:
   web: gunicorn Backend.api:app

2. Deploy:
   git push heroku main

PRODUCTION - AWS EC2
1. Create security group (port 8000)
2. Run supervisor to manage uvicorn
3. Setup nginx as reverse proxy
4. Use RDS for database

PRODUCTION - DOCKER
1. Create Dockerfile:
   FROM python:3.11
   WORKDIR /app
   COPY requirements.txt .
   RUN pip install -r requirements.txt
   COPY . .
   CMD ["python", "-m", "uvicorn", "Backend.api:app", "--host", "0.0.0.0"]

2. Build & run:
   docker build -t learning-platform .
   docker run -p 8000:8000 learning-platform
"""

TROUBLESHOOTING = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                    TROUBLESHOOTING                                           ║
╚══════════════════════════════════════════════════════════════════════════════╝

Problem: "ModuleNotFoundError: No module named 'teacher_ai_agent'"
Solution: Install: pip install -r Backend/requirements.txt
          Or: Add path: import sys; sys.path.append("Backend/")

Problem: "CORS error from frontend"
Solution: CORS is already enabled in api.py for all origins
          If still failing, check browser console for specific error

Problem: "Database is locked"
Solution: Close other connections to SQLite
          Switch to PostgreSQL for production

Problem: "Games take too long to generate"
Solution: This is normal - AI generation takes 5-10 seconds per game
          Implement loading spinners in frontend
          Caching improves subsequent requests

Problem: "API returns 500 error"
Solution: Check Backend/logs for errors
          Verify OPENAI_API_KEY is set
          Check if teacher_ai_agent.py can import

Problem: "User data not persisting"
Solution: Verify database.db exists
          Check database initialization ran successfully
          Verify database folder has write permissions
"""

if __name__ == "__main__":
    print("╔════════════════════════════════════════════════════════════╗")
    print("║         INTEGRATED BACKEND - SETUP & USAGE GUIDE          ║")
    print("╚════════════════════════════════════════════════════════════╝")
    print()
    print(ARCHITECTURE)
    print()
    print(SETUP_STEPS)
    print()
    print(ENDPOINTS_OVERVIEW)
    print()
    print(QUICK_START_EXAMPLE)
    print()
    print(FRONTEND_INTEGRATION)
    print()
    print(DATABASE_MODELS)
    print()
    print(SERVICES_LAYER)
    print()
    print(DEPLOYMENT_OPTIONS)
    print()
    print(TROUBLESHOOTING)
    print()
    print("════════════════════════════════════════════════════════════")
    print("✅ Backend setup complete!")
    print("   Start with: python -m uvicorn Backend.api:app --reload")
    print("   Docs at: http://localhost:8000/docs")
    print()
