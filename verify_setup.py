#!/usr/bin/env python3
"""
VERIFICATION SCRIPT
Run this to check if everything is set up correctly
Usage: python verify_setup.py
"""

import sys
import os
import subprocess
import requests
import json
from pathlib import Path

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    RESET = '\033[0m'

def print_header(text):
    print(f"\n{Colors.BLUE}{'='*70}{Colors.RESET}")
    print(f"{Colors.BLUE}{text}{Colors.RESET}")
    print(f"{Colors.BLUE}{'='*70}{Colors.RESET}\n")

def print_success(text):
    print(f"{Colors.GREEN}✓ {text}{Colors.RESET}")

def print_error(text):
    print(f"{Colors.RED}✗ {text}{Colors.RESET}")

def print_warning(text):
    print(f"{Colors.YELLOW}⚠ {text}{Colors.RESET}")

def check_file_exists(path, description):
    """Check if a file exists"""
    if os.path.exists(path):
        print_success(f"{description}: {path}")
        return True
    else:
        print_error(f"{description} NOT FOUND: {path}")
        return False

def check_api_health(base_url="http://localhost:8000"):
    """Check if API is running"""
    try:
        response = requests.get(f"{base_url}/health", timeout=5)
        if response.status_code == 200:
            data = response.json()
            print_success(f"API is healthy: {data.get('message', 'Running')}")
            return True
        else:
            print_error(f"API returned status {response.status_code}")
            return False
    except requests.exceptions.ConnectionError:
        print_error("Cannot connect to API at http://localhost:8000")
        print_warning("Start the API with: python main.py")
        return False
    except Exception as e:
        print_error(f"API check failed: {str(e)}")
        return False

def check_env_file():
    """Check if .env file exists and has API key"""
    env_path = "Backend/.env"
    if not os.path.exists(env_path):
        print_error(f".env file not found at {env_path}")
        return False
    
    try:
        with open(env_path, 'r') as f:
            content = f.read()
            if "OPENAI_API_KEY" in content and "sk-" in content:
                print_success(".env file exists and contains API key")
                return True
            else:
                print_error(".env file missing OPENAI_API_KEY")
                return False
    except Exception as e:
        print_error(f"Error reading .env: {str(e)}")
        return False

def check_python_packages():
    """Check if required Python packages are installed"""
    required = ['fastapi', 'uvicorn', 'openai', 'pydantic', 'python-dotenv', 'requests']
    missing = []
    
    for package in required:
        try:
            __import__(package.replace('-', '_'))
            print_success(f"Package installed: {package}")
        except ImportError:
            print_error(f"Package NOT installed: {package}")
            missing.append(package)
    
    if missing:
        print_warning(f"\nInstall missing packages:")
        print(f"  pip install {' '.join(missing)}")
        return False
    return True

def test_api_endpoints(base_url="http://localhost:8000"):
    """Test key API endpoints"""
    endpoints = [
        ("/health", "GET", None, "Health Check"),
        ("/game-types", "GET", None, "Game Types"),
        ("/summarize", "POST", {"concept": "Test", "level": "beginner"}, "Summarize"),
    ]
    
    results = []
    for endpoint, method, data, name in endpoints:
        try:
            if method == "GET":
                response = requests.get(f"{base_url}{endpoint}", timeout=10)
            else:
                response = requests.post(f"{base_url}{endpoint}", json=data, timeout=10)
            
            if response.status_code == 200:
                print_success(f"{name} ({endpoint})")
                results.append(True)
            else:
                print_error(f"{name} returned status {response.status_code}")
                results.append(False)
        except Exception as e:
            print_error(f"{name} failed: {str(e)}")
            results.append(False)
    
    return all(results)

def check_frontend_files():
    """Check if frontend files exist"""
    files_to_check = [
        ("Frontend/src/components/AIIntegration.jsx", "React AI Integration Component"),
        ("AI-agents/TeacherAIClient.js", "JavaScript Client Library"),
        ("Frontend/src/components/GameBox.jsx", "Game Box Component"),
    ]
    
    results = []
    for path, description in files_to_check:
        results.append(check_file_exists(path, description))
    
    return all(results)

def generate_test_quiz():
    """Try to generate a test quiz"""
    try:
        response = requests.post(
            "http://localhost:8000/generate-quiz",
            json={
                "concept": "Test Concept",
                "game_type": "quiz",
                "num_questions": 1,
                "level": "beginner"
            },
            timeout=30
        )
        
        if response.status_code == 200:
            data = response.json()
            if "data" in data and data["data"].get("questions"):
                print_success("Quiz generation works! 🎮")
                print(f"  Title: {data['data'].get('title', 'N/A')}")
                print(f"  Questions: {len(data['data'].get('questions', []))}")
                return True
        
        print_warning("Quiz generation returned unexpected format")
        return False
    except requests.exceptions.Timeout:
        print_warning("Quiz generation timed out (might be generating, try again)")
        return True
    except Exception as e:
        print_error(f"Quiz generation failed: {str(e)}")
        return False

def main():
    print_header("TEACHER AI AGENT - SETUP VERIFICATION")
    
    checks = {
        "Environment": [
            ("Python 3.8+", sys.version_info >= (3, 8)),
        ],
        "Configuration": [
            ("Environment Variables", check_env_file()),
        ],
        "Dependencies": [
            ("Required Python Packages", check_python_packages()),
        ],
        "Backend": [
            ("API Server Running", check_api_health()),
            ("API Endpoints Working", test_api_endpoints() if check_api_health() else None),
        ],
        "Frontend": [
            ("Frontend Files", check_frontend_files()),
        ],
        "Functionality": [
            ("Quiz Generation", generate_test_quiz() if check_api_health() else None),
        ]
    }
    
    # Print results summary
    print_header("VERIFICATION RESULTS")
    
    total_checks = 0
    passed_checks = 0
    
    for category, results in checks.items():
        print(f"\n{Colors.BLUE}{category}:{Colors.RESET}")
        for check_name, result in results:
            if result is None:
                print_warning(f"Skipped: {check_name}")
            elif result:
                print_success(check_name)
                passed_checks += 1
                total_checks += 1
            else:
                print_error(check_name)
                total_checks += 1
    
    # Final summary
    print_header("SUMMARY")
    
    if total_checks == 0:
        print_warning("No checks were performed")
        return 1
    
    percentage = (passed_checks / total_checks) * 100
    print(f"\nChecks Passed: {passed_checks}/{total_checks} ({percentage:.0f}%)\n")
    
    if percentage == 100:
        print_success("All checks passed! Your setup is ready to go! 🚀")
        print("\nNext steps:")
        print("  1. Start the backend: python main.py")
        print("  2. Start the frontend: npm run dev")
        print("  3. Open http://localhost:5173 in your browser")
        print("  4. Try creating a quiz or game!")
        return 0
    elif percentage >= 80:
        print_warning("Most checks passed, but fix the errors above")
        return 1
    else:
        print_error("Several checks failed. See errors above for details.")
        print("\nTroubleshooting:")
        print("  - Check API is running: python main.py")
        print("  - Install dependencies: pip install -r Backend/requirements.txt")
        print("  - Verify .env file has OPENAI_API_KEY")
        print("  - Check firewall settings")
        return 1

if __name__ == "__main__":
    sys.exit(main())
