"""
Database Configuration and Models
Handles all data persistence for users, games, and progress tracking
"""

from sqlalchemy import create_engine, Column, Integer, String, Float, DateTime, Boolean, JSON, ForeignKey, Text
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, relationship
from datetime import datetime
import os

# Database setup (uses SQLite for simplicity, easily switchable to PostgreSQL)
DATABASE_URL = os.getenv("DATABASE_URL", "sqlite:///./game_platform.db")

# Create engine
if DATABASE_URL.startswith("sqlite"):
    engine = create_engine(
        DATABASE_URL,
        connect_args={"check_same_thread": False} if "sqlite" in DATABASE_URL else {}
    )
else:
    engine = create_engine(DATABASE_URL)

SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

# ============================================
# DATABASE MODELS
# ============================================

class User(Base):
    """Student user model"""
    __tablename__ = "users"
    
    id = Column(Integer, primary_key=True, index=True)
    username = Column(String, unique=True, index=True)
    email = Column(String, unique=True, index=True)
    password_hash = Column(String)  # In production, use proper hashing
    created_at = Column(DateTime, default=datetime.utcnow)
    last_login = Column(DateTime)
    is_active = Column(Boolean, default=True)
    level = Column(String, default="beginner")  # beginner, intermediate, advanced
    total_score = Column(Integer, default=0)
    total_games_played = Column(Integer, default=0)
    
    # Relationships
    game_results = relationship("GameResult", back_populates="user", cascade="all, delete-orphan")
    progress = relationship("Progress", back_populates="user", cascade="all, delete-orphan")
    
    def __repr__(self):
        return f"<User {self.username}>"


class Concept(Base):
    """Learning concept model"""
    __tablename__ = "concepts"
    
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, index=True)
    subject = Column(String, index=True)  # Math, Science, Language, etc.
    description = Column(Text)
    difficulty = Column(String, default="intermediate")
    created_at = Column(DateTime, default=datetime.utcnow)
    
    # Relationships
    game_results = relationship("GameResult", back_populates="concept", cascade="all, delete-orphan")
    progress = relationship("Progress", back_populates="concept", cascade="all, delete-orphan")
    
    def __repr__(self):
        return f"<Concept {self.name}>"


class GameResult(Base):
    """Result of a game played"""
    __tablename__ = "game_results"
    
    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(Integer, ForeignKey("users.id"))
    concept_id = Column(Integer, ForeignKey("concepts.id"))
    game_type = Column(String)  # quiz, puzzle, speed
    score = Column(Integer)
    max_score = Column(Integer)
    accuracy = Column(Float)  # 0-100
    time_spent = Column(Integer)  # seconds
    difficulty = Column(String)
    completed = Column(Boolean, default=True)
    created_at = Column(DateTime, default=datetime.utcnow)
    game_data = Column(JSON)  # Store the game structure for replay
    answers = Column(JSON)  # Store user's answers
    
    # Relationships
    user = relationship("User", back_populates="game_results")
    concept = relationship("Concept", back_populates="game_results")
    
    def __repr__(self):
        return f"<GameResult user_id={self.user_id} concept={self.concept_id}>"


class Progress(Base):
    """Track student progress on concepts"""
    __tablename__ = "progress"
    
    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(Integer, ForeignKey("users.id"))
    concept_id = Column(Integer, ForeignKey("concepts.id"))
    times_attempted = Column(Integer, default=0)
    best_score = Column(Integer, default=0)
    average_score = Column(Float, default=0)
    average_accuracy = Column(Float, default=0)
    total_time_spent = Column(Integer, default=0)  # seconds
    mastery_level = Column(Integer, default=0)  # 0-100, indicates how well topic is understood
    last_attempted = Column(DateTime)
    created_at = Column(DateTime, default=datetime.utcnow)
    
    # Relationships
    user = relationship("User", back_populates="progress")
    concept = relationship("Concept", back_populates="progress")
    
    def __repr__(self):
        return f"<Progress user_id={self.user_id} concept_id={self.concept_id}>"


class LearningPath(Base):
    """Recommended learning path for a student"""
    __tablename__ = "learning_paths"
    
    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(Integer, ForeignKey("users.id"), unique=True)
    custom_name = Column(String)  # e.g., "Physics 101", "Math Basics"
    concepts = Column(JSON)  # List of concept IDs in order
    total_concepts = Column(Integer)
    completed_concepts = Column(Integer, default=0)
    is_completed = Column(Boolean, default=False)
    created_at = Column(DateTime, default=datetime.utcnow)
    updated_at = Column(DateTime, default=datetime.utcnow)
    
    def __repr__(self):
        return f"<LearningPath {self.custom_name}>"


class CacheData(Base):
    """Cache frequently generated content"""
    __tablename__ = "cache_data"
    
    id = Column(Integer, primary_key=True, index=True)
    concept = Column(String, index=True)
    game_type = Column(String)  # quiz, puzzle, speed, module
    difficulty = Column(String)
    num_questions = Column(Integer)
    content = Column(JSON)  # The cached game data
    created_at = Column(DateTime, default=datetime.utcnow)
    expires_at = Column(DateTime)  # Cache expiration
    
    def __repr__(self):
        return f"<CacheData {self.concept} ({self.game_type})>"


# Create all tables
def init_db():
    """Initialize database - create all tables"""
    Base.metadata.create_all(bind=engine)
    print("✅ Database initialized successfully!")


def get_db():
    """Get database session for dependency injection"""
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


if __name__ == "__main__":
    init_db()
