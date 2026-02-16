/**
 * Example: Using Teacher AI Client in a React Component
 * This shows how to integrate the AI Agent into your frontend
 */

import React, { useState, useCallback } from 'react';
import TeacherAIClient from './TeacherAIClient';

const AIGameGenerator = () => {
  const [concept, setConcept] = useState('');
  const [level, setLevel] = useState('intermediate');
  const [loading, setLoading] = useState(false);
  const [generatedModule, setGeneratedModule] = useState(null);
  const [error, setError] = useState(null);

  const client = new TeacherAIClient('http://localhost:8000');

  const handleGenerateModule = useCallback(async () => {
    if (!concept.trim()) {
      setError('Please enter a concept');
      return;
    }

    setLoading(true);
    setError(null);

    try {
      const result = await client.generateLearningModule(concept, level);
      if (result.success) {
        setGeneratedModule(result.data);
      } else {
        setError('Failed to generate module');
      }
    } catch (err) {
      setError(err.message);
    } finally {
      setLoading(false);
    }
  }, [concept, level]);

  return (
    <div className="ai-game-generator">
      <h1>AI-Powered Game Generator for Teachers</h1>

      <div className="input-section">
        <div className="form-group">
          <label htmlFor="concept">Teaching Concept:</label>
          <input
            id="concept"
            type="text"
            value={concept}
            onChange={(e) => setConcept(e.target.value)}
            placeholder="e.g., Photosynthesis, Newton's Laws, Fractions..."
          />
        </div>

        <div className="form-group">
          <label htmlFor="level">Student Level:</label>
          <select
            id="level"
            value={level}
            onChange={(e) => setLevel(e.target.value)}
          >
            <option value="beginner">Beginner</option>
            <option value="intermediate">Intermediate</option>
            <option value="advanced">Advanced</option>
          </select>
        </div>

        <button
          onClick={handleGenerateModule}
          disabled={loading}
          className="generate-button"
        >
          {loading ? 'Generating...' : 'Generate Learning Module'}
        </button>
      </div>

      {error && <div className="error-message">{error}</div>}

      {generatedModule && (
        <div className="module-display">
          <h2>Generated Module: {generatedModule.summary.title}</h2>
          <p>{generatedModule.summary.summary}</p>

          <div className="key-points">
            <h3>Key Points:</h3>
            <ul>
              {generatedModule.summary.key_points.map((point, idx) => (
                <li key={idx}>{point}</li>
              ))}
            </ul>
          </div>

          <div className="games-section">
            <h3>Available Games:</h3>
            <div className="game-cards">
              {Object.entries(generatedModule.games).map(([gameType, gameData]) => (
                <div key={gameType} className="game-card">
                  <h4>{gameData.title}</h4>
                  <p>{gameData.description}</p>
                  <button className="play-button">
                    Play {gameType.charAt(0).toUpperCase() + gameType.slice(1)}
                  </button>
                </div>
              ))}
            </div>
          </div>

          <div className="module-info">
            <p>
              <strong>Total Duration:</strong>{' '}
              {Math.round(generatedModule.estimated_total_duration / 60)} minutes
            </p>
            <p>
              <strong>Teaching Notes:</strong> {generatedModule.teaching_notes}
            </p>
          </div>
        </div>
      )}
    </div>
  );
};

export default AIGameGenerator;
