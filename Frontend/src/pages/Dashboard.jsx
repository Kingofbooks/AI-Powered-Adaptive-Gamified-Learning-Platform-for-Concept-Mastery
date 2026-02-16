import { useNavigate } from 'react-router-dom';
import { useContext } from 'react';
import { GameContext } from '../context/GameContext';
import Button from '../components/Button';
import '../styles/Pages.css';

/**
 * Dashboard page - shows quick actions and continue learning
 */
const Dashboard = () => {
  const navigate = useNavigate();
  const { selectSubject } = useContext(GameContext);

  const handleContinueLearning = () => {
    selectSubject('Java');
    navigate('/concepts');
  };

  const handleChooseSubject = () => {
    navigate('/subjects');
  };

  const handleViewProgress = () => {
    navigate('/progress');
  };

  return (
    <div className="page dashboard-page">
      <div className="page-header">
        <h1>Dashboard</h1>
        <p>Welcome back! Choose your next learning path</p>
      </div>

      <div className="dashboard-grid">
        <div className="dashboard-card continue-card">
          <div className="card-header">
            <h2>📚 Continue Learning</h2>
            <span className="badge">Java</span>
          </div>
          <div className="card-content">
            <p className="card-text">
              You were learning about <strong>Object-Oriented Programming</strong>
            </p>
            <div className="progress-bar">
              <div className="progress-fill" style={{ width: '60%' }}></div>
            </div>
            <p className="progress-text">60% progress</p>
          </div>
          <Button 
            onClick={handleContinueLearning}
            variant="primary"
            className="card-action"
          >
            Continue →
          </Button>
        </div>

        <div className="dashboard-card subjects-card">
          <div className="card-header">
            <h2>🎓 Choose Subject</h2>
          </div>
          <div className="card-content">
            <p className="card-text">
              Explore new subjects and expand your knowledge across different domains
            </p>
            <div className="subject-list">
              <div className="subject-chip">Java</div>
              <div className="subject-chip">DSA</div>
              <div className="subject-chip">SQL</div>
              <div className="subject-chip">Aptitude</div>
            </div>
          </div>
          <Button 
            onClick={handleChooseSubject}
            variant="primary"
            className="card-action"
          >
            Browse Subjects →
          </Button>
        </div>

        <div className="dashboard-card progress-card">
          <div className="card-header">
            <h2>📈 View Progress</h2>
          </div>
          <div className="card-content">
            <p className="card-text">
              Check your statistics, accuracy metrics, and discover areas to improve
            </p>
            <div className="quick-stats">
              <div className="stat">
                <span className="stat-value">47</span>
                <span className="stat-label">Games Played</span>
              </div>
              <div className="stat">
                <span className="stat-value">78.5%</span>
                <span className="stat-label">Accuracy</span>
              </div>
            </div>
          </div>
          <Button 
            onClick={handleViewProgress}
            variant="primary"
            className="card-action"
          >
            View Details →
          </Button>
        </div>
      </div>

      <div className="recent-activity">
        <h2>Recent Activity</h2>
        <div className="activity-list">
          <div className="activity-item">
            <span className="activity-icon">✓</span>
            <div className="activity-text">
              <p className="activity-title">Completed Java - Strings Game</p>
              <p className="activity-time">2 hours ago</p>
            </div>
            <span className="activity-score">+100 pts</span>
          </div>
          <div className="activity-item">
            <span className="activity-icon">✓</span>
            <div className="activity-text">
              <p className="activity-title">Completed DSA - Sorting Quiz</p>
              <p className="activity-time">1 day ago</p>
            </div>
            <span className="activity-score">+95 pts</span>
          </div>
          <div className="activity-item">
            <span className="activity-icon">✓</span>
            <div className="activity-text">
              <p className="activity-title">Unlocked Level 5</p>
              <p className="activity-time">3 days ago</p>
            </div>
            <span className="activity-score">+50 pts</span>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Dashboard;
