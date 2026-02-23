"""
Integrated Backend API
Main server that connects AI engine, database, and frontend
"""

from fastapi import FastAPI, Depends, HTTPException, status, Header
from fastapi.middleware.cors import CORSMiddleware
from sqlalchemy.orm import Session
from datetime import datetime
from typing import Optional
import logging

from game_generation_service import GameGenerationService
from database import get_db
from schemas import (
    TokenResponse, RegisterRequest, LoginRequest, UserResponse,
    GameRequest, GameResponse, GameResultResponse, UserCreate, UserLogin,
    UserStats, QuizSubmission, PuzzleSubmission, SpeedSubmission,
    UserProgressResponse, LearningPathResponse, LearningPathCreate, 
    GamePromptRequest, GameGenerationResponse
)


# Add this after the existing imports
# ============================================


# Setup logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Initialize FastAPI
app = FastAPI(
    title="AI Learning Platform Backend",
    description="Integrated backend connecting AI, database, and frontend",
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

# ============================================
# DEPENDENCY INJECTION
# ============================================

def get_current_user(
    authorization: Optional[str] = Header(None),
    db: Session = Depends(get_db)
) -> dict:
    """
    Dependency to verify JWT token and get current user
    
    Usage: @app.get("/protected")
           async def protected_route(current_user = Depends(get_current_user)):
    """
    
    if not authorization:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Authorization header missing"
        )
    
    try:
        # Extract token from "Bearer <token>"
        scheme, token = authorization.split()
        if scheme.lower() != "bearer":
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid authentication scheme"
            )
        
        # Decode token
        payload = decode_access_token(token)
        if not payload:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid or expired token"
            )
        
        # Get user from database
        user_id = payload.get("sub")
        user = UserService.get_user(db, user_id)
        
        if not user or not user.is_active:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="User not found or inactive"
            )
        
        return {"user": user, "payload": payload}
        
    except ValueError:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid authorization header format"
        )
    except Exception as e:
        logger.error(f"Auth error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Authentication failed"
        )

# Initialize database on startup
@app.on_event("startup")
async def startup():
    """Initialize database on app startup"""
    try:
        init_db()
        logger.info("✅ Database initialized")
    except Exception as e:
        logger.error(f"❌ Database initialization failed: {str(e)}")


# ============================================
# HEALTH CHECK
# ============================================

@app.get("/health")
async def health_check():
    """
    🏥 Health Check Endpoint
    
    Verifies that the backend is running and ready to serve requests.
    
    Returns:
        - status: "healthy"
        - message: Description of system status
        - timestamp: Current server time
    """
    return {
        "status": "healthy",
        "message": "AI Learning Platform Backend is running",
        "timestamp": datetime.utcnow()
    }


# ============================================
# AUTHENTICATION ENDPOINTS
# ============================================

@app.post("/auth/register", response_model=TokenResponse)
async def register(request: RegisterRequest, db: Session = Depends(get_db)):
    """
    📝 Register New User
    
    Register a new user account (Student or Teacher).
    
    Request JSON:
    ```json
    {
        "name": "John Doe",
        "email": "john@example.com",
        "username": "johndoe",
        "password": "SecurePassword123!",
        "role": "student",
        "college": "MIT",
        "department": "Computer Science",
        "year": "2nd Year"
    }
    ```
    
    For Teachers:
    ```json
    {
        "name": "Jane Smith",
        "email": "jane@example.com",
        "username": "janesmith",
        "password": "SecurePassword123!",
        "role": "teacher",
        "experience": "5 years",
        "subjects": ["Mathematics", "Physics"]
    }
    ```
    
    Returns: Access token and user info
    """
    try:
        # Register the user
        user, message = UserService.register_user(
            db,
            name=request.name,
            email=request.email,
            username=request.username,
            password=request.password,
            role=request.role,
            college=request.college,
            department=request.department,
            year=request.year,
            experience=request.experience,
            subjects=request.subjects
        )
        
        # Create JWT token
        access_token = create_access_token(
            data={
                "sub": user.id,
                "username": user.username,
                "email": user.email,
                "role": user.role.value
            }
        )
        
        logger.info(f"✅ User registered: {user.username}")
        
        return TokenResponse(
            access_token=access_token,
            token_type="bearer",
            user=UserResponse(
                id=user.id,
                username=user.username,
                email=user.email,
                name=user.name,
                role=UserRoleEnum(user.role.value),
                created_at=user.created_at,
                is_active=user.is_active,
                level=getattr(user, 'level', None),
                total_score=getattr(user, 'total_score', 0),
                total_games_played=getattr(user, 'total_games_played', 0),
                college=getattr(user, 'college', None),
                department=getattr(user, 'department', None),
                year=getattr(user, 'year', None),
                experience=getattr(user, 'experience', None),
                subjects=getattr(user, 'subjects', None)
            )
        )
        
    except ValueError as e:
        logger.warning(f"Registration validation error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=str(e)
        )
    except Exception as e:
        import traceback
        logger.error(f"Registration error: {str(e)}")
        logger.error(f"Traceback: {traceback.format_exc()}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Registration failed: {str(e)}"
        )


@app.post("/auth/login", response_model=TokenResponse)
async def login(request: LoginRequest, db: Session = Depends(get_db)):
    """
    🔐 User Login
    
    Authenticate user with email and password.
    
    Request JSON:
    ```json
    {
        "email": "john@example.com",
        "password": "SecurePassword123!",
        "role": "student"
    }
    ```
    
    Returns: Access token and user info
    """
    try:
        # Authenticate user
        user = UserService.authenticate_user(
            db,
            email=request.email,
            password=request.password,
            role=request.role
        )
        
        if not user:
            logger.warning(f"Failed login attempt for {request.email} as {request.role.value}")
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Invalid email, password, or role"
            )
        
        # Create JWT token
        access_token = create_access_token(
            data={
                "sub": user.id,
                "username": user.username,
                "email": user.email,
                "role": user.role.value
            }
        )
        
        logger.info(f"✅ User logged in: {user.username}")
        
        return TokenResponse(
            access_token=access_token,
            token_type="bearer",
            user=UserResponse(
                id=user.id,
                username=user.username,
                email=user.email,
                name=user.name,
                role=UserRoleEnum(user.role.value),
                created_at=user.created_at,
                is_active=user.is_active,
                level=getattr(user, 'level', None),
                total_score=getattr(user, 'total_score', 0),
                total_games_played=getattr(user, 'total_games_played', 0),
                college=getattr(user, 'college', None),
                department=getattr(user, 'department', None),
                year=getattr(user, 'year', None),
                experience=getattr(user, 'experience', None),
                subjects=getattr(user, 'subjects', None)
            )
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"Login error: {str(e)}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Login failed"
        )


@app.get("/auth/verify")
async def verify_token(current_user = Depends(get_current_user)):
    """
    ✅ Verify Token
    
    Verify that the provided JWT token is valid.
    
    Headers:
    ```
    Authorization: Bearer <token>
    ```
    
    Returns: User info and token validity
    """
    user = current_user["user"]
    return {
        "valid": True,
        "user_id": user.id,
        "username": user.username,
        "email": user.email,
        "role": user.role.value,
        "message": "Token is valid"
    }

# ============================================
# USER ENDPOINTS
# ============================================

@app.post("/users/register", response_model=UserResponse)
async def register_user(user: UserCreate, db: Session = Depends(get_db)):
    """
    👤 Register New User (Legacy)
    
    Create a new student account.
    
    Request JSON:
    {
        "username": "john_doe",
        "email": "john@example.com",
        "password": "secure_password",
        "level": "beginner"
    }
    
    Returns: User profile with ID and metadata
    """
    try:
        # Check if user exists
        existing = UserService.get_user_by_username(db, user.username)
        if existing:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Username already registered"
            )
        
        # Create user
        db_user = UserService.create_user(
            db, user.username, user.email, user.password, user.level
        )
        return db_user
    except Exception as e:
        logger.error(f"Registration error: {str(e)}")
        raise HTTPException(status_code=400, detail=str(e))


@app.post("/users/login", response_model=UserResponse)
async def login_user(credentials: UserLogin, db: Session = Depends(get_db)):
    """
    🔐 User Login (Legacy)
    
    Authenticate user with username and password.
    
    Request JSON:
    {
        "username": "john_doe",
        "password": "secure_password"
    }
    
    Returns: User profile if credentials match
    """
    user = UserService.get_user_by_username(db, credentials.username)
    
    if not user or user.password_hash != credentials.password:  # In production: proper hashing
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid credentials"
        )
    
    return user


@app.get("/users/{user_id}", response_model=UserResponse)
async def get_user(user_id: int, db: Session = Depends(get_db)):
    """
    👤 Get User Profile
    
    Retrieve user information by ID.
    """
    user = UserService.get_user(db, user_id)
    
    if not user:
        raise HTTPException(status_code=404, detail="User not found")
    
    return user


@app.get("/users/me")
async def get_current_user_profile(current_user = Depends(get_current_user)):
    """
    👤 Get Current User Profile
    
    Retrieve the authenticated user's profile information.
    
    Headers:
    ```
    Authorization: Bearer <token>
    ```
    """
    user = current_user["user"]
    user_data = UserResponse(
        id=user.id,
        username=user.username,
        email=user.email,
        name=user.name,
        role=UserRoleEnum(user.role.value),
        created_at=user.created_at,
        is_active=user.is_active,
        level=getattr(user, 'level', None),
        total_score=getattr(user, 'total_score', 0),
        total_games_played=getattr(user, 'total_games_played', 0),
        college=getattr(user, 'college', None),
        department=getattr(user, 'department', None),
        year=getattr(user, 'year', None),
        experience=getattr(user, 'experience', None),
        subjects=getattr(user, 'subjects', None)
    )
    return user_data


@app.get("/users/{user_id}/stats", response_model=UserStats)
async def get_user_stats(user_id: int, db: Session = Depends(get_db)):
    """
    📊 Get User Statistics
    
    Get comprehensive statistics about a user's performance.
    
    Returns:
    {
        "user_id": 1,
        "total_score": 850,
        "total_games": 42,
        "average_accuracy": 78.5,
        "concepts_mastered": 12,
        "current_level": "intermediate"
    }
    """
    stats = UserService.get_user_stats(db, user_id)
    
    if not stats:
        raise HTTPException(status_code=404, detail="User not found")
    
    return stats


# ============================================
# GAME ENDPOINTS
# ============================================

@app.post("/games/generate", response_model=GameResponse)
async def generate_game(request: GameRequest, db: Session = Depends(get_db)):
    """
    🎮 Generate Game
    
    Generate a new game of specified type for a concept.
    
    Request JSON:
    {
        "concept": "Photosynthesis",
        "game_type": "quiz",
        "difficulty": "intermediate",
        "num_questions": 5,
        "user_id": 1
    }
    
    Game Types:
    - "quiz": Multiple choice quiz
    - "puzzle": Logic puzzle
    - "speed": Timed rapid-fire challenges
    
    Returns: Complete game with objectives, format, instructions, and data
    """
    try:
        game_data = GameService.generate_or_get_cached_game(
            db,
            request.concept,
            request.game_type,
            request.difficulty,
            request.num_questions
        )
        
        return GameResponse(
            id=f"{request.concept}_{request.game_type}_{datetime.utcnow().timestamp()}",
            concept=request.concept,
            game_type=request.game_type,
            difficulty=request.difficulty,
            objectives=game_data.get("learning_objectives", []),
            format=game_data.get("format", {}),
            instructions=game_data.get("instructions", {}),
            game_data=game_data,
            estimated_duration=game_data.get("estimated_duration", 300),
            created_at=datetime.utcnow()
        )
    except Exception as e:
        logger.error(f"Game generation error: {str(e)}")
        raise HTTPException(status_code=500, detail=f"Failed to generate game: {str(e)}")


@app.post("/games/quiz/submit", response_model=GameResultResponse)
async def submit_quiz(submission: QuizSubmission, db: Session = Depends(get_db)):
    """
    ✅ Submit Quiz Answers
    
    Submit answers for a quiz game and get scoring.
    
    Request JSON:
    {
        "game_id": "game_123",
        "user_id": 1,
        "concept": "Photosynthesis",
        "answers": [0, 1, 2, 0, 1],
        "time_spent": 180
    }
    
    Returns: Game result with score and accuracy
    """
    try:
        # Get game from cache/DB to validate and score
        # This is simplified - in production use the saved game data
        score = len(submission.answers) * 10  # Placeholder scoring
        max_score = len(submission.answers) * 10
        accuracy = (score / max_score) * 100
        
        result = GameService.save_game_result(
            db,
            submission.user_id,
            submission.concept,
            "quiz",
            score,
            max_score,
            accuracy,
            submission.time_spent,
            "intermediate",
            {"answers": submission.answers}
        )
        
        return result
    except Exception as e:
        logger.error(f"Quiz submission error: {str(e)}")
        raise HTTPException(status_code=500, detail=str(e))


@app.post("/games/puzzle/submit", response_model=GameResultResponse)
async def submit_puzzle(submission: PuzzleSubmission, db: Session = Depends(get_db)):
    """
    ✅ Submit Puzzle Solution
    
    Submit solution to a puzzle game.
    """
    try:
        # Simplified scoring
        score = 50 if submission.answer else 0
        max_score = 50
        accuracy = (score / max_score) * 100
        
        result = GameService.save_game_result(
            db,
            submission.user_id,
            submission.concept,
            "puzzle",
            score,
            max_score,
            accuracy,
            submission.time_spent,
            "intermediate",
            {"answer": submission.answer, "hints_used": submission.hints_used}
        )
        
        return result
    except Exception as e:
        logger.error(f"Puzzle submission error: {str(e)}")
        raise HTTPException(status_code=500, detail=str(e))


@app.post("/games/speed/submit", response_model=GameResultResponse)
async def submit_speed_challenge(submission: SpeedSubmission, db: Session = Depends(get_db)):
    """
    ✅ Submit Speed Challenge Answers
    
    Submit answers for speed challenge game.
    """
    try:
        # Simplified scoring
        score = len(submission.answers) * 5
        max_score = len(submission.answers) * 5
        accuracy = (score / max_score) * 100
        
        result = GameService.save_game_result(
            db,
            submission.user_id,
            submission.concept,
            "speed",
            score,
            max_score,
            accuracy,
            submission.time_spent,
            "intermediate",
            {"answers": submission.answers}
        )
        
        return result
    except Exception as e:
        logger.error(f"Speed submission error: {str(e)}")
        raise HTTPException(status_code=500, detail=str(e))


@app.get("/games/results/{user_id}")
async def get_game_results(user_id: int, limit: int = 10, db: Session = Depends(get_db)):
    """
    📋 Get Game Results
    
    Get user's recent game results.
    
    Query params:
    - limit: Number of results to return (default: 10)
    
    Returns: List of game results with scores and accuracy
    """
    results = GameService.get_game_results(db, user_id, limit=limit)
    
    return [
        {
            "id": r.id,
            "game_type": r.game_type,
            "concept": r.concept.name,
            "score": r.score,
            "max_score": r.max_score,
            "accuracy": r.accuracy,
            "time_spent": r.time_spent,
            "created_at": r.created_at
        }
        for r in results
    ]


# ============================================
# PROGRESS ENDPOINTS
# ============================================

@app.get("/progress/{user_id}", response_model=UserProgressResponse)
async def get_user_progress(user_id: int, db: Session = Depends(get_db)):
    """
    📈 Get User Progress
    
    Get comprehensive progress data for a user across all concepts.
    
    Returns:
    {
        "user_id": 1,
        "total_concepts": 25,
        "concepts_mastered": 8,
        "average_accuracy": 76.5,
        "total_games_played": 42,
        "total_time_spent": 3600,
        "progress_by_subject": {
            "Math": [...],
            "Science": [...]
        }
    }
    """
    progress_data = ProgressService.get_user_progress(db, user_id)
    
    if not progress_data:
        raise HTTPException(status_code=404, detail="No progress data found")
    
    return progress_data


@app.get("/progress/{user_id}/concept/{concept_name}")
async def get_concept_progress(user_id: int, concept_name: str, db: Session = Depends(get_db)):
    """
    📊 Get Progress for Specific Concept
    
    Get progress details for one concept.
    """
    concept = ConceptService.get_concept_by_name(db, concept_name)
    
    if not concept:
        raise HTTPException(status_code=404, detail="Concept not found")
    
    progress = db.query(db.query(Progress).filter(
        Progress.user_id == user_id,
        Progress.concept_id == concept.id
    ).first())
    
    if not progress:
        raise HTTPException(status_code=404, detail="No progress data")
    
    return {
        "concept": concept_name,
        "times_attempted": progress.times_attempted,
        "best_score": progress.best_score,
        "average_score": progress.average_score,
        "average_accuracy": progress.average_accuracy,
        "mastery_level": progress.mastery_level
    }


# ============================================
# DASHBOARD ENDPOINTS
# ============================================

@app.get("/dashboard/{user_id}")
async def get_dashboard(user_id: int, db: Session = Depends(get_db)):
    """
    🎯 Get Dashboard
    
    Get complete dashboard data including:
    - User profile
    - Statistics
    - Recent results
    - Progress overview
    - Recommended next concepts
    - Learning paths
    
    Perfect for initializing the frontend dashboard view.
    """
    try:
        dashboard_data = DashboardService.get_dashboard_data(db, user_id)
        
        if not dashboard_data:
            raise HTTPException(status_code=404, detail="User not found")
        
        return dashboard_data
    except Exception as e:
        logger.error(f"Dashboard error: {str(e)}")
        raise HTTPException(status_code=500, detail=str(e))


# ============================================
# RECOMMENDATION ENDPOINTS
# ============================================

@app.get("/recommendations/{user_id}")
async def get_recommendations(user_id: int, db: Session = Depends(get_db)):
    """
    🎓 Get Recommended Concepts
    
    Get AI-recommended next concepts based on user's progress.
    
    Returns:
    [
        {"id": 1, "name": "Advanced Photosynthesis", "difficulty": "advanced"},
        {"id": 5, "name": "Cellular Respiration", "difficulty": "intermediate"},
        ...
    ]
    """
    recommended = ProgressService.get_recommended_next_concepts(db, user_id)
    
    return [
        {
            "id": c.id,
            "name": c.name,
            "subject": c.subject,
            "difficulty": c.difficulty
        }
        for c in recommended
    ]


# ============================================
# LEARNING PATH ENDPOINTS
# ============================================

@app.post("/learning-paths", response_model=LearningPathResponse)
async def create_learning_path(
    path: LearningPathCreate,
    user_id: int,
    db: Session = Depends(get_db)
):
    """
    📚 Create Learning Path
    
    Create a custom learning path for a user.
    
    Request JSON:
    {
        "custom_name": "Physics 101",
        "concepts": [1, 2, 3, 5, 8]
    }
    
    Query param: user_id
    
    Returns: Created learning path with progress tracking
    """
    try:
        learning_path = LearningPathService.create_learning_path(
            db, user_id, path.custom_name, path.concepts
        )
        return learning_path
    except Exception as e:
        logger.error(f"Learning path creation error: {str(e)}")
        raise HTTPException(status_code=500, detail=str(e))


@app.get("/learning-paths/{user_id}")
async def get_learning_paths(user_id: int, db: Session = Depends(get_db)):
    """
    📚 Get User's Learning Paths
    
    Get all learning paths created for a user.
    """
    from database import LearningPath
    paths = db.query(LearningPath).filter(LearningPath.user_id == user_id).all()
    
    return [
        {
            "id": p.id,
            "custom_name": p.custom_name,
            "concepts": p.concepts,
            "progress": f"{p.completed_concepts}/{p.total_concepts}",
            "is_completed": p.is_completed
        }
        for p in paths
    ]


# ============================================
# CONCEPT ENDPOINTS
# ============================================

@app.get("/concepts")
async def get_all_concepts(subject: str = None, db: Session = Depends(get_db)):
    """
    📖 Get All Concepts
    
    Get list of all available concepts.
    
    Query params:
    - subject: Filter by subject (optional)
    
    Returns: List of concepts with metadata
    """
    concepts = ConceptService.get_all_concepts(db, subject)
    
    return [
        {
            "id": c.id,
            "name": c.name,
            "subject": c.subject,
            "difficulty": c.difficulty
        }
        for c in concepts
    ]


@app.get("/concepts/{concept_name}")
async def get_concept_detail(concept_name: str, user_id: int = None, db: Session = Depends(get_db)):
    """
    📖 Get Concept Details
    
    Get detailed information about a specific concept.
    
    Query params:
    - user_id: Include user's progress (optional)
    """
    concept = ConceptService.get_concept_by_name(db, concept_name)
    
    if not concept:
        raise HTTPException(status_code=404, detail="Concept not found")
    
    response = {
        "id": concept.id,
        "name": concept.name,
        "subject": concept.subject,
        "description": concept.description,
        "difficulty": concept.difficulty
    }
    
    # Add user progress if user_id provided
    if user_id:
        progress = db.query(Progress).filter(
            Progress.user_id == user_id,
            Progress.concept_id == concept.id
        ).first()
        
        if progress:
            response["user_progress"] = {
                "times_attempted": progress.times_attempted,
                "mastery_level": progress.mastery_level,
                "average_accuracy": progress.average_accuracy
            }
    
    return response


# ============================================
# API INFORMATION
# ============================================

@app.get("/")
async def root():
    """
    📚 API Information
    
    Welcome to the AI Learning Platform Backend!
    
    Available endpoints:
    - /health - System health check
    - /docs - Interactive API documentation (Swagger)
    - /redoc - Alternative API documentation
    
    Quick Start:
    1. Register: POST /users/register
    2. Login: POST /users/login
    3. Create game: POST /games/generate
    4. Submit results: POST /games/quiz/submit
    5. View dashboard: GET /dashboard/{user_id}
    
    ✨ NEW - Game Generation Endpoints:
    1. POST /ai/generate-game - Generate complete game from prompt
    2. GET /ai/game/{scene_id} - Get game information
    3. POST /ai/launch-game/{scene_id} - Launch game engine
    4. GET /ai/games - List all generated games
    """
    return {
        "name": "AI Learning Platform Backend",
        "version": "1.0.0",
        "description": "Integrated backend connecting AI, database, and frontend",
        "documentation": "http://localhost:8000/docs",
        "status": "running",
        "game_generation": {
            "endpoints": [
                "POST /ai/generate-game",
                "GET /ai/game/{scene_id}",
                "POST /ai/launch-game/{scene_id}",
                "GET /ai/games"
            ]
        }
    }


# ============================================
# LAYER 2: GAME GENERATION ENDPOINTS
# ============================================

@app.post("/ai/generate-game", response_model=GameGenerationResponse)
async def generate_game(request: GamePromptRequest):
    """
    🎮 **Generate a Complete Educational Game**
    
    This is the main endpoint that orchestrates the entire workflow:
    1. Receives prompt from frontend/user
    2. Calls AI-Agents to generate scene JSON
    3. Saves scene to GameEngine directory
    4. Returns game info ready for launch
    
    **Parameters:**
    - `prompt` (required): Natural language description
      - Example: "teach photosynthesis for grade 5"
    - `grade` (optional): Override detected grade (1-12)
    - `subject` (optional): Override detected subject
    
    **Response:**
    - `scene_id`: Unique identifier for the generated game
    - `title`: Human-readable game title
    - `scene_file`: Path where scene JSON was saved
    - `entities_count`: Number of game entities
    
    **Example Request:**
    ```json
    {
      "prompt": "space adventure for grade 3 students"
    }
    ```
    
    **Example Response:**
    ```json
    {
      "success": true,
      "scene_id": "space_grade3",
      "title": "Space — Grade 3",
      "subject": "space",
      "grade": "3",
      "scene_file": "/path/to/GameEngine/data/scenes/space_grade3.json",
      "entities_count": 15,
      "message": "✓ Game ready: Space — Grade 3"
    }
    ```
    """
    try:
        logger.info(f"\n📥 Received game generation request: {request.prompt}")
        
        # Call the orchestration service
        result = GameGenerationService.generate_game(
            prompt=request.prompt,
            grade=request.grade,
            subject=request.subject
        )
        
        if result.get("success"):
            logger.info(f"✅ Game generated successfully: {result['scene_id']}")
            return GameGenerationResponse(**result)
        else:
            logger.error(f"❌ Game generation failed: {result.get('error')}")
            return GameGenerationResponse(
                success=False,
                message=result.get("message", "Game generation failed"),
                error=result.get("error")
            )
            
    except Exception as e:
        logger.error(f"❌ Endpoint error: {str(e)}")
        raise HTTPException(
            status_code=500,
            detail=f"Game generation failed: {str(e)}"
        )


@app.get("/ai/game/{scene_id}")
async def get_game_info(scene_id: str):
    """
    Get information about a previously generated game
    
    **Parameters:**
    - `scene_id`: Scene identifier (e.g., "photosynthesis_grade5")
    
    **Returns:** Scene metadata and entity information
    """
    try:
        result = GameGenerationService.get_game_info(scene_id)
        return result
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@app.get("/ai/games")
async def list_games():
    """
    List all previously generated games in the system
    
    **Returns:** Array of game information objects
    """
    try:
        result = GameGenerationService.list_generated_games()
        return result
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@app.post("/ai/launch-game/{scene_id}")
async def launch_game(scene_id: str):
    """
    🚀 Launch the game engine with a generated scene
    
    **Parameters:**
    - `scene_id`: Scene to load
    
    **Returns:** Process info if successful launch
    
    **Note:** Game will open in a new window on the server machine
    """
    try:
        logger.info(f"\n🚀 Launch request for scene: {scene_id}")
        result = GameGenerationService.launch_game(scene_id)
        return result
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


# ============================================


@app.get("/")
async def root():
    return {"message": "AI Learning Platform Backend", "status": "running"}


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=4001)
