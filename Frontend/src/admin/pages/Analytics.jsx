import { useState, useEffect } from 'react';
import * as adminApi from '../api/adminApi';
import AdminNavbar from '../components/AdminNavbar';
import StatCard from '../components/StatCard';
import '../styles/Analytics.css';

function Analytics() {
  const [analytics, setAnalytics] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchAnalytics = async () => {
      try {
        const data = await adminApi.getAnalytics();
        setAnalytics(data);
      } catch (error) {
        console.error('Error fetching analytics:', error);
      } finally {
        setLoading(false);
      }
    };

    fetchAnalytics();
  }, []);

  if (loading) {
    return (
      <>
        <AdminNavbar />
        <div className="admin-container">Loading analytics...</div>
      </>
    );
  }

  return (
    <>
      <AdminNavbar />
      <div className="admin-container">
        <div className="admin-header">
          <h1>📈 Learning Analytics</h1>
          <p>Comprehensive insights into user learning patterns and platform metrics</p>
        </div>

        <div className="stats-grid">
          <StatCard
            icon="👥"
            label="Total Users"
            value={analytics?.totalUsers || 0}
            subtitle={`${analytics?.activeUsers || 0} active`}
            color="blue"
          />
          <StatCard
            icon="📊"
            label="Average Accuracy"
            value={analytics?.averageAccuracy || 0 + '%'}
            subtitle="Across all users"
            color="green"
          />
          <StatCard
            icon="⭐"
            label="Average Level"
            value={analytics?.averageLevel || 0}
            subtitle="User progression"
            color="purple"
          />
        </div>

        <div className="analytics-grid">
          <div className="analytics-card">
            <h2>🏆 Top Performers (5)</h2>
            <div className="performer-list">
              {analytics?.topPerformers?.map((user, idx) => (
                <div key={idx} className="performer-item">
                  <span className="performer-rank">#{idx + 1}</span>
                  <span className="performer-name">{user.name}</span>
                  <span className="performer-accuracy">{user.accuracy}%</span>
                </div>
              ))}
            </div>
          </div>

          <div className="analytics-card">
            <h2>🎮 Most Played Games</h2>
            <div className="game-chart">
              {analytics?.mostPlayedGames?.map((game, idx) => (
                <div key={idx} className="chart-item">
                  <span className="chart-label">{game.name}</span>
                  <div className="chart-bar-container">
                    <div 
                      className="chart-bar"
                      style={{ width: `${(game.plays / 250) * 100}%` }}
                    ></div>
                  </div>
                  <span className="chart-value">{game.plays}</span>
                </div>
              ))}
            </div>
          </div>
        </div>

        <div className="analytics-card full-width">
          <h2>⚠️ Weak Topics (Need Attention)</h2>
          <div className="weak-topics-chart">
            {analytics?.weakTopics?.map((topic, idx) => (
              <div key={idx} className="topic-bar-item">
                <span className="topic-bar-label">{topic.topic}</span>
                <div className="topic-bar-container">
                  <div 
                    className="topic-bar"
                    style={{ width: `${(topic.mentions / 50) * 100}%` }}
                  ></div>
                </div>
                <span className="topic-bar-value">{topic.mentions} mentions</span>
              </div>
            ))}
          </div>
        </div>

        <div className="analytics-card full-width">
          <h2>📅 User Growth Trend</h2>
          <div className="growth-chart">
            {analytics?.userGrowth?.map((data, idx) => (
              <div key={idx} className="growth-item">
                <div className="growth-bar-container">
                  <div 
                    className="growth-bar"
                    style={{ height: `${(data.users / 100) * 100}%` }}
                  ></div>
                </div>
                <span className="growth-label">{data.month}</span>
                <span className="growth-value">{data.users}</span>
              </div>
            ))}
          </div>
        </div>

        <div className="analytics-grid">
          <div className="analytics-card">
            <h2>📊 Platform Summary</h2>
            <div className="summary-list">
              <div className="summary-item">
                <span>Total Users:</span>
                <strong>{analytics?.totalUsers}</strong>
              </div>
              <div className="summary-item">
                <span>Active Users:</span>
                <strong>{analytics?.activeUsers}</strong>
              </div>
              <div className="summary-item">
                <span>Average Accuracy:</span>
                <strong>{analytics?.averageAccuracy}%</strong>
              </div>
              <div className="summary-item">
                <span>Average Level:</span>
                <strong>{analytics?.averageLevel}</strong>
              </div>
            </div>
          </div>

          <div className="analytics-card">
            <h2>🎯 Key Insights</h2>
            <ul className="insights-list">
              <li>✅ {Math.round((analytics?.activeUsers / analytics?.totalUsers) * 100)}% of users are active</li>
              <li>📈 User growth trend is positive</li>
              <li>🎮 Quiz Mode is the most popular game</li>
              <li>⚠️ Focus on improving weak topic teaching</li>
              <li>🏆 Creating more challenging content for top performers</li>
            </ul>
          </div>
        </div>
      </div>
    </>
  );
}

export default Analytics;
