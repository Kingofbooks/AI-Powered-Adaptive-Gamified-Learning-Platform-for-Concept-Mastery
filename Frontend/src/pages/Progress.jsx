import { MOCK_PROGRESS } from '../utils/constants';
import '../styles/Pages.css';

/**
 * Progress page - shows user statistics and achievements
 */
const Progress = () => {
  const {
    totalGamesPlayed,
    accuracy,
    currentLevel,
    subjectProgress,
  } = MOCK_PROGRESS;

  return (
    <div className="page progress-page">
      <div className="page-header">
        <h1>📊 Your Progress</h1>
        <p>Track your learning journey and achievements</p>
      </div>

      <div className="progress-overview">
        <div className="overview-card games-played">
          <div className="overview-value">{totalGamesPlayed}</div>
          <div className="overview-label">Games Played</div>
        </div>
        <div className="overview-card accuracy-card">
          <div className="overview-value">{accuracy}%</div>
          <div className="overview-label">Accuracy</div>
        </div>
        <div className="overview-card level-card">
          <div className="overview-value">Level {currentLevel}</div>
          <div className="overview-label">Current Level</div>
        </div>
      </div>

      <div className="subject-progress-section">
        <h2>Progress by Subject</h2>
        <div className="subject-progress-grid">
          {Object.entries(subjectProgress).map(([subject, progress]) => (
            <div key={subject} className="subject-progress-card">
              <h3>{subject}</h3>
              
              <div className="progress-item">
                <label>Games Played</label>
                <div className="progress-bar">
                  <div 
                    className="progress-fill" 
                    style={{ width: `${(progress.played / 20) * 100}%` }}
                  ></div>
                </div>
                <span className="progress-value">{progress.played}</span>
              </div>

              <div className="progress-item">
                <label>Accuracy</label>
                <div className="progress-bar">
                  <div 
                    className="progress-fill accurate"
                    style={{ width: `${progress.accuracy}%` }}
                  ></div>
                </div>
                <span className="progress-value">{progress.accuracy}%</span>
              </div>

              <div className="progress-item">
                <label>Level</label>
                <div className="level-badges">
                  {Array.from({ length: 7 }).map((_, i) => (
                    <div 
                      key={i}
                      className={`level-badge ${i < progress.level ? 'active' : ''}`}
                    >
                      {i + 1}
                    </div>
                  ))}
                </div>
              </div>
            </div>
          ))}
        </div>
      </div>

      <div className="achievements-section">
        <h2>🏆 Achievements</h2>
        <div className="achievements-grid">
          <div className="achievement-card unlocked">
            <div className="achievement-icon">🎓</div>
            <h4>First Steps</h4>
            <p>Complete 5 games</p>
          </div>
          <div className="achievement-card unlocked">
            <div className="achievement-icon">⚡</div>
            <h4>Speed Learner</h4>
            <p>80%+ accuracy achieved</p>
          </div>
          <div className="achievement-card unlocked">
            <div className="achievement-icon">🔥</div>
            <h4>On Fire</h4>
            <p>3-day streak</p>
          </div>
          <div className="achievement-card locked">
            <div className="achievement-icon">👑</div>
            <h4>Master</h4>
            <p>Reach level 10</p>
          </div>
          <div className="achievement-card locked">
            <div className="achievement-icon">🌟</div>
            <h4>Expert</h4>
            <p>100 games completed</p>
          </div>
          <div className="achievement-card locked">
            <div className="achievement-icon">💎</div>
            <h4>Perfect Score</h4>
            <p>100% accuracy once</p>
          </div>
        </div>
      </div>

      <div className="tips-section">
        <h2>💡 Tips for Better Learning</h2>
        <ul className="tips-list">
          <li>Practice consistently - aim for at least 5 games per day</li>
          <li>Focus on weak areas - revisit concepts with low accuracy</li>
          <li>Try different game types - variety helps reinforce learning</li>
          <li>Challenge yourself - select harder difficulties as you progress</li>
          <li>Track your streaks - maintain consistency for better retention</li>
        </ul>
      </div>
    </div>
  );
};

export default Progress;
