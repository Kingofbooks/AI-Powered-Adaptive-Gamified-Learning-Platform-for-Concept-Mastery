import Button from './Button';
import '../styles/GameBox.css';

/**
 * GameBox component - container for game iframe
 */
const GameBox = ({ gamePath, subject, concept, onBack }) => {
  if (!gamePath) {
    return (
      <div className="game-box-empty">
        <p>No game loaded</p>
        <Button onClick={onBack} variant="secondary">
          Back
        </Button>
      </div>
    );
  }

  return (
    <div className="game-box-wrapper">
      <div className="game-box-header">
        <div className="game-info">
          <h2>{subject} - {concept}</h2>
        </div>
        <Button 
          onClick={onBack} 
          variant="secondary"
          size="small"
        >
          ← Back
        </Button>
      </div>

      <div className="game-box-container">
        <iframe
          src={gamePath}
          title="Game"
          className="game-iframe"
          sandbox="allow-scripts allow-same-origin allow-forms"
        />
      </div>

      <div className="game-box-footer">
        <p className="hint">Complete the game to earn points and unlock new challenges!</p>
      </div>
    </div>
  );
};

export default GameBox;
