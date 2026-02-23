"""
Pydantic Models for API Request/Response Validation
"""

from __future__ import annotations
from pydantic import BaseModel, EmailStr, Field
from typing import List, Optional, Dict, Any
from datetime import datetime
from enum import Enum


# ============================================
# ENUMS
# ============================================

class UserRoleEnum(str, Enum):
    """User role enumeration"""
    STUDENT = "student"
    TEACHER = "teacher"
    ADMIN = "admin"


# ============================================
# USER MODELS
# ============================================

class UserBase(BaseModel):
    """Base user data"""
    username: str
    email: EmailStr
    name: str
    role: UserRoleEnum


class UserCreate(BaseModel):
    """User creation request (legacy - use RegisterRequest)"""
    username: str
    email: EmailStr
    password: str
    level: str = "beginner"


class UserLogin(BaseModel):
    """User login request (legacy - use LoginRequest)"""
    username: str
    password: str


class UserResponse(UserBase):
    """User response data"""
    id: int
    created_at: datetime
    is_active: bool
    
    # Student fields
    level: Optional[str] = None
    total_score: Optional[int] = None
    total_games_played: Optional[int] = None
    college: Optional[str] = None
    department: Optional[str] = None
    year: Optional[str] = None
    
    # Teacher fields
    experience: Optional[str] = None
    subjects: Optional[List[str]] = None
    
    class Config:
        from_attributes = True


class UserStats(BaseModel):
    """User statistics"""
    user_id: int
    total_score: int
    total_games: int
    average_accuracy: float
    concepts_mastered: int
    current_level: str


# ============================================
# AUTH MODELS
# ============================================

class RegisterRequest(BaseModel):
    """User registration request - supports both Student and Teacher"""
    name: str
    email: EmailStr
    username: str
    password: str
    role: UserRoleEnum = UserRoleEnum.STUDENT
    
    # Student fields
    college: Optional[str] = None
    department: Optional[str] = None
    year: Optional[str] = None
    
    # Teacher fields
    experience: Optional[str] = None
    subjects: Optional[List[str]] = None


class LoginRequest(BaseModel):
    """User login request"""
    email: EmailStr
    password: str
    role: UserRoleEnum = UserRoleEnum.STUDENT


class TokenResponse(BaseModel):
    """JWT Token response"""
    access_token: str
    token_type: str = "bearer"
    user: UserResponse


class TokenPayload(BaseModel):
    """JWT Token payload"""
    sub: int  # user_id
    exp: int
    role: UserRoleEnum


# ============================================
# CONCEPT MODELS
# ============================================

class ConceptBase(BaseModel):
    """Base concept data"""
    name: str
    subject: str
    description: str
    difficulty: str = "intermediate"


class ConceptCreate(ConceptBase):
    """Concept creation request"""
    pass


class ConceptResponse(ConceptBase):
    """Concept response data"""
    id: int
    created_at: datetime
    
    class Config:
        from_attributes = True


# ============================================
# GAME REQUEST/RESPONSE MODELS
# ============================================

class GameRequest(BaseModel):
    """Request to generate a game"""
    concept: str
    game_type: str  # quiz, puzzle, speed
    difficulty: str = "intermediate"
    num_questions: int = 5
    user_id: Optional[int] = None


class GameResponse(BaseModel):
    """Complete game response"""
    id: str
    concept: str
    game_type: str
    difficulty: str
    objectives: List[str]
    format: Dict[str, Any]
    instructions: Dict[str, Any]
    game_data: Dict[str, Any]
    estimated_duration: int
    created_at: datetime


class QuizSubmission(BaseModel):
    """Quiz game submission"""
    game_id: str
    user_id: int
    concept: str
    answers: List[int]  # Answer indices
    time_spent: int


class PuzzleSubmission(BaseModel):
    """Puzzle submission"""
    game_id: str
    user_id: int
    concept: str
    answer: str
    time_spent: int
    hints_used: int = 0


class SpeedSubmission(BaseModel):
    """Speed challenge submission"""
    game_id: str
    user_id: int
    concept: str
    answers: List[str]
    time_spent: int
    completed: bool


# ============================================
# GAME RESULT MODELS
# ============================================

class GameResultBase(BaseModel):
    """Base game result data"""
    game_type: str
    score: int
    max_score: int
    accuracy: float
    time_spent: int
    difficulty: str


class GameResultCreate(GameResultBase):
    """Game result creation"""
    user_id: int
    concept_id: int
    answers: Optional[Dict[str, Any]] = None
    game_data: Optional[Dict[str, Any]] = None


class GameResultResponse(GameResultBase):
    """Game result response"""
    id: int
    user_id: int
    concept_id: int
    completed: bool
    created_at: datetime
    
    class Config:
        from_attributes = True


class GameResultsStats(BaseModel):
    """Statistics from game results"""
    concept: str
    game_type: str
    total_attempts: int
    best_score: int
    average_score: float
    average_accuracy: float
    average_time: int


# ============================================
# PROGRESS MODELS
# ============================================

class ProgressResponse(BaseModel):
    """Progress tracking response"""
    concept: str
    times_attempted: int
    best_score: int
    average_score: float
    average_accuracy: float
    total_time_spent: int
    mastery_level: int  # 0-100
    last_attempted: Optional[datetime]


class UserProgressResponse(BaseModel):
    """All progress for a user"""
    user_id: int
    total_concepts: int
    concepts_mastered: int
    average_accuracy: float
    total_games_played: int
    total_time_spent: int
    progress_by_subject: Dict[str, List[ProgressResponse]]


# ============================================
# LEARNING PATH MODELS
# ============================================

class LearningPathCreate(BaseModel):
    """Create a learning path"""
    custom_name: str
    concepts: List[int]  # Concept IDs


class LearningPathResponse(BaseModel):
    """Learning path response"""
    id: int
    custom_name: str
    concepts: List[int]
    total_concepts: int
    completed_concepts: int
    is_completed: bool
    created_at: datetime
    
    class Config:
        from_attributes = True


class LearningPathProgress(BaseModel):
    """Progress on a learning path"""
    path_id: int
    current_concept: int
    concepts_completed: int
    total_concepts: int
    progress_percentage: float
    estimated_time_remaining: int  # minutes


# ============================================
# DASHBOARD MODELS
# ============================================

class DashboardResponse(BaseModel):
    """Complete dashboard data for frontend"""
    user: UserResponse
    stats: UserStats
    recent_results: List[GameResultResponse]
    progress: UserProgressResponse
    learning_paths: List[LearningPathResponse]
    recommended_next_concepts: List[ConceptResponse]


class ConceptDetailResponse(BaseModel):
    """Detailed concept information"""
    concept: ConceptResponse
    summary: Dict[str, Any]
    user_progress: Optional[ProgressResponse]
    recommended_games: List[str]  # Game types recommended
    available_difficulty_levels: List[str]


# ============================================
# BATCH REQUEST MODELS
# ============================================

class BatchGameRequest(BaseModel):
    """Request to generate multiple games at once"""
    concept: str
    game_types: List[str]  # Multiple game types
    difficulty: str = "intermediate"


class BatchGameResponse(BaseModel):
    """Multiple games response"""
    concept: str
    games: Dict[str, GameResponse]
    total_estimated_time: int


# ============================================
# ERROR MODELS
# ============================================

class ErrorResponse(BaseModel):
    """Standard error response"""
    error: str
    detail: str
    code: int
    timestamp: datetime


class ValidationError(BaseModel):
    """Validation error response"""
    field: str
    error: str
    received_value: Any


# ============================================
# PAGINATION MODELS
# ============================================

class PaginationParams(BaseModel):
    """Pagination parameters"""
    skip: int = 0
    limit: int = 10


class PaginatedResponse(BaseModel):
    """Paginated response wrapper"""
    total: int
    skip: int
    limit: int
    items: List[Any]


# ============================================
# ACHIEVEMENT/BADGE MODELS
# ============================================

class Achievement(BaseModel):
    """User achievement"""
    id: str
    name: str
    description: str
    icon: str
    earned_at: Optional[datetime]


class Badge(BaseModel):
    """User badge"""
    id: str
    name: str
    requirement: str
    current_progress: int
    max_progress: int
    earned: bool


# ============================================
# CONCEPT MODELS
# ============================================

class ConceptBase(BaseModel):
    """Base concept data"""
    name: str
    subject: str
    description: str
    difficulty: str = "intermediate"


class ConceptCreate(ConceptBase):
    """Concept creation request"""
    pass


class ConceptResponse(ConceptBase):
    """Concept response data"""
    id: int
    created_at: datetime
    
    class Config:
        from_attributes = True


# ============================================
# GAME REQUEST/RESPONSE MODELS
# ============================================

class GameRequest(BaseModel):
    """Request to generate a game"""
    concept: str
    game_type: str  # quiz, puzzle, speed
    difficulty: str = "intermediate"
    num_questions: int = 5
    user_id: Optional[int] = None


class GameResponse(BaseModel):
    """Complete game response"""
    id: str
    concept: str
    game_type: str
    difficulty: str
    objectives: List[str]
    format: Dict[str, Any]
    instructions: Dict[str, Any]
    game_data: Dict[str, Any]
    estimated_duration: int
    created_at: datetime


class QuizSubmission(BaseModel):
    """Quiz game submission"""
    game_id: str
    user_id: int
    concept: str
    answers: List[int]  # Answer indices
    time_spent: int


class PuzzleSubmission(BaseModel):
    """Puzzle submission"""
    game_id: str
    user_id: int
    concept: str
    answer: str
    time_spent: int
    hints_used: int = 0


class SpeedSubmission(BaseModel):
    """Speed challenge submission"""
    game_id: str
    user_id: int
    concept: str
    answers: List[str]
    time_spent: int
    completed: bool


# ============================================
# GAME RESULT MODELS
# ============================================

class GameResultBase(BaseModel):
    """Base game result data"""
    game_type: str
    score: int
    max_score: int
    accuracy: float
    time_spent: int
    difficulty: str


class GameResultCreate(GameResultBase):
    """Game result creation"""
    user_id: int
    concept_id: int
    answers: Optional[Dict[str, Any]] = None
    game_data: Optional[Dict[str, Any]] = None


class GameResultResponse(GameResultBase):
    """Game result response"""
    id: int
    user_id: int
    concept_id: int
    completed: bool
    created_at: datetime
    
    class Config:
        from_attributes = True


class GameResultsStats(BaseModel):
    """Statistics from game results"""
    concept: str
    game_type: str
    total_attempts: int
    best_score: int
    average_score: float
    average_accuracy: float
    average_time: int


# ============================================
# PROGRESS MODELS
# ============================================

class ProgressResponse(BaseModel):
    """Progress tracking response"""
    concept: str
    times_attempted: int
    best_score: int
    average_score: float
    average_accuracy: float
    total_time_spent: int
    mastery_level: int  # 0-100
    last_attempted: Optional[datetime]


class UserProgressResponse(BaseModel):
    """All progress for a user"""
    user_id: int
    total_concepts: int
    concepts_mastered: int
    average_accuracy: float
    total_games_played: int
    total_time_spent: int
    progress_by_subject: Dict[str, List[ProgressResponse]]


# ============================================
# LEARNING PATH MODELS
# ============================================

class LearningPathCreate(BaseModel):
    """Create a learning path"""
    custom_name: str
    concepts: List[int]  # Concept IDs


class LearningPathResponse(BaseModel):
    """Learning path response"""
    id: int
    custom_name: str
    concepts: List[int]
    total_concepts: int
    completed_concepts: int
    is_completed: bool
    created_at: datetime
    
    class Config:
        from_attributes = True


class LearningPathProgress(BaseModel):
    """Progress on a learning path"""
    path_id: int
    current_concept: int
    concepts_completed: int
    total_concepts: int
    progress_percentage: float
    estimated_time_remaining: int  # minutes


# ============================================
# DASHBOARD MODELS
# ============================================

class DashboardResponse(BaseModel):
    """Complete dashboard data for frontend"""
    user: UserResponse
    stats: UserStats
    recent_results: List[GameResultResponse]
    progress: UserProgressResponse
    learning_paths: List[LearningPathResponse]
    recommended_next_concepts: List[ConceptResponse]


class ConceptDetailResponse(BaseModel):
    """Detailed concept information"""
    concept: ConceptResponse
    summary: Dict[str, Any]
    user_progress: Optional[ProgressResponse]
    recommended_games: List[str]  # Game types recommended
    available_difficulty_levels: List[str]


# ============================================
# BATCH REQUEST MODELS
# ============================================

class BatchGameRequest(BaseModel):
    """Request to generate multiple games at once"""
    concept: str
    game_types: List[str]  # Multiple game types
    difficulty: str = "intermediate"


class BatchGameResponse(BaseModel):
    """Multiple games response"""
    concept: str
    games: Dict[str, GameResponse]
    total_estimated_time: int


# ============================================
# ERROR MODELS
# ============================================

class ErrorResponse(BaseModel):
    """Standard error response"""
    error: str
    detail: str
    code: int
    timestamp: datetime


class ValidationError(BaseModel):
    """Validation error response"""
    field: str
    error: str
    received_value: Any


# ============================================
# PAGINATION MODELS
# ============================================

class PaginationParams(BaseModel):
    """Pagination parameters"""
    skip: int = 0
    limit: int = 10


class PaginatedResponse(BaseModel):
    """Paginated response wrapper"""
    total: int
    skip: int
    limit: int
    items: List[Any]


# ============================================
# ACHIEVEMENT/BADGE MODELS
# ============================================

class Achievement(BaseModel):
    """User achievement"""
    id: str
    name: str
    description: str
    icon: str
    earned_at: Optional[datetime]


class Badge(BaseModel):
    """User badge"""
    id: str
    name: str
    requirement: str
    current_progress: int
    max_progress: int
    earned: bool


# ============================================
# AI GAME GENERATION MODELS
# ============================================

class GamePromptRequest(BaseModel):
    """Request to generate game from text prompt"""
    prompt: str
    grade: Optional[str] = None
    subject: Optional[str] = None


class GameGenerationResponse(BaseModel):
    """Response from AI game generation"""
    success: bool
    scene_id: Optional[str] = None
    title: Optional[str] = None
    subject: Optional[str] = None
    grade: Optional[str] = None
    scene_file: Optional[str] = None
    entities_count: Optional[int] = None
    message: str
    error: Optional[str] = None
