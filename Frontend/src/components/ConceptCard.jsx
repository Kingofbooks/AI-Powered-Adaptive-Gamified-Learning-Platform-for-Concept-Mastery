import { Link } from 'react-router-dom';
import Button from './Button';
import '../styles/Card.css';

/**
 * Concept card component - displays a concept with action to play game
 */
const ConceptCard = ({ concept, onPlayGame }) => {
  return (
    <div className="card concept-card">
      <h3 className="card-title">{concept.name}</h3>
      <p className="card-description">{concept.description}</p>
      <Button 
        variant="primary" 
        size="small"
        onClick={() => onPlayGame(concept.name)}
      >
        Play Game
      </Button>
    </div>
  );
};

export default ConceptCard;
