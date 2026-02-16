"""
Teacher AI Agent - Python Package
AI-Powered Teacher Assistant that generates interactive mini-games for learning

Version: 1.0.0
Author: AI-Powered Adaptive Gamified Learning Platform Team
"""

__version__ = "1.0.0"
__author__ = "Learning Platform Team"
__description__ = "AI-Powered agent that generates interactive educational mini-games"

# Main exports
from teacher_ai_agent import TeacherAIAgent, GameType
from client import TeacherAIClient

__all__ = [
    'TeacherAIAgent',
    'TeacherAIClient',
    'GameType',
]
