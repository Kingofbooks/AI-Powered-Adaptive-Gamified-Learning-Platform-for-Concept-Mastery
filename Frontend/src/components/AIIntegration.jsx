/**
 * FRONTEND INTEGRATION GUIDE
 * 
 * This file shows how to integrate the Teacher AI Agent into your React frontend
 * Copy these patterns directly into your React components!
 */

// ============================================
// 1. BASIC SETUP IN YOUR REACT APP
// ============================================

// App.jsx or main entry point:
import React from 'react';
import { TeacherAI } from './TeacherAIClient';

// Create a context for the AI client (optional but recommended)
export const AIContext = React.createContext();

export function AIProvider({ children }) {
    const [ai, setAi] = React.useState(null);

    React.useEffect(() => {
        const client = new TeacherAI("http://localhost:8000");
        setAi(client);
    }, []);

    return (
        <AIContext.Provider value={ai}>
            {children}
        </AIContext.Provider>
    );
}

// Hook to use AI client in any component
export function useAI() {
    const ai = React.useContext(AIContext);
    if (!ai) {
        throw new Error('useAI must be used within AIProvider');
    }
    return ai;
}

// ============================================
// 2. SIMPLE QUIZ COMPONENT
// ============================================

export function QuizGame({ concept = "Photosynthesis", numQuestions = 5 }) {
    const ai = useAI();
    const [quiz, setQuiz] = React.useState(null);
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState(null);
    const [currentIndex, setCurrentIndex] = React.useState(0);
    const [score, setScore] = React.useState(0);
    const [streak, setStreak] = React.useState(0);
    const [answers, setAnswers] = React.useState([]); // Track all answers
    const [finished, setFinished] = React.useState(false);

    // Load quiz on mount
    React.useEffect(() => {
        async function loadQuiz() {
            try {
                setLoading(true);
                const data = await ai.generateQuiz(concept, numQuestions, "intermediate");
                setQuiz(data);
            } catch (err) {
                setError(err.message);
                console.error("Failed to load quiz:", err);
            } finally {
                setLoading(false);
            }
        }

        loadQuiz();
    }, [concept, numQuestions, ai]);

    if (loading) return <div className="loading">Loading quiz...</div>;
    if (error) return <div className="error">Error: {error}</div>;
    if (!quiz) return <div>No quiz loaded</div>;

    // Handle answer click
    const handleAnswer = (selectedIndex) => {
        const question = quiz.questions[currentIndex];
        const isCorrect = selectedIndex === question.correct_answer;

        // Calculate points
        const newStreak = isCorrect ? streak + 1 : 0;
        const points = ai.scoreAnswer(isCorrect, 10, newStreak);
        
        setScore(score + points);
        setStreak(newStreak);
        setAnswers([...answers, {
            questionIndex: currentIndex,
            selected: selectedIndex,
            correct: question.correct_answer,
            isCorrect
        }]);

        // Move to next question or finish
        if (currentIndex < quiz.questions.length - 1) {
            setCurrentIndex(currentIndex + 1);
        } else {
            setFinished(true);
        }
    };

    // Results screen
    if (finished) {
        const correctCount = answers.filter(a => a.isCorrect).length;
        const accuracy = ai.calculateAccuracy(correctCount, quiz.questions.length);

        return (
            <div className="quiz-results">
                <h2>Quiz Complete! 🎉</h2>
                <div className="stats">
                    <p>Score: <strong>{score}/{quiz.questions.length * 10}</strong></p>
                    <p>Accuracy: <strong>{accuracy}%</strong></p>
                    <p>Correct: <strong>{correctCount}/{quiz.questions.length}</strong></p>
                </div>
                
                <button 
                    onClick={() => window.location.reload()}
                    className="btn-primary"
                >
                    Play Again
                </button>
            </div>
        );
    }

    // Quiz display
    const question = quiz.questions[currentIndex];
    const progress = ((currentIndex + 1) / quiz.questions.length) * 100;

    return (
        <div className="quiz-game">
            <div className="header">
                <h2>{quiz.title}</h2>
                <div className="progress-bar">
                    <div className="progress-fill" style={{ width: `${progress}%` }} />
                </div>
                <p className="progress-text">
                    Question {currentIndex + 1} of {quiz.questions.length}
                </p>
                <p className="score">Score: {score}</p>
                {streak > 1 && <p className="streak">🔥 Streak: {streak}</p>}
            </div>

            <div className="question-container">
                <h3>{question.question}</h3>
                <div className="difficulty">Level: {question.difficulty}</div>

                <div className="options">
                    {question.options.map((option, idx) => (
                        <button
                            key={idx}
                            className="option-btn"
                            onClick={() => handleAnswer(idx)}
                        >
                            <span className="number">{idx})</span>
                            <span className="text">{option}</span>
                        </button>
                    ))}
                </div>

                <div className="explanation">
                    <strong>Concept:</strong> {question.explanation}
                </div>
            </div>
        </div>
    );
}

// ============================================
// 3. SPEED CHALLENGE COMPONENT
// ============================================

export function SpeedChallenge({ concept = "Basic Math" }) {
    const ai = useAI();
    const [challenge, setChallenge] = React.useState(null);
    const [loading, setLoading] = React.useState(true);
    const [currentIndex, setCurrentIndex] = React.useState(0);
    const [score, setScore] = React.useState(0);
    const [timeLeft, setTimeLeft] = React.useState(0);
    const [inputValue, setInputValue] = React.useState("");
    const [finished, setFinished] = React.useState(false);

    // Load challenge
    React.useEffect(() => {
        async function loadChallenge() {
            try {
                setLoading(false);
                const data = await ai.generateSpeedChallenge(concept, 10, "intermediate");
                setChallenge(data);
                setTimeLeft(data.total_duration);
            } catch (err) {
                console.error("Failed to load challenge:", err);
            }
        }

        loadChallenge();
    }, [concept, ai]);

    // Timer countdown
    React.useEffect(() => {
        if (!challenge || finished) return;

        const timer = setInterval(() => {
            setTimeLeft((prev) => {
                if (prev <= 1) {
                    setFinished(true);
                    clearInterval(timer);
                    return 0;
                }
                return prev - 1;
            });
        }, 1000);

        return () => clearInterval(timer);
    }, [challenge, finished]);

    if (loading) return <div className="loading">Loading challenge...</div>;
    if (!challenge) return <div>No challenge loaded</div>;

    if (finished) {
        return (
            <div className="results-screen">
                <h2>Challenge Complete! 🏆</h2>
                <p className="score">Final Score: {score}</p>
                <p className="time">Time: {challenge.total_duration}s</p>
                <button 
                    onClick={() => window.location.reload()}
                    className="btn-primary"
                >
                    Try Again
                </button>
            </div>
        );
    }

    const current = challenge.challenges[currentIndex];
    const progress = ((currentIndex + 1) / challenge.challenges.length) * 100;
    const formattedTime = ai.formatTime(timeLeft);

    return (
        <div className="speed-challenge">
            <div className="header">
                <h2>{challenge.title}</h2>
                <div className="stats">
                    <div className="stat">
                        <span className="label">Score:</span>
                        <span className="value">{score}</span>
                    </div>
                    <div className="stat highlight">
                        <span className="label">Time:</span>
                        <span className="value">{formattedTime}</span>
                    </div>
                    <div className="stat">
                        <span className="label">Question:</span>
                        <span className="value">{currentIndex + 1}/{challenge.challenges.length}</span>
                    </div>
                </div>
            </div>

            <div className="progress-bar">
                <div className="progress-fill" style={{ width: `${progress}%` }} />
            </div>

            <div className="challenge-container">
                <h3>{current.prompt}</h3>
                <p className="time-limit">Max: {current.time_limit}s</p>

                <input
                    type="text"
                    value={inputValue}
                    onChange={(e) => setInputValue(e.target.value)}
                    onKeyPress={(e) => {
                        if (e.key === "Enter") {
                            // Check answer
                            if (String(inputValue).trim() === String(current.answer)) {
                                setScore(score + 10);
                            }
                            // Move to next
                            setInputValue("");
                            if (currentIndex < challenge.challenges.length - 1) {
                                setCurrentIndex(currentIndex + 1);
                            } else {
                                setFinished(true);
                            }
                        }
                    }}
                    placeholder="Type answer and press Enter"
                    className="answer-input"
                    autoFocus
                />
                <p className="hint">Press Enter to submit</p>
            </div>
        </div>
    );
}

// ============================================
// 4. CONCEPT SUMMARY COMPONENT
// ============================================

export function ConceptSummary({ concept = "Photosynthesis" }) {
    const ai = useAI();
    const [summary, setSummary] = React.useState(null);
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState(null);

    React.useEffect(() => {
        async function loadSummary() {
            try {
                const data = await ai.summarizeConcept(concept, "intermediate");
                setSummary(data);
            } catch (err) {
                setError(err.message);
            } finally {
                setLoading(false);
            }
        }

        loadSummary();
    }, [concept, ai]);

    if (loading) return <div className="loading">Loading concept...</div>;
    if (error) return <div className="error">Error: {error}</div>;
    if (!summary) return <div>No summary loaded</div>;

    return (
        <div className="concept-summary">
            <h2>{summary.title}</h2>
            <p className="level">Level: {summary.level}</p>

            <section className="summary-section">
                <h3>Overview</h3>
                <p>{summary.summary}</p>
            </section>

            <section className="summary-section">
                <h3>Key Points</h3>
                <ul>
                    {summary.key_points.map((point, idx) => (
                        <li key={idx}>{point}</li>
                    ))}
                </ul>
            </section>

            <section className="summary-section">
                <h3>Learning Objectives</h3>
                <ul>
                    {summary.learning_objectives.map((obj, idx) => (
                        <li key={idx}>{obj}</li>
                    ))}
                </ul>
            </section>
        </div>
    );
}

// ============================================
// 5. COMPLETE LEARNING MODULE COMPONENT
// ============================================

export function CompleteLearningModule({ concept = "Water Cycle" }) {
    const ai = useAI();
    const [module, setModule] = React.useState(null);
    const [loading, setLoading] = React.useState(true);
    const [error, setError] = React.useState(null);
    const [activeTab, setActiveTab] = React.useState("summary"); // summary, quiz, puzzle, speed

    React.useEffect(() => {
        async function loadModule() {
            try {
                setLoading(true);
                const data = await ai.generateCompleteModule(concept, "intermediate");
                setModule(data);
            } catch (err) {
                setError(err.message);
            } finally {
                setLoading(false);
            }
        }

        loadModule();
    }, [concept, ai]);

    if (loading) return <div className="loading">Generating complete module (30-40 seconds)...</div>;
    if (error) return <div className="error">Error: {error}</div>;
    if (!module) return <div>No module loaded</div>;

    return (
        <div className="learning-module">
            <h2>{module.concept}</h2>
            <p className="level">Level: {module.level}</p>

            <div className="tabs">
                {['summary', 'quiz', 'puzzle', 'speed'].map(tab => (
                    <button
                        key={tab}
                        className={`tab ${activeTab === tab ? 'active' : ''}`}
                        onClick={() => setActiveTab(tab)}
                    >
                        {tab.toUpperCase()}
                    </button>
                ))}
            </div>

            <div className="tab-content">
                {activeTab === "summary" && (
                    <div className="summary-content">
                        <h3>{module.summary.title}</h3>
                        <p>{module.summary.summary}</p>
                        <ul>
                            {module.summary.key_points.map((point, idx) => (
                                <li key={idx}>{point}</li>
                            ))}
                        </ul>
                    </div>
                )}

                {activeTab === "quiz" && (
                    <div className="quiz-content">
                        <h3>Quiz - {module.games.quiz.title}</h3>
                        <p>Questions: {module.games.quiz.questions.length}</p>
                        <p>Duration: {module.games.quiz.estimated_duration}s</p>
                    </div>
                )}

                {activeTab === "puzzle" && (
                    <div className="puzzle-content">
                        <h3>{module.games.puzzle.title}</h3>
                        <p>Type: {module.games.puzzle.puzzle_type}</p>
                        <p>Problem: {module.games.puzzle.content.problem_statement}</p>
                    </div>
                )}

                {activeTab === "speed" && (
                    <div className="speed-content">
                        <h3>Speed Challenge</h3>
                        <p>Challenges: {module.games.speed.challenges.length}</p>
                        <p>Duration: {module.games.speed.total_duration}s</p>
                    </div>
                )}
            </div>

            <div className="teaching-notes">
                <h4>Teaching Notes</h4>
                <p>{module.teaching_notes}</p>
            </div>
        </div>
    );
}

// ============================================
// 6. USAGE IN YOUR APP
// ============================================

// In your main App.jsx:
/*
import { AIProvider } from './components/AIIntegration'
import { QuizGame } from './components/QuizGame'

function App() {
    return (
        <AIProvider>
            <div className="app">
                <h1>AI Learning Platform</h1>
                <QuizGame concept="Photosynthesis" numQuestions={5} />
            </div>
        </AIProvider>
    );
}

export default App;
*/

// ============================================
// 7. CSS STYLING EXAMPLES
// ============================================

/*
.quiz-game {
    max-width: 800px;
    margin: 0 auto;
    padding: 20px;
    background: white;
    border-radius: 8px;
    box-shadow: 0 2px 8px rgba(0,0,0,0.1);
}

.options {
    display: flex;
    flex-direction: column;
    gap: 10px;
    margin: 20px 0;
}

.option-btn {
    padding: 15px;
    text-align: left;
    border: 2px solid #e0e0e0;
    border-radius: 4px;
    background: white;
    cursor: pointer;
    transition: all 0.3s;
}

.option-btn:hover {
    border-color: #4CAF50;
    background: #f5f5f5;
}

.progress-bar {
    width: 100%;
    height: 4px;
    background: #e0e0e0;
    border-radius: 2px;
    overflow: hidden;
    margin: 10px 0;
}

.progress-fill {
    height: 100%;
    background: #4CAF50;
    transition: width 0.3s;
}

.streak {
    color: #ff6b6b;
    font-weight: bold;
    font-size: 18px;
}

.btn-primary {
    background: #4CAF50;
    color: white;
    padding: 12px 24px;
    border: none;
    border-radius: 4px;
    cursor: pointer;
    font-size: 16px;
}

.btn-primary:hover {
    background: #45a049;
}
*/

export default {
    AIProvider,
    useAI,
    QuizGame,
    SpeedChallenge,
    ConceptSummary,
    CompleteLearningModule
};
