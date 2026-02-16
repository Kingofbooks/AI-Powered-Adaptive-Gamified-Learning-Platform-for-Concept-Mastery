"""
IMPLEMENTATION CHECKLIST & QUICK START GUIDE

Follow this checklist to integrate the Teacher AI Agent into your app
"""

# ============================================
# BEFORE YOU START
# ============================================

PREREQUISITES = {
    "backend": [
        "✅ Python 3.8+ installed",
        "✅ FastAPI server running (python main.py)",
        "✅ OpenAI API key in .env file",
        "✅ Server running on http://localhost:8000",
        "✅ /docs endpoint shows Swagger UI"
    ],
    "frontend": [
        "✅ React project created (npm create vite@latest -- --template react)",
        "✅ fetch API available (native browser API)",
        "✅ npm or yarn for package management",
        "✅ Your app running on http://localhost (port 5173 or similar)"
    ]
}

# ============================================
# SETUP STEPS
# ============================================

SETUP_CHECKLIST = {
    "Step 1: Backend Setup": [
        "1. Start the FastAPI server",
        "   Command: python main.py",
        "   Expected: Server running on http://localhost:8000",
        "   Verify: Open http://localhost:8000/docs in browser",
        "",
        "2. Test with health endpoint",
        "   GET http://localhost:8000/health",
        "   Expected Response: {\"status\": \"healthy\"}"
    ],
    
    "Step 2: Frontend Setup": [
        "1. Copy TeacherAIClient.js to your project",
        "   Location: src/api/TeacherAIClient.js",
        "",
        "2. Copy AIIntegration.jsx to your project",
        "   Location: src/components/AIIntegration.jsx",
        "",
        "3. Wrap your app with AIProvider in main App component",
        "   See AIIntegration.jsx for example",
    ],
    
    "Step 3: Test Integration": [
        "1. Run your React app",
        "   Command: npm run dev",
        "",
        "2. Use useAI() hook in any component",
        "   const ai = useAI()",
        "   const quiz = await ai.generateQuiz('Photosynthesis')",
        "",
        "3. Check browser console for errors"
    ],
    
    "Step 4: Build Your First Game": [
        "1. Copy QuizGame component from AIIntegration.jsx",
        "   Add to your pages/Concepts.jsx or similar",
        "",
        "2. Test with a simple concept",
        "   <QuizGame concept='Photosynthesis' numQuestions={3} />",
        "",
        "3. Verify questions display and scoring works"
    ]
}

# ============================================
# IMPLEMENTATION PATTERNS
# ============================================

PATTERNS = {
    "Pattern 1: Load and Display Quiz": """
// In your component:
const QuizPage = ({ concept }) => {
    const ai = useAI();
    const [quiz, setQuiz] = React.useState(null);
    const [loading, setLoading] = React.useState(true);
    
    React.useEffect(() => {
        async function load() {
            const data = await ai.generateQuiz(concept, 5, 'intermediate');
            setQuiz(data);
            setLoading(false);
        }
        load();
    }, [concept, ai]);
    
    if (loading) return <div>Loading...</div>;
    if (!quiz) return <div>No quiz</div>;
    
    return (
        <div>
            <h2>{quiz.title}</h2>
            {quiz.questions.map((q, idx) => (
                <div key={idx}>
                    <p>{q.question}</p>
                    {q.options.map((opt, i) => (
                        <button key={i}>{opt}</button>
                    ))}
                </div>
            ))}
        </div>
    );
};
    """,
    
    "Pattern 2: Get Complete Game Details": """
// RECOMMENDED: Use the /game endpoint for best results
const GameDetails = ({ concept, type = 'quiz' }) => {
    const ai = useAI();
    const [game, setGame] = React.useState(null);
    
    React.useEffect(() => {
        async function load() {
            // This returns: objectives, format, instructions, game_data
            const details = await ai.getGameDetails(
                concept,
                type,
                5,           // num_questions
                'intermediate'
            );
            setGame(details);
        }
        load();
    }, [concept, type, ai]);
    
    if (!game) return <div>Loading...</div>;
    
    return (
        <div>
            {/* Display objectives first */}
            <h3>Learning Goals</h3>
            {game.objectives.learning_objectives.map(obj => (
                <p key={obj}>{obj}</p>
            ))}
            
            {/* Then display game using game_data */}
            <h3>Game</h3>
            {/* Implement UI based on game.format.structure */}
            {/* Use game.game_data for actual content */}
            {/* Follow game.instructions for game flow */}
        </div>
    );
};
    """,
    
    "Pattern 3: Handle Scoring": """
// Score answers and track progress
const scoreQuiz = (questions, userAnswers, ai) => {
    let score = 0;
    let streak = 0;
    let correct = 0;
    
    userAnswers.forEach((answer, idx) => {
        const question = questions[idx];
        const isCorrect = answer === question.correct_answer;
        
        if (isCorrect) {
            correct += 1;
            streak += 1;
            // Add points with streak multiplier
            score += ai.scoreAnswer(true, 10, streak);
        } else {
            streak = 0;
        }
    });
    
    return {
        totalScore: score,
        correct: correct,
        total: questions.length,
        accuracy: ai.calculateAccuracy(correct, questions.length)
    };
};
    """,
    
    "Pattern 4: Speed Challenge with Timer": """
// Implement timed challenges
const SpeedChallengeLogic = ({ challenges, totalTime, ai }) => {
    const [timeLeft, setTimeLeft] = React.useState(totalTime);
    const [currentIndex, setCurrentIndex] = React.useState(0);
    const [score, setScore] = React.useState(0);
    const [finished, setFinished] = React.useState(false);
    
    // Timer
    React.useEffect(() => {
        if (finished) return;
        
        const timer = setInterval(() => {
            setTimeLeft(t => {
                if (t <= 1) {
                    setFinished(true);
                    return 0;
                }
                return t - 1;
            });
        }, 1000);
        
        return () => clearInterval(timer);
    }, [finished]);
    
    const handleAnswer = (userAnswer) => {
        const current = challenges[currentIndex];
        
        // Check if correct
        if (String(userAnswer) === String(current.answer)) {
            setScore(score + 10);
        }
        
        // Move to next
        if (currentIndex < challenges.length - 1) {
            setCurrentIndex(currentIndex + 1);
        } else {
            setFinished(true);
        }
    };
    
    return {
        timeLeft,
        formattedTime: ai.formatTime(timeLeft),
        currentChallenge: challenges[currentIndex],
        score,
        isFinished: finished,
        handleAnswer
    };
};
    """,
    
    "Pattern 5: Store Game Results": """
// Save results to your database
const saveGameResults = async (results) => {
    const saved = {
        concept: results.concept,
        gameType: results.gameType,
        score: results.score,
        accuracy: results.accuracy,
        timeSpent: results.timeSpent,
        completed: true,
        timestamp: new Date().toISOString(),
        studentLevel: results.studentLevel
    };
    
    // Save to your backend
    const response = await fetch('/api/results', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(saved)
    });
    
    return response.json();
};
    """
}

# ============================================
# COMMON ISSUES & SOLUTIONS
# ============================================

TROUBLESHOOTING = {
    "API Connection Error": {
        "Problem": "Cannot connect to http://localhost:8000",
        "Solutions": [
            "1. Check if FastAPI server is running: python main.py",
            "2. Verify port 8000 is open",
            "3. Check .env file has valid OPENAI_API_KEY",
            "4. Try http://localhost:8000/health in browser",
            "5. Check firewall settings"
        ]
    },
    
    "CORS Error": {
        "Problem": "Cross-Origin Request Blocked",
        "Solutions": [
            "1. This is normal for http://localhost development",
            "2. CORS middleware is already added to main.py",
            "3. Check browser console for exact error",
            "4. Try on same machine (not remote server)",
            "5. If using remote: check CORS settings in main.py"
        ]
    },
    
    "Quiz Questions Not Loading": {
        "Problem": "Quiz appears but questions are empty",
        "Solutions": [
            "1. Check API response in browser DevTools -> Network",
            "2. Verify questions array is populated",
            "3. Try simpler concept name (like 'Math' not 'Advanced Calculus')",
            "4. Check console for API errors",
            "5. Verify OpenAI API key is valid"
        ]
    },
    
    "Slow Response Time": {
        "Problem": "Games take 30-40 seconds to generate",
        "Solutions": [
            "1. This is NORMAL for /generate-module endpoint",
            "2. Individual endpoints are much faster",
            "3. Show loading spinner to user",
            "4. Use /game endpoint for faster results (9 params instead of 40)",
            "5. Cache results if same concept requested again"
        ]
    },
    
    "React Hook Error 'useAI'": {
        "Problem": "useAI is not defined or Context error",
        "Solutions": [
            "1. Verify AIProvider wraps your entire app",
            "2. Import useAI from AIIntegration.jsx",
            "3. Don't use useAI outside of AIProvider",
            "4. Check that TeacherAIClient.js is imported",
            "5. Clear node_modules and reinstall if needed"
        ]
    }
}

# ============================================
# FILE STRUCTURE AFTER SETUP
# ============================================

EXPECTED_FILE_STRUCTURE = """
Frontend/
├── src/
│   ├── api/
│   │   └── TeacherAIClient.js           (COPY from AI-agents/)
│   ├── components/
│   │   ├── AIIntegration.jsx            (COPY from AI-agents/)
│   │   ├── QuizGame.jsx                 (USE QuizComponent)
│   │   ├── SpeedChallenge.jsx           (USE SpeedChallengeComponent)
│   │   ├── ConceptSummary.jsx           (USE ConceptSummary)
│   │   └── ... other components
│   ├── pages/
│   │   ├── Concepts.jsx                 (USE components)
│   │   ├── PlayGame.jsx
│   │   └── Progress.jsx
│   ├── App.jsx                          (WRAP with AIProvider)
│   └── main.jsx
└── package.json

Backend/
├── main.py                              (FastAPI server)
├── teacher_ai_agent.py                 (AI logic)
├── config.py
├── requirements.txt
├── .env                                 (OPENAI_API_KEY)
└── test_agent.py
"""

# ============================================
# QUICK COMMAND REFERENCE
# ============================================

COMMANDS = {
    "Start Backend": "cd Backend && python main.py",
    "Install Dependencies": "cd Backend && pip install -r requirements.txt",
    "Start Frontend": "cd Frontend && npm run dev",
    "Test API": "curl http://localhost:8000/health",
    "View API Docs": "http://localhost:8000/docs",
    "Check OpenAI Key": "echo $OPENAI_API_KEY",
    "Run Tests": "cd Backend && python test_agent.py"
}

# ============================================
# NEXT STEPS
# ============================================

NEXT_STEPS = """
After basic implementation:

1. STYLING
   - Add CSS to make games look beautiful
   - Use Navbar.jsx and other existing components
   - Style the quiz buttons, timer, progress bar

2. FEATURES
   - Add student progress tracking
   - Create dashboard to show scores
   - Add difficulty selection
   - Show explanations after each answer

3. OPTIMIZATION
   - Cache frequently requested concepts
   - Add keyboard shortcuts for faster input
   - Implement sound effects for feedback
   - Add animations for transitions

4. PRODUCTION
   - Deploy FastAPI to cloud (Heroku, AWS, etc.)
   - Deploy React to Vercel/Netlify
   - Set up database for user progress
   - Add authentication (login/signup)
   - Create admin panel for teachers

5. ADVANCED
   - Add adaptive difficulty based on performance
   - Create learning paths
   - Add social features (leaderboards)
   - Implement real-time multiplayer challenges
"""

# ============================================
# SUPPORT
# ============================================

SUPPORT = {
    "API Documentation": "http://localhost:8000/docs",
    "Python Examples": "AI-agents/examples.py",
    "Quick Snippets": "AI-agents/quick_snippets.py",
    "React Integration": "Frontend/src/components/AIIntegration.jsx",
    "JavaScript Client": "AI-agents/TeacherAIClient.js",
    "Testing Script": "Backend/test_agent.py"
}

if __name__ == "__main__":
    print("=" * 70)
    print("TEACHER AI AGENT - IMPLEMENTATION GUIDE")
    print("=" * 70)
    print()
    
    print("STEP 1: PREREQUISITES")
    for category, items in PREREQUISITES.items():
        print(f"\n{category.upper()}:")
        for item in items:
            print(f"  {item}")
    
    print("\n" + "=" * 70)
    print("\nSTEP 2: SETUP")
    for step, instructions in SETUP_CHECKLIST.items():
        print(f"\n{step}")
        for instruction in instructions:
            print(f"  {instruction}")
    
    print("\n" + "=" * 70)
    print("\nSTEP 3: COMMON COMMANDS")
    for name, command in COMMANDS.items():
        print(f"  {name}: {command}")
    
    print("\n" + "=" * 70)
    print("\nNEXT STEPS:")
    print(NEXT_STEPS)
