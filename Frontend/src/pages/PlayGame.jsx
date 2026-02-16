import { useContext, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { GameContext } from '../context/GameContext';
import GameBox from '../components/GameBox';
import Loader from '../components/Loader';
import Button from '../components/Button';
import { getNextGame } from '../api/aiApi';
import '../styles/Pages.css';

/**
 * PlayGame page - main game interface
 */
const PlayGame = () => {
  const navigate = useNavigate();
  const { selectedSubject, selectedConcept, currentGame, gameLoading, startGame, resetGame } = 
    useContext(GameContext);
  const [gamePath, setGamePath] = useState(null);
  const [isGenerating, setIsGenerating] = useState(false);

  const handleGenerateGame = async () => {
    if (!selectedSubject || !selectedConcept) {
      navigate('/subjects');
      return;
    }

    setIsGenerating(true);
    try {
      const gameData = await getNextGame(selectedSubject, selectedConcept);
      if (gameData.success) {
        startGame(gameData.game);
        setGamePath(`/games/${gameData.game}/index.html`);
      }
    } catch (error) {
      console.error('Error generating game:', error);
    } finally {
      setIsGenerating(false);
    }
  };

  const handleBackToConceptts = () => {
    resetGame();
    navigate('/concepts');
  };

  if (!selectedSubject || !selectedConcept) {
    return (
      <div className="page play-game-page">
        <div className="empty-state">
          <p className="empty-icon">🎮</p>
          <h2>No Concept Selected</h2>
          <p>Please select a subject and concept first</p>
          <Button onClick={() => navigate('/subjects')} variant="primary">
            Go to Subjects
          </Button>
        </div>
      </div>
    );
  }

  return (
    <div className="page play-game-page">
      <div className="page-header">
        <Button 
          onClick={handleBackToConceptts}
          variant="secondary"
          size="small"
          className="back-button-inline"
        >
          ← Back
        </Button>
        <div>
          <h1>🎮 Play Game</h1>
          <p>{selectedSubject} - {selectedConcept}</p>
        </div>
      </div>

      {!currentGame && !gamePath ? (
        <div className="game-setup">
          <div className="setup-card">
            <h2>Ready to Challenge Yourself?</h2>
            <p>
              You're about to start a game on <strong>{selectedConcept}</strong> in <strong>{selectedSubject}</strong>
            </p>
            
            <div className="game-type-info">
              <div className="type-description">
                <h4>Game Types:</h4>
                <ul>
                  <li>🎯 <strong>Quiz:</strong> Answer multiple choice questions</li>
                  <li>⚡ <strong>Speed:</strong> Race against the clock</li>
                  <li>🧩 <strong>Puzzle:</strong> Solve programming challenges</li>
                </ul>
              </div>
            </div>

            <Button 
              onClick={handleGenerateGame}
              disabled={isGenerating || gameLoading}
              variant="primary"
              size="large"
              className="generate-btn"
            >
              {isGenerating || gameLoading ? 'Generating Game...' : 'Generate Game'}
            </Button>
          </div>
        </div>
      ) : isGenerating || gameLoading ? (
        <div className="game-loading">
          <Loader size="large" message="Generating your personalized game..." />
        </div>
      ) : (
        <GameBox 
          gamePath={gamePath}
          subject={selectedSubject}
          concept={selectedConcept}
          onBack={handleBackToConceptts}
        />
      )}
    </div>
  );
};

export default PlayGame;
