# AI Agent Backend Integration Guide

This guide shows how to integrate the Teacher AI Agent API with your React frontend.

---

## Step 1: Add the Client SDK to Your Project

```bash
# Copy the JavaScript client to your frontend
cp TeacherAIClient.js ../Frontend/src/api/
```

---

## Step 2: Create an API Service Wrapper (Optional but Recommended)

File: `src/services/aiService.js`

```javascript
import TeacherAIClient from '../api/TeacherAIClient';

class AIService {
  constructor(baseUrl = process.env.REACT_APP_AI_API_URL || 'http://localhost:8000') {
    this.client = new TeacherAIClient(baseUrl);
  }

  async generateLearningModule(concept, level = 'intermediate') {
    try {
      const result = await this.client.generateLearningModule(concept, level);
      if (result.success) {
        return result.data;
      } else {
        throw new Error('Failed to generate module');
      }
    } catch (error) {
      console.error('Error generating module:', error);
      throw error;
    }
  }

  async generateQuiz(concept, numQuestions = 5) {
    try {
      const result = await this.client.generateQuiz(concept, numQuestions);
      if (result.success) {
        return result.data;
      } else {
        throw new Error('Failed to generate quiz');
      }
    } catch (error) {
      console.error('Error generating quiz:', error);
      throw error;
    }
  }

  async summarizeConcept(concept, level = 'intermediate') {
    try {
      const result = await this.client.summarizeConcept(concept, level);
      if (result.success) {
        return result.data;
      } else {
        throw new Error('Failed to summarize concept');
      }
    } catch (error) {
      console.error('Error summarizing concept:', error);
      throw error;
    }
  }

  async checkHealth() {
    return this.client.healthCheck();
  }
}

export default new AIService();
```

---

## Step 3: Create a Context for Shared State

File: `src/context/AIContext.jsx`

```javascript
import React, { createContext, useState, useCallback } from 'react';
import aiService from '../services/aiService';

export const AIContext = createContext();

export const AIProvider = ({ children }) => {
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [generatedModule, setGeneratedModule] = useState(null);
  const [apiHealth, setApiHealth] = useState(null);

  const checkHealth = useCallback(async () => {
    try {
      const isHealthy = await aiService.checkHealth();
      setApiHealth(isHealthy);
      return isHealthy;
    } catch (err) {
      setApiHealth(false);
      return false;
    }
  }, []);

  const generateModule = useCallback(async (concept, level = 'intermediate') => {
    setLoading(true);
    setError(null);
    try {
      const module = await aiService.generateLearningModule(concept, level);
      setGeneratedModule(module);
      return module;
    } catch (err) {
      setError(err.message);
      setGeneratedModule(null);
      throw err;
    } finally {
      setLoading(false);
    }
  }, []);

  const generateQuiz = useCallback(async (concept, numQuestions = 5) => {
    setLoading(true);
    setError(null);
    try {
      const quiz = await aiService.generateQuiz(concept, numQuestions);
      return quiz;
    } catch (err) {
      setError(err.message);
      throw err;
    } finally {
      setLoading(false);
    }
  }, []);

  const summarizeConcept = useCallback(async (concept, level = 'intermediate') => {
    setLoading(true);
    setError(null);
    try {
      const summary = await aiService.summarizeConcept(concept, level);
      return summary;
    } catch (err) {
      setError(err.message);
      throw err;
    } finally {
      setLoading(false);
    }
  }, []);

  const value = {
    loading,
    error,
    generatedModule,
    apiHealth,
    checkHealth,
    generateModule,
    generateQuiz,
    summarizeConcept,
  };

  return <AIContext.Provider value={value}>{children}</AIContext.Provider>;
};

export const useAI = () => {
  const context = React.useContext(AIContext);
  if (!context) {
    throw new Error('useAI must be used within an AIProvider');
  }
  return context;
};
```

---

## Step 4: Create a Game Generator Component

File: `src/components/GameGenerator.jsx`

```javascript
import React, { useState, useEffect } from 'react';
import { useAI } from '../context/AIContext';
import '../styles/GameGenerator.css';

const GameGenerator = () => {
  const [concept, setConcept] = useState('');
  const [level, setLevel] = useState('intermediate');
  const { loading, error, apiHealth, checkHealth, generateModule } = useAI();

  useEffect(() => {
    checkHealth();
  }, [checkHealth]);

  const handleGenerateModule = async (e) => {
    e.preventDefault();
    if (!concept.trim()) {
      alert('Please enter a teaching concept');
      return;
    }
    await generateModule(concept, level);
  };

  return (
    <div className="game-generator">
      <h1>🎮 AI-Powered Game Generator</h1>

      {!apiHealth && (
        <div className="alert alert-warning">
          ⚠️ API Server is not responding. Make sure it's running on http://localhost:8000
        </div>
      )}

      {error && <div className="alert alert-error">❌ Error: {error}</div>}

      <form onSubmit={handleGenerateModule} className="generator-form">
        <div className="form-group">
          <label htmlFor="concept">
            Teaching Concept:
            <span className="required">*</span>
          </label>
          <input
            id="concept"
            type="text"
            value={concept}
            onChange={(e) => setConcept(e.target.value)}
            placeholder="e.g., Photosynthesis, Newton's Laws, Fractions..."
            disabled={loading}
            className="form-input"
          />
        </div>

        <div className="form-group">
          <label htmlFor="level">
            Student Level:
            <span className="required">*</span>
          </label>
          <select
            id="level"
            value={level}
            onChange={(e) => setLevel(e.target.value)}
            disabled={loading}
            className="form-input"
          >
            <option value="beginner">🌱 Beginner</option>
            <option value="intermediate">📚 Intermediate</option>
            <option value="advanced">⚡ Advanced</option>
          </select>
        </div>

        <button
          type="submit"
          disabled={loading || !apiHealth}
          className="btn btn-primary"
        >
          {loading ? (
            <>
              <span className="spinner"></span>
              Generating module...
            </>
          ) : (
            '✨ Generate Learning Module'
          )}
        </button>
      </form>
    </div>
  );
};

export default GameGenerator;
```

---

## Step 5: Create a Module Display Component

File: `src/components/GeneratedModuleDisplay.jsx`

```javascript
import React from 'react';
import { useAI } from '../context/AIContext';
import '../styles/ModuleDisplay.css';

const GeneratedModuleDisplay = () => {
  const { generatedModule } = useAI();

  if (!generatedModule) {
    return null;
  }

  const { summary, games, estimated_total_duration, teaching_notes } = generatedModule;

  return (
    <div className="module-display">
      <div className="module-header">
        <h2>📖 {summary.title}</h2>
        <span className="level-badge">{summary.level.toUpperCase()}</span>
      </div>

      <div className="module-content">
        <section className="summary-section">
          <h3>Concept Overview</h3>
          <p className="summary-text">{summary.summary}</p>

          <h4>Key Learning Points:</h4>
          <ul className="key-points">
            {summary.key_points.map((point, idx) => (
              <li key={idx}>✓ {point}</li>
            ))}
          </ul>

          <h4>Learning Objectives:</h4>
          <ul className="objectives">
            {summary.learning_objectives.map((obj, idx) => (
              <li key={idx}>🎯 {obj}</li>
            ))}
          </ul>

          {summary.examples && summary.examples.length > 0 && (
            <>
              <h4>Real-World Examples:</h4>
              <ul className="examples">
                {summary.examples.map((example, idx) => (
                  <li key={idx}>💡 {example}</li>
                ))}
              </ul>
            </>
          )}
        </section>

        <section className="games-section">
          <h3>🎮 Interactive Games</h3>
          <div className="game-cards">
            {Object.entries(games).map(([gameType, gameData]) => (
              <div key={gameType} className="game-card">
                <div className="game-card-header">
                  <h4>{gameData.title}</h4>
                  <span className="game-type-badge">{gameType}</span>
                </div>

                <p className="game-description">{gameData.description}</p>

                {gameData.learning_objectives && (
                  <div className="game-objectives">
                    <small>
                      <strong>Objectives:</strong>{' '}
                      {gameData.learning_objectives.join(', ')}
                    </small>
                  </div>
                )}

                <div className="game-info">
                  <span className="info-item">
                    ⏱️ {gameData.estimated_duration || gameData.total_duration}s
                  </span>
                  {gameData.difficulty_level && (
                    <span className="info-item">
                      📊 {gameData.difficulty_level}
                    </span>
                  )}
                </div>

                <button className="btn btn-game">
                  Play {gameType.charAt(0).toUpperCase() + gameType.slice(1)}
                </button>
              </div>
            ))}
          </div>
        </section>

        <section className="module-info">
          <h3>ℹ️ Module Information</h3>
          <div className="info-grid">
            <div className="info-item">
              <strong>Total Duration:</strong>{' '}
              {Math.round(estimated_total_duration / 60)} minutes
            </div>
            <div className="info-item">
              <strong>Number of Games:</strong> {Object.keys(games).length}
            </div>
          </div>
          <p className="teaching-notes">
            <strong>Teaching Notes:</strong> {teaching_notes}
          </p>
        </section>
      </div>
    </div>
  );
};

export default GeneratedModuleDisplay;
```

---

## Step 6: Add Styling

File: `src/styles/GameGenerator.css`

```css
.game-generator {
  max-width: 800px;
  margin: 2rem auto;
  padding: 2rem;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  border-radius: 15px;
  color: white;
}

.game-generator h1 {
  text-align: center;
  margin-bottom: 2rem;
  font-size: 2.5rem;
}

.alert {
  padding: 1rem;
  margin-bottom: 1.5rem;
  border-radius: 8px;
}

.alert-warning {
  background-color: #fff3cd;
  color: #856404;
  border: 1px solid #ffeaa7;
}

.alert-error {
  background-color: #f8d7da;
  color: #721c24;
  border: 1px solid #f5c6cb;
}

.generator-form {
  background: rgba(255, 255, 255, 0.95);
  padding: 2rem;
  border-radius: 10px;
  color: #333;
}

.form-group {
  margin-bottom: 1.5rem;
}

.form-group label {
  display: block;
  margin-bottom: 0.5rem;
  font-weight: bold;
}

.required {
  color: #e74c3c;
}

.form-input {
  width: 100%;
  padding: 0.75rem;
  border: 2px solid #e0e0e0;
  border-radius: 6px;
  font-size: 1rem;
  transition: border-color 0.3s;
}

.form-input:focus {
  outline: none;
  border-color: #667eea;
  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
}

.btn {
  padding: 0.75rem 1.5rem;
  border: none;
  border-radius: 6px;
  font-size: 1rem;
  font-weight: bold;
  cursor: pointer;
  transition: all 0.3s;
  width: 100%;
}

.btn-primary {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
}

.btn-primary:hover:not(:disabled) {
  transform: translateY(-2px);
  box-shadow: 0 5px 20px rgba(102, 126, 234, 0.4);
}

.btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.spinner {
  display: inline-block;
  width: 1rem;
  height: 1rem;
  border: 3px solid rgba(255, 255, 255, 0.3);
  border-top-color: white;
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
  margin-right: 0.5rem;
}

@keyframes spin {
  to {
    transform: rotate(360deg);
  }
}
```

---

## Step 7: Integrate into Main App

File: `src/App.jsx`

```javascript
import React from 'react';
import { AIProvider } from './context/AIContext';
import GameGenerator from './components/GameGenerator';
import GeneratedModuleDisplay from './components/GeneratedModuleDisplay';
import './App.css';

function App() {
  return (
    <AIProvider>
      <div className="App">
        <GameGenerator />
        <GeneratedModuleDisplay />
      </div>
    </AIProvider>
  );
}

export default App;
```

---

## Step 8: Set Environment Variables

File: `.env.local`

```
REACT_APP_AI_API_URL=http://localhost:8000
REACT_APP_API_TIMEOUT=120000
```

---

## Step 9: Configure CORS (Backend)

If you haven't already, ensure the backend allows your frontend domain:

In `main.py`:

```python
app.add_middleware(
    CORSMiddleware,
    allow_origins=[
        "http://localhost:3000",      # Local development
        "http://localhost:5173",      # Vite dev
        "https://yourdomain.com",     # Production
    ],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
```

---

## Step 10: Test the Integration

1. **Start the AI Agent backend:**
```bash
cd AI-agents
python main.py
```

2. **Start the React frontend:**
```bash
cd Frontend
npm run dev
```

3. **Test in browser:**
   - Navigate to `http://localhost:3000` (or your dev server)
   - Enter a concept (e.g., "Photosynthesis")
   - Select a level
   - Click "Generate Learning Module"
   - View the generated games!

---

## Troubleshooting Integration

### "API Server is not responding"
- Ensure `python main.py` is running
- Check the port is 8000
- Verify firewall allows localhost connections
- Check `REACT_APP_AI_API_URL` in .env

### CORS Errors
- Add frontend URL to `allow_origins` in `main.py`
- Ensure backend is using latest CORS middleware config

### Blank Module Display
- Check browser console for errors
- Verify API response structure matches expected format
- Check that API key is valid and set

### Slow Response Times
- API calls take 30-40 seconds for complete modules
- Show loading state to users
- Implement request caching

---

## Performance Optimization Tips

### 1. Implement Response Caching

```javascript
const cache = new Map();

export const useAI = () => {
  const generateModule = async (concept, level) => {
    const cacheKey = `${concept}:${level}`;
    
    if (cache.has(cacheKey)) {
      return cache.get(cacheKey);
    }
    
    const result = await aiService.generateLearningModule(concept, level);
    cache.set(cacheKey, result);
    return result;
  };
};
```

### 2. Implement Request Debouncing

```javascript
import { useCallback } from 'react';
import debounce from 'lodash.debounce';

const debouncedGenerate = useCallback(
  debounce((concept, level) => generateModule(concept, level), 1000),
  [generateModule]
);
```

### 3. Lazy Load Game Components

```javascript
const QuizGame = React.lazy(() => import('./games/QuizGame'));
const PuzzleGame = React.lazy(() => import('./games/PuzzleGame'));

<Suspense fallback={<div>Loading game...</div>}>
  <QuizGame data={games.quiz} />
</Suspense>
```

---

## Production Deployment

### Environment Variables for Production

```env
REACT_APP_AI_API_URL=https://api.yourdomain.com
REACT_APP_API_TIMEOUT=60000
```

### Build and Deploy

```bash
npm run build
# Deploy build/ folder to your hosting service
```

---

## Next Steps

1. ✅ Integrate the API client
2. ✅ Create context and state management
3. ✅ Build UI components
4. ✅ Test the integration
5. ⬜ Add game rendering components (Quiz, Puzzle, Speed)
6. ⬜ Implement scoring and progress tracking
7. ⬜ Add analytics and logging
8. ⬜ Deploy to production

---

For more information:
- Backend API: See [README.md](./README.md)
- Architecture: See [SYSTEM_OVERVIEW.md](./SYSTEM_OVERVIEW.md)
- Deployment: See [DEPLOYMENT.md](./DEPLOYMENT.md)
