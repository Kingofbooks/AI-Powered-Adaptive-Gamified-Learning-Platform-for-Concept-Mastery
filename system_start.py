"""
SYSTEM START GUIDE
Quick commands to get the entire system running
"""

import subprocess
import os
import sys
from pathlib import Path

class SystemStarter:
    """Helper to start the entire AI Learning Platform"""
    
    def __init__(self):
        self.workspace_root = Path(__file__).parent
        self.backend_dir = self.workspace_root / "Backend"
        self.frontend_dir = self.workspace_root / "Frontend"
    
    def print_section(self, title):
        print(f"\n{'='*80}")
        print(f"  {title}")
        print(f"{'='*80}\n")
    
    def print_step(self, number, title, command=None):
        print(f"\n  {number}. {title}")
        if command:
            print(f"     Command: {command}")
    
    def run_system_check(self):
        """Check if system is properly set up"""
        self.print_section("🔍 SYSTEM CHECK")
        
        checks = {
            "Python": self.check_python,
            "Frontend files": self.check_frontend,
            "Backend files": self.check_backend,
            ".env file": self.check_env,
        }
        
        results = {}
        for check_name, check_func in checks.items():
            try:
                result = check_func()
                status = "✅" if result else "❌"
                print(f"  {status} {check_name}")
                results[check_name] = result
            except Exception as e:
                print(f"  ❌ {check_name}: {str(e)}")
                results[check_name] = False
        
        return all(results.values())
    
    def check_python(self):
        return sys.version_info >= (3, 8)
    
    def check_frontend(self):
        return (self.frontend_dir / "src" / "App.jsx").exists()
    
    def check_backend(self):
        return (self.backend_dir / "api.py").exists()
    
    def check_env(self):
        env_file = self.backend_dir / ".env"
        if not env_file.exists():
            return False
        with open(env_file) as f:
            content = f.read()
            return "OPENAI_API_KEY" in content
    
    def setup_backend(self):
        """Setup backend - install dependencies and init database"""
        self.print_section("🚀 SETTING UP BACKEND")
        
        self.print_step(1, "Installing backend dependencies", 
                       "pip install -r Backend/requirements.txt")
        
        self.print_step(2, "Initialize database",
                       'python -c "from Backend.database import init_db; init_db()"')
        
        print("\n  ✓ To run these commands:")
        print("    cd Backend")
        print("    pip install -r requirements.txt")
        print("    python -c \"from database import init_db; init_db()\"")
    
    def setup_frontend(self):
        """Setup frontend - install dependencies"""
        self.print_section("⚛️ SETTING UP FRONTEND")
        
        self.print_step(1, "Installing frontend dependencies",
                       "npm install")
        
        print("\n  ✓ To run these commands:")
        print("    cd Frontend")
        print("    npm install")
    
    def start_backend(self):
        """Start backend server"""
        self.print_section("🔧 STARTING BACKEND")
        
        self.print_step(1, "Start FastAPI server",
                       "python -m uvicorn Backend.api:app --reload --port 8000")
        
        print("\n  Expected output:")
        print("    INFO:     Uvicorn running on http://127.0.0.1:8000")
        print("    INFO:     Application startup complete")
        print("\n  Access API docs: http://localhost:8000/docs")
    
    def start_frontend(self):
        """Start frontend dev server"""
        self.print_section("⚛️ STARTING FRONTEND")
        
        self.print_step(1, "Start Vite dev server",
                       "npm run dev")
        
        print("\n  Expected output:")
        print("    VITE v... ready in ... ms")
        print("    ➜  Local:   http://localhost:5173/")
    
    def print_full_guide(self):
        """Print complete setup and start guide"""
        self.print_section("📚 COMPLETE INTEGRATION GUIDE")
        
        print("""
With the new integrated backend, your complete system works like this:

ARCHITECTURE
════════════════════════════════════════════════════════════════════════════

    FRONTEND (React)
    http://localhost:5173
           ↓
    [HTTP REST API]
           ↓
    BACKEND (api.py)
    http://localhost:8000
           ↓
    ┌─────────────────────┬──────────┬──────────┐
    │ Business Logic      │ AI Engine│ Database │
    │ (services.py)       │ (Claude) │ (SQLite) │
    └─────────────────────┴──────────┴──────────┘


SETUP CHECKLIST
════════════════════════════════════════════════════════════════════════════

Before running, ensure:
  ☐ Python 3.8+ installed
  ☐ Node.js and npm installed
  ☐ Backend/.env has OPENAI_API_KEY
  ☐ Backend/requirements.txt installed
  ☐ Frontend/node_modules exists (npm install)


TERMINAL 1: START BACKEND
════════════════════════════════════════════════════════════════════════════

  cd Backend
  pip install -r requirements.txt
  python -c "from database import init_db; init_db()"
  python -m uvicorn api:app --reload --port 8000

  Expected: ✓ http://localhost:8000/docs


TERMINAL 2: START FRONTEND
════════════════════════════════════════════════════════════════════════════

  cd Frontend
  npm install
  npm run dev

  Expected: ✓ http://localhost:5173


TEST THE CONNECTION
════════════════════════════════════════════════════════════════════════════

1. Open browser: http://localhost:5173
2. You should see the React app
3. Open API docs: http://localhost:8000/docs
4. Try registering a user
5. Generate a game
6. View the dashboard


FILE STRUCTURE
════════════════════════════════════════════════════════════════════════════

Backend/
├── api.py ........................ Main API server (NEW)
├── database.py ................... Database models (NEW)
├── schemas.py .................... Data validation (NEW)
├── services.py ................... Business logic (NEW)
├── teacher_ai_agent.py ........... AI engine
├── requirements.txt .............. Dependencies (UPDATED)
└── game_platform.db .............. Database (auto-created)

Frontend/
├── src/api/
│   ├── BackendClient.js .......... API client (UPDATED)
│   └── ... other files
└── src/components/
    ├── AIIntegration.jsx ......... React components
    └── ... other components


API ENDPOINTS (22 total)
════════════════════════════════════════════════════════════════════════════

Users
  POST   /users/register              Create account
  POST   /users/login                 Login
  GET    /users/{id}                  Get profile
  GET    /users/{id}/stats            Get stats

Games
  POST   /games/generate              Generate game
  POST   /games/quiz/submit           Submit quiz
  POST   /games/puzzle/submit         Submit puzzle
  POST   /games/speed/submit          Submit speed
  GET    /games/results/{id}          Get results

Progress
  GET    /progress/{id}               Get progress
  GET    /progress/{id}/concept/{name}Get concept progress

Dashboard & More
  GET    /dashboard/{id}              Get dashboard
  GET    /recommendations/{id}        Get recommendations
  GET    /concepts                    List concepts
  GET    /concepts/{name}             Get concept
  POST   /learning-paths              Create path
  GET    /learning-paths/{id}         Get paths


EXAMPLE WORKFLOW
════════════════════════════════════════════════════════════════════════════

// In your React component:

import { BackendClient } from './api/BackendClient'
const api = new BackendClient("http://localhost:8000")

// 1. Register user
const user = await api.registerUser(
  "john_doe",
  "john@example.com",
  "password123"
)
console.log("User registered:", user.id)

// 2. Generate game
const game = await api.generateGame(
  "Photosynthesis",
  "quiz",
  "intermediate",
  5
)
console.log("Game objectives:", game.objectives)

// 3. Display game to student
// ... student plays game ...

// 4. Submit results
const result = await api.submitQuiz(
  game.id,
  "Photosynthesis",
  [0, 1, 2, 0, 1],  // user's answers
  180               // time spent in seconds
)
console.log("Score:", result.score, "Accuracy:", result.accuracy)

// 5. Get dashboard
const dashboard = await api.getDashboard(user.id)
console.log("Dashboard data:", dashboard)


PRODUCTION DEPLOYMENT
════════════════════════════════════════════════════════════════════════════

Backend (Heroku):
  1. Add Procfile: web: gunicorn Backend.api:app
  2. git push heroku main
  3. set OPENAI_API_KEY in config vars
  4. Database auto-migrates

Frontend (Vercel):
  1. Connect GitHub repo
  2. Set backend URL in environment
  3. npm run build
  4. Deploy


TROUBLESHOOTING
════════════════════════════════════════════════════════════════════════════

❌ "Cannot find module 'openai'"
   → pip install -r Backend/requirements.txt

❌ "CORS error from frontend"
   → CORS is enabled, check browser console for details

❌ "Database is locked"
   → Close other connections, or switch to PostgreSQL

❌ "API returns 500"
   → Check Backend logs, verify OPENAI_API_KEY

❌ "Games take too long"
   → Normal! Can take 5-10 seconds. Show loading spinner.


DOCUMENTATION FILES
════════════════════════════════════════════════════════════════════════════

Backend:
  • Backend/BACKEND_GUIDE.py     Full backend documentation
  • Backend/api.py               Endpoint docstrings
  • Backend/services.py          Business logic comments

Frontend:
  • Frontend/src/api/BackendClient.js    JavaScript SDK docs
  • AI-agents/examples.py                API usage examples

System:
  • BACKEND_SUMMARY.py           This file
  • RESOURCES.py                 Complete resource index


NEXT STEPS
════════════════════════════════════════════════════════════════════════════

Short term (today):
  ✓ Setup both backend and frontend
  ✓ Verify they can communicate
  ✓ Test creating a user and game

Medium term (this week):
  ○ Implement user authentication UI
  ○ Build game display components
  ○ Connect dashboard to backend API

Long term (this month):
  ○ Deploy to production
  ○ Add more features
  ○ Optimize performance
        """)
    
    def main_menu(self):
        """Interactive menu"""
        while True:
            self.print_section("🎓 AI LEARNING PLATFORM - SETUP MENU")
            
            print("""
    1. Check System Setup
    2. Install Backend Dependencies
    3. Install Frontend Dependencies
    4. Show Backend Start Command
    5. Show Frontend Start Command
    6. Show Complete Setup Guide
    7. Exit
            """)
            
            choice = input("  Select option (1-7): ").strip()
            
            if choice == "1":
                self.run_system_check()
                input("\n  Press Enter to continue...")
            elif choice == "2":
                self.setup_backend()
                input("\n  Press Enter to continue...")
            elif choice == "3":
                self.setup_frontend()
                input("\n  Press Enter to continue...")
            elif choice == "4":
                self.start_backend()
                input("\n  Press Enter to continue...")
            elif choice == "5":
                self.start_frontend()
                input("\n  Press Enter to continue...")
            elif choice == "6":
                self.print_full_guide()
                input("\n  Press Enter to continue...")
            elif choice == "7":
                print("\n✅ Goodbye!\n")
                break
            else:
                print("\n❌ Invalid option, try again")


if __name__ == "__main__":
    starter = SystemStarter()
    
    # Check if running with command line args
    if len(sys.argv) > 1:
        if sys.argv[1] == "check":
            starter.run_system_check()
        elif sys.argv[1] == "setup-backend":
            starter.setup_backend()
        elif sys.argv[1] == "setup-frontend":
            starter.setup_frontend()
        elif sys.argv[1] == "start-backend":
            starter.start_backend()
        elif sys.argv[1] == "start-frontend":
            starter.start_frontend()
        elif sys.argv[1] == "guide":
            starter.print_full_guide()
        else:
            print("Usage: python system_start.py [check|setup-backend|setup-frontend|start-backend|start-frontend|guide]")
    else:
        # Interactive menu
        starter.main_menu()
