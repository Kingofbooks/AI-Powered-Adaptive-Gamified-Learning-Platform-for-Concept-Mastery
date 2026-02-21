"""
Business Logic Services
Handles coordination between database, AI engine, and API
"""

import json
from datetime import datetime, timedelta
from typing import List, Optional, Dict, Any, Tuple
from sqlalchemy.orm import Session
from teacher_ai_agent import TeacherAIAgent
from database import (
    User, Concept, GameResult, Progress, LearningPath, CacheData, UserRole
)
from auth import hash_password, verify_password, create_access_token
from schemas import UserRoleEnum
import logging
import os
from dotenv import load_dotenv

# Load environment variables
load_dotenv()

logger = logging.getLogger(__name__)

# Global cache for AI agent (lazy initialization)
_ai_agent = None

def get_ai_agent():
    """Lazily initialize and return the AI agent"""
    global _ai_agent
    if _ai_agent is None:
        api_key = os.getenv("OPENAI_API_KEY")
        if not api_key:
            logger.warning("OPENAI_API_KEY not set - AI game generation will fail")
        _ai_agent = TeacherAIAgent(api_key=api_key)
    return _ai_agent


# ============================================
# USER SERVICES
# ============================================

class UserService:
    """Handle user-related operations"""
    
    @staticmethod
    def register_user(
        db: Session,
        name: str,
        email: str,
        username: str,
        password: str,
        role: UserRoleEnum,
        **kwargs
    ) -> Tuple[User, str]:
        """
        Register a new user (Student or Teacher)
        
        Args:
            db: Database session
            name: Full name
            email: Email address
            username: Username
            password: Plain password (will be hashed)
            role: User role (student, teacher, admin)
            **kwargs: Additional fields (college, department, year, experience, subjects)
        
        Returns:
            Tuple of (User object, success message)
        """
        
        # Check if user already exists
        existing_user = db.query(User).filter(
            (User.email == email) | (User.username == username)
        ).first()
        
        if existing_user:
            raise ValueError("Email or username already registered")
        
        # Hash password
        password_hash = hash_password(password)
        
        # Create user
        db_user = User(
            name=name,
            email=email,
            username=username,
            password_hash=password_hash,
            role=UserRole[role.name],  # Convert to UserRole enum using name
            is_active=True
        )
        
        # Add role-specific fields
        if role == UserRoleEnum.STUDENT:
            db_user.college = kwargs.get('college')
            db_user.department = kwargs.get('department')
            db_user.year = kwargs.get('year')
            db_user.level = "beginner"
        elif role == UserRoleEnum.TEACHER:
            db_user.experience = kwargs.get('experience')
            db_user.subjects = kwargs.get('subjects', [])
        
        db.add(db_user)
        db.commit()
        db.refresh(db_user)
        
        logger.info(f"Registered new {role.value}: {username}")
        return db_user, f"User {username} created successfully"
    
    @staticmethod
    def authenticate_user(
        db: Session,
        email: str,
        password: str,
        role: UserRoleEnum
    ) -> Optional[User]:
        """
        Authenticate a user by email and password
        
        Args:
            db: Database session
            email: Email address
            password: Plain password to verify
            role: Expected user role
        
        Returns:
            User object if authentication successful, None otherwise
        """
        
        user = db.query(User).filter(
            (User.email == email) & (User.role == UserRole[role.name])
        ).first()
        
        if not user or not user.is_active:
            return None
        
        # Verify password
        if not verify_password(password, user.password_hash):
            return None
        
        # Update last login
        user.last_login = datetime.utcnow()
        db.commit()
        
        logger.info(f"User authenticated: {user.username}")
        return user
    
    @staticmethod
    def create_user(db: Session, username: str, email: str, password: str, level: str = "beginner") -> User:
        """Create a new user (for backward compatibility)"""
        password_hash = hash_password(password)
        db_user = User(
            username=username,
            email=email,
            name=username,
            password_hash=password_hash,
            level=level,
            role=UserRole.STUDENT
        )
        db.add(db_user)
        db.commit()
        db.refresh(db_user)
        logger.info(f"Created user: {username}")
        return db_user
    
    @staticmethod
    def get_user(db: Session, user_id: int) -> Optional[User]:
        """Get user by ID"""
        return db.query(User).filter(User.id == user_id).first()
    
    @staticmethod
    def get_user_by_username(db: Session, username: str) -> Optional[User]:
        """Get user by username"""
        return db.query(User).filter(User.username == username).first()
    
    @staticmethod
    def get_user_by_email(db: Session, email: str) -> Optional[User]:
        """Get user by email"""
        return db.query(User).filter(User.email == email).first()
    
    @staticmethod
    def get_user_stats(db: Session, user_id: int) -> Dict[str, Any]:
        """Get user statistics"""
        user = UserService.get_user(db, user_id)
        if not user:
            return {}
        
        # Calculate stats
        results = db.query(GameResult).filter(GameResult.user_id == user_id).all()
        
        if not results:
            return {
                "user_id": user_id,
                "total_score": 0,
                "total_games": 0,
                "average_accuracy": 0,
                "concepts_mastered": 0,
                "current_level": user.level if user.level else "beginner"
            }
        
        total_score = sum(r.score for r in results)
        avg_accuracy = sum(r.accuracy for r in results) / len(results)
        
        # Count mastered concepts (>80% average)
        progress_records = db.query(Progress).filter(Progress.user_id == user_id).all()
        mastered = sum(1 for p in progress_records if p.average_accuracy >= 80)
        
        return {
            "user_id": user_id,
            "total_score": total_score,
            "total_games": len(results),
            "average_accuracy": round(avg_accuracy, 2),
            "concepts_mastered": mastered,
            "current_level": user.level if user.level else "beginner"
        }


# ============================================
# CONCEPT SERVICES
# ============================================
# CONCEPT SERVICES
# ============================================

class ConceptService:
    """Handle concept-related operations"""
    
    @staticmethod
    def get_or_create_concept(db: Session, name: str, subject: str = "General") -> Concept:
        """Get concept or create if doesn't exist"""
        concept = db.query(Concept).filter(Concept.name == name).first()
        
        if not concept:
            concept = Concept(
                name=name,
                subject=subject,
                description=f"Concept: {name}",
                difficulty="intermediate"
            )
            db.add(concept)
            db.commit()
            db.refresh(concept)
            logger.info(f"Created concept: {name}")
        
        return concept
    
    @staticmethod
    def get_all_concepts(db: Session, subject: Optional[str] = None) -> List[Concept]:
        """Get all concepts, optionally filtered by subject"""
        query = db.query(Concept)
        
        if subject:
            query = query.filter(Concept.subject == subject)
        
        return query.all()
    
    @staticmethod
    def get_concept_by_name(db: Session, name: str) -> Optional[Concept]:
        """Get concept by name"""
        return db.query(Concept).filter(Concept.name == name).first()


# ============================================
# GAME SERVICES
# ============================================

class GameService:
    """Handle game generation and management"""
    
    @staticmethod
    def generate_or_get_cached_game(
        db: Session,
        concept: str,
        game_type: str,
        difficulty: str = "intermediate",
        num_questions: int = 5
    ) -> Dict[str, Any]:
        """Generate game or return from cache"""
        
        # Check cache first
        cache = db.query(CacheData).filter(
            CacheData.concept == concept,
            CacheData.game_type == game_type,
            CacheData.difficulty == difficulty,
            CacheData.num_questions == num_questions,
            CacheData.expires_at > datetime.utcnow()
        ).first()
        
        if cache:
            logger.info(f"Cache hit: {concept} - {game_type}")
            return cache.content
        
        # Generate new game
        try:
            ai_agent = get_ai_agent()
            if game_type == "quiz":
                game_data = ai_agent.generate_quiz_game(
                    concept=concept,
                    num_questions=num_questions
                )
            elif game_type == "puzzle":
                game_data = ai_agent.generate_puzzle_game(concept)
            elif game_type == "speed":
                game_data = ai_agent.generate_speed_game(concept, num_questions)
            else:
                raise ValueError(f"Unknown game type: {game_type}")
            
            # Cache the result
            cache = CacheData(
                concept=concept,
                game_type=game_type,
                difficulty=difficulty,
                num_questions=num_questions,
                content=game_data,
                expires_at=datetime.utcnow() + timedelta(hours=24)
            )
            db.add(cache)
            db.commit()
            
            logger.info(f"Generated and cached: {concept} - {game_type}")
            return game_data
            
        except Exception as e:
            logger.error(f"Error generating game: {str(e)}")
            raise
    
    @staticmethod
    def save_game_result(
        db: Session,
        user_id: int,
        concept: str,
        game_type: str,
        score: int,
        max_score: int,
        accuracy: float,
        time_spent: int,
        difficulty: str,
        answers: Optional[Dict] = None
    ) -> GameResult:
        """Save game result to database"""
        
        # Get or create concept
        concept_obj = ConceptService.get_or_create_concept(db, concept)
        
        # Create game result
        result = GameResult(
            user_id=user_id,
            concept_id=concept_obj.id,
            game_type=game_type,
            score=score,
            max_score=max_score,
            accuracy=accuracy,
            time_spent=time_spent,
            difficulty=difficulty,
            answers=answers,
            completed=True
        )
        
        db.add(result)
        
        # Update user total score
        user = UserService.get_user(db, user_id)
        user.total_score += score
        user.total_games_played += 1
        
        db.commit()
        db.refresh(result)
        
        # Update progress
        ProgressService.update_progress(
            db, user_id, concept_obj.id, score, max_score, accuracy, time_spent
        )
        
        logger.info(f"Saved game result: user={user_id}, concept={concept}, score={score}")
        return result
    
    @staticmethod
    def get_game_results(
        db: Session,
        user_id: int,
        concept: Optional[str] = None,
        game_type: Optional[str] = None,
        limit: int = 10
    ) -> List[GameResult]:
        """Get user's game results with optional filters"""
        query = db.query(GameResult).filter(GameResult.user_id == user_id)
        
        if concept:
            concept_obj = ConceptService.get_concept_by_name(db, concept)
            if concept_obj:
                query = query.filter(GameResult.concept_id == concept_obj.id)
        
        if game_type:
            query = query.filter(GameResult.game_type == game_type)
        
        return query.order_by(GameResult.created_at.desc()).limit(limit).all()


# ============================================
# PROGRESS SERVICES
# ============================================

class ProgressService:
    """Handle user progress tracking"""
    
    @staticmethod
    def update_progress(
        db: Session,
        user_id: int,
        concept_id: int,
        score: int,
        max_score: int,
        accuracy: float,
        time_spent: int
    ) -> Progress:
        """Update or create progress record"""
        
        progress = db.query(Progress).filter(
            Progress.user_id == user_id,
            Progress.concept_id == concept_id
        ).first()
        
        if not progress:
            progress = Progress(user_id=user_id, concept_id=concept_id)
        
        # Update counters
        progress.times_attempted += 1
        progress.last_attempted = datetime.utcnow()
        progress.total_time_spent += time_spent
        
        # Update score tracking
        if score > progress.best_score:
            progress.best_score = score
        
        # Calculate running averages
        prev_total_score = progress.average_score * (progress.times_attempted - 1)
        progress.average_score = (prev_total_score + score) / progress.times_attempted
        
        prev_total_accuracy = progress.average_accuracy * (progress.times_attempted - 1)
        progress.average_accuracy = (prev_total_accuracy + accuracy) / progress.times_attempted
        
        # Update mastery level (simplified: based on accuracy)
        if progress.average_accuracy >= 90:
            progress.mastery_level = 100
        elif progress.average_accuracy >= 80:
            progress.mastery_level = 80
        elif progress.average_accuracy >= 70:
            progress.mastery_level = 60
        elif progress.average_accuracy >= 60:
            progress.mastery_level = 40
        else:
            progress.mastery_level = max(0, progress.mastery_level - 5)
        
        db.add(progress)
        db.commit()
        db.refresh(progress)
        
        logger.info(f"Updated progress: user={user_id}, concept={concept_id}, mastery={progress.mastery_level}")
        return progress
    
    @staticmethod
    def get_user_progress(db: Session, user_id: int) -> Dict[str, Any]:
        """Get all progress records for a user"""
        progress_records = db.query(Progress).filter(Progress.user_id == user_id).all()
        
        # Group by subject
        by_subject = {}
        
        for record in progress_records:
            subject = record.concept.subject
            if subject not in by_subject:
                by_subject[subject] = []
            
            by_subject[subject].append({
                "concept": record.concept.name,
                "times_attempted": record.times_attempted,
                "best_score": record.best_score,
                "average_score": round(record.average_score, 2),
                "average_accuracy": round(record.average_accuracy, 2),
                "total_time_spent": record.total_time_spent,
                "mastery_level": record.mastery_level,
                "last_attempted": record.last_attempted
            })
        
        return by_subject
    
    @staticmethod
    def get_recommended_next_concepts(db: Session, user_id: int, limit: int = 5) -> List[Concept]:
        """Get recommended concepts based on progress"""
        # Get user's progress
        progress = db.query(Progress).filter(Progress.user_id == user_id).all()
        attempted_concept_ids = [p.concept_id for p in progress]
        
        # Find concepts not yet attempted
        unattempted = db.query(Concept).filter(
            ~Concept.id.in_(attempted_concept_ids)
        ).limit(limit).all()
        
        # If attempted all, recommend mastery improvement
        if not unattempted:
            low_mastery = db.query(Progress).filter(
                Progress.user_id == user_id,
                Progress.mastery_level < 80
            ).order_by(Progress.mastery_level).limit(3).all()
            
            return [p.concept for p in low_mastery]
        
        return unattempted


# ============================================
# LEARNING PATH SERVICES
# ============================================

class LearningPathService:
    """Handle learning paths"""
    
    @staticmethod
    def create_learning_path(
        db: Session,
        user_id: int,
        name: str,
        concept_ids: List[int]
    ) -> LearningPath:
        """Create a learning path"""
        path = LearningPath(
            user_id=user_id,
            custom_name=name,
            concepts=concept_ids,
            total_concepts=len(concept_ids)
        )
        db.add(path)
        db.commit()
        db.refresh(path)
        
        logger.info(f"Created learning path: {name} for user {user_id}")
        return path
    
    @staticmethod
    def update_learning_path_progress(
        db: Session,
        path_id: int,
        completed_concept_count: int
    ) -> LearningPath:
        """Update path completion progress"""
        path = db.query(LearningPath).filter(LearningPath.id == path_id).first()
        
        if path:
            path.completed_concepts = completed_concept_count
            path.is_completed = (completed_concept_count >= path.total_concepts)
            path.updated_at = datetime.utcnow()
            
            db.commit()
            db.refresh(path)
        
        return path


# ============================================
# DASHBOARD SERVICES
# ============================================

class DashboardService:
    """Generate complete dashboard data"""
    
    @staticmethod
    def get_dashboard_data(db: Session, user_id: int) -> Dict[str, Any]:
        """Get all dashboard data for a user"""
        
        user = UserService.get_user(db, user_id)
        if not user:
            return {}
        
        stats = UserService.get_user_stats(db, user_id)
        recent_results = GameService.get_game_results(db, user_id, limit=5)
        progress = ProgressService.get_user_progress(db, user_id)
        recommended = ProgressService.get_recommended_next_concepts(db, user_id)
        learning_paths = db.query(LearningPath).filter(LearningPath.user_id == user_id).all()
        
        return {
            "user": user,
            "stats": stats,
            "recent_results": recent_results,
            "progress": progress,
            "learning_paths": learning_paths,
            "recommended_concepts": recommended
        }
