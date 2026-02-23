#!/usr/bin/env python3
"""
Setup and verify the EduEngine AI-to-Game pipeline

This script:
1. Checks Python version
2. Installs required dependencies
3. Verifies environment
4. Runs basic tests
"""

import sys
import subprocess
import os
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
GAME_ENGINE_DIR = SCRIPT_DIR.parent / "GameEngine"

def check_python():
    """Check Python version"""
    print(f"🐍 Python version: {sys.version}")
    if sys.version_info < (3, 8):
        print("❌ ERROR: Python 3.8+ required")
        return False
    print("✅ Python version OK\n")
    return True

def install_dependencies():
    """Install required packages"""
    print("📦 Checking dependencies...")
    
    requirements_file = SCRIPT_DIR / "requirements.txt"
    
    if not requirements_file.exists():
        print("❌ requirements.txt not found")
        return False
    
    try:
        subprocess.check_call(
            [sys.executable, "-m", "pip", "install", "-r", str(requirements_file)],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.STDOUT
        )
        print("✅ Dependencies installed\n")
        return True
    except subprocess.CalledProcessError:
        print("⚠️  Some dependencies may not have installed properly")
        print("   Try running manually:")
        print(f"   pip install -r {requirements_file}")
        return False

def check_openai_key():
    """Check if OpenAI API key is set"""
    print("🔑 Checking OpenAI API key...")
    
    key = os.getenv("OPENAI_API_KEY")
    
    if key:
        hidden_key = key[:7] + "..." + key[-4:]
        print(f"✅ OPENAI_API_KEY is set: {hidden_key}\n")
        return True
    else:
        print("❌ OPENAI_API_KEY environment variable not set")
        print("   Set it with:")
        print("   export OPENAI_API_KEY='sk-...'")
        print("")
        print("   Or create a .env file in the AI-agents folder with:")
        print("   OPENAI_API_KEY=sk-...\n")
        return False

def check_game_engine():
    """Check if game engine binary exists"""
    print("🎮 Checking game engine...")
    
    engine_binary = GAME_ENGINE_DIR / "EduEngine"
    
    if engine_binary.exists():
        print(f"✅ Game engine found: {engine_binary}\n")
        return True
    else:
        print(f"❌ Game engine not found: {engine_binary}")
        print("   Build the game engine first")
        print(f"   cd {GAME_ENGINE_DIR}")
        print("   # Build instructions in GameEngine/README or PROJECT_OVERVIEW.txt\n")
        return False

def check_imports():
    """Check if key modules can be imported"""
    print("📚 Checking imports...")
    
    modules = [
        ("fastapi", "FastAPI"),
        ("pydantic", "Pydantic"),
        ("dotenv", "python-dotenv"),
        ("openai", "OpenAI SDK"),
    ]
    
    all_ok = True
    for module, display_name in modules:
        try:
            __import__(module)
            print(f"  ✅ {display_name}")
        except ImportError:
            print(f"  ❌ {module} - Install with: pip install {module}")
            all_ok = False
    
    print()
    return all_ok

def test_ai_agent():
    """Quick test of AI agent"""
    print("🤖 Testing AI agent...")
    
    try:
        from teacher_ai_agent import TeacherAIAgent, THEME_CONFIG
        
        # Check theme detection
        test_prompt = "photosynthesis for grade 5"
        agent = TeacherAIAgent()
        
        theme = agent.detect_theme(test_prompt)
        grade = agent.detect_grade(test_prompt)
        
        print(f"  Test prompt: '{test_prompt}'")
        print(f"  Detected theme: {theme}")
        print(f"  Detected grade: {grade}")
        
        if theme == "photosynthesis" and grade == "5":
            print("✅ AI agent working correctly\n")
            return True
        else:
            print("⚠️  Theme/grade detection returned unexpected values\n")
            return False
    
    except Exception as e:
        print(f"❌ Error testing AI agent: {e}\n")
        return False

def main():
    print(f"\n{'='*70}")
    print("🚀 EduEngine AI-to-Game Pipeline Setup")
    print(f"{'='*70}\n")
    
    all_ok = True
    
    # 1. Check Python
    if not check_python():
        return False
    
    # 2. Install dependencies
    if not install_dependencies():
        all_ok = False
    
    # 3. Check imports
    if not check_imports():
        all_ok = False
    
    # 4. Check OpenAI key
    openai_ok = check_openai_key()
    if not openai_ok:
        all_ok = False
    
    # 5. Check game engine
    engine_ok = check_game_engine()
    
    # 6. Test AI agent (only if imports are OK)
    if all_ok:
        test_ai_agent()
    
    # Summary
    print(f"{'='*70}")
    if all_ok and engine_ok:
        print("✅ SETUP COMPLETE - Ready to use!")
        print(f"{'='*70}\n")
        print("Next steps:")
        print("  1. Run tests:")
        print("     python3 test_pipeline.py")
        print("")
        print("  2. Generate and play a game:")
        print("     python3 run_game.py \"teach photosynthesis for grade 5\"")
        print("")
        return True
    elif all_ok and not engine_ok:
        print("✅ SETUP MOSTLY COMPLETE")
        print("⚠️  Game engine needs to be compiled")
        print(f"{'='*70}\n")
        print("You can still test the AI agent:")
        print("  python3 test_pipeline.py --verbose")
        return True
    else:
        print("❌ SETUP INCOMPLETE")
        print(f"{'='*70}\n")
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
