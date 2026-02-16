import { Link } from 'react-router-dom';
import '../styles/Card.css';

/**
 * Subject card component - displays a subject with icon and description
 */
const SubjectCard = ({ subject, onSelect }) => {
  return (
    <Link to="/concepts" className="card-link">
      <div 
        className="card subject-card"
        onClick={() => onSelect(subject.name)}
      >
        <div className="card-icon">{subject.icon}</div>
        <h3 className="card-title">{subject.name}</h3>
        <p className="card-description">{subject.description}</p>
        <div className="card-hover-action">
          Explore →
        </div>
      </div>
    </Link>
  );
};

export default SubjectCard;
