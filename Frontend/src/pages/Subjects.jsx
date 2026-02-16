import { useContext } from 'react';
import { GameContext } from '../context/GameContext';
import SubjectCard from '../components/SubjectCard';
import { SUBJECTS } from '../utils/constants';
import '../styles/Pages.css';

/**
 * Subjects page - displays all available subjects
 */
const Subjects = () => {
  const { selectSubject } = useContext(GameContext);

  return (
    <div className="page subjects-page">
      <div className="page-header">
        <h1>Choose Your Subject</h1>
        <p>Select a subject to start learning and practicing</p>
      </div>

      <div className="subjects-grid">
        {SUBJECTS.map((subject) => (
          <SubjectCard 
            key={subject.id}
            subject={subject}
            onSelect={selectSubject}
          />
        ))}
      </div>

      <div className="info-section">
        <h2>How It Works</h2>
        <ol className="steps">
          <li>
            <strong>Choose a Subject</strong> - Pick from Java, DSA, SQL, or Aptitude
          </li>
          <li>
            <strong>Select a Concept</strong> - Pick a specific topic within the subject
          </li>
          <li>
            <strong>Play Games</strong> - Solve problems through quiz, speed, or puzzle games
          </li>
          <li>
            <strong>Track Progress</strong> - Monitor your improvement and unlock achievements
          </li>
        </ol>
      </div>
    </div>
  );
};

export default Subjects;
