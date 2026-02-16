import { useContext, useMemo } from 'react';
import { useNavigate } from 'react-router-dom';
import { GameContext } from '../context/GameContext';
import ConceptCard from '../components/ConceptCard';
import { CONCEPTS_BY_SUBJECT } from '../utils/constants';
import '../styles/Pages.css';

/**
 * Concepts page - displays concepts for selected subject
 */
const Concepts = () => {
  const navigate = useNavigate();
  const { selectedSubject, selectConcept } = useContext(GameContext);

  const concepts = useMemo(() => {
    return selectedSubject ? CONCEPTS_BY_SUBJECT[selectedSubject] : [];
  }, [selectedSubject]);

  const handlePlayGame = (conceptName) => {
    selectConcept(conceptName);
    navigate('/play-game');
  };

  if (!selectedSubject) {
    return (
      <div className="page concepts-page">
        <div className="empty-state">
          <p className="empty-icon">📚</p>
          <h2>No Subject Selected</h2>
          <p>Please select a subject first</p>
          <button 
            onClick={() => navigate('/subjects')}
            className="btn btn-primary"
          >
            Go to Subjects
          </button>
        </div>
      </div>
    );
  }

  return (
    <div className="page concepts-page">
      <div className="page-header">
        <button 
          onClick={() => navigate('/subjects')}
          className="back-button"
        >
          ← Back
        </button>
        <div>
          <h1>📚 {selectedSubject}</h1>
          <p>Choose a concept to master</p>
        </div>
      </div>

      <div className="concepts-grid">
        {concepts.map((concept) => (
          <ConceptCard 
            key={concept.id}
            concept={concept}
            onPlayGame={handlePlayGame}
          />
        ))}
      </div>
    </div>
  );
};

export default Concepts;
