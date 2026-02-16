import { useNavigate } from 'react-router-dom';
import Button from '../components/Button';
import '../styles/Pages.css';

/**
 * Home page - landing page of the application
 */
const Home = () => {
  const navigate = useNavigate();

  return (
    <div className="page home-page">
      <div className="hero-section">
        <div className="hero-content">
          <h1 className="hero-title">
            Master Skills Through <span className="gradient-text">Interactive Games</span>
          </h1>
          <p className="hero-subtitle">
            Learn programming concepts, data structures, databases, and more through engaging game formats. Gamified learning for better retention and faster mastery.
          </p>

          <div className="hero-cta">
            <Button 
              onClick={() => navigate('/dashboard')} 
              variant="primary"
              size="large"
            >
              Get Started
            </Button>
            <Button 
              onClick={() => navigate('/subjects')} 
              variant="secondary"
              size="large"
            >
              Explore Subjects
            </Button>
          </div>
        </div>

        <div className="hero-stats">
          <div className="stat-card">
            <div className="stat-number">1000+</div>
            <div className="stat-label">Questions</div>
          </div>
          <div className="stat-card">
            <div className="stat-number">4</div>
            <div className="stat-label">Subjects</div>
          </div>
          <div className="stat-card">
            <div className="stat-number">3</div>
            <div className="stat-label">Game Types</div>
          </div>
        </div>
      </div>

      <div className="features-section">
        <h2>Why LearnGame?</h2>
        <div className="features-grid">
          <div className="feature-card">
            <div className="feature-icon">🎯</div>
            <h3>Targeted Learning</h3>
            <p>Focus on specific concepts with curated questions</p>
          </div>
          <div className="feature-card">
            <div className="feature-icon">⚡</div>
            <h3>Fast Progress</h3>
            <p>Adaptive difficulty that grows with your skills</p>
          </div>
          <div className="feature-card">
            <div className="feature-icon">📊</div>
            <h3>Track Performance</h3>
            <p>Visual progress tracking and detailed analytics</p>
          </div>
          <div className="feature-card">
            <div className="feature-icon">🏆</div>
            <h3>Earn Rewards</h3>
            <p>Unlock badges and climb the leaderboard</p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default Home;
