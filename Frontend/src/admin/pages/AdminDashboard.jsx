import { useState, useEffect } from 'react';
import * as adminApi from '../api/adminApi';
import AdminNavbar from '../components/AdminNavbar';
import StatCard from '../components/StatCard';
import '../styles/AdminDashboard.css';

function AdminDashboard() {
  const [stats, setStats] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchStats = async () => {
      try {
        const data = await adminApi.getAdminStats();
        setStats(data);
      } catch (error) {
        console.error('Error fetching stats:', error);
      } finally {
        setLoading(false);
      }
    };

    fetchStats();
  }, []);

  if (loading) {
    return <div className="admin-loading">Loading...</div>;
  }

  return (
    <>
      <AdminNavbar />
      <div className="admin-container">
        <div className="admin-header">
          <h1>📊 Admin Dashboard</h1>
          <p>Welcome to the Admin Panel - Manage your learning platform</p>
        </div>

        <div className="stats-grid">
          <StatCard
            icon="👥"
            label="Total Users"
            value={stats?.totalUsers || 0}
            subtitle={`${stats?.activeUsers || 0} active`}
            color="blue"
          />
          <StatCard
            icon="🎮"
            label="Total Games"
            value={stats?.totalGames || 0}
            subtitle={stats?.mostPlayedGame || 'Quiz in progress'}
            color="purple"
          />
          <StatCard
            icon="📚"
            label="Total Subjects"
            value={stats?.totalSubjects || 0}
            subtitle="Learning areas"
            color="green"
          />
          <StatCard
            icon="❓"
            label="Total Questions"
            value={stats?.totalQuestions || 0}
            subtitle="Available questions"
            color="orange"
          />
        </div>

        <div className="dashboard-grid">
          <div className="dashboard-card">
            <h2>📈 Platform Metrics</h2>
            <div className="metric-item">
              <span className="metric-label">Average User Accuracy:</span>
              <span className="metric-value">{stats?.averageAccuracy || 0}%</span>
            </div>
            <div className="metric-item">
              <span className="metric-label">Total Users:</span>
              <span className="metric-value">{stats?.totalUsers || 0}</span>
            </div>
            <div className="metric-item">
              <span className="metric-label">Active Users:</span>
              <span className="metric-value">{stats?.activeUsers || 0}</span>
            </div>
          </div>

          <div className="dashboard-card">
            <h2>🎯 Weak Topics (Need Attention)</h2>
            <ul className="weak-topics-list">
              {stats?.weakTopics?.map((topic, idx) => (
                <li key={idx}>
                  <span className="weak-topic-marker">⚠️</span>
                  {topic}
                </li>
              ))}
            </ul>
          </div>
        </div>

        <div className="dashboard-card full-width">
          <h2>🚀 Quick Actions</h2>
          <div className="quick-actions">
            <a href="/admin/subjects" className="action-link">
              <span className="action-icon">📚</span>
              <span>Manage Subjects</span>
            </a>
            <a href="/admin/concepts" className="action-link">
              <span className="action-icon">💡</span>
              <span>Manage Concepts</span>
            </a>
            <a href="/admin/questions" className="action-link">
              <span className="action-icon">❓</span>
              <span>Manage Questions</span>
            </a>
            <a href="/admin/games" className="action-link">
              <span className="action-icon">🎮</span>
              <span>Manage Games</span>
            </a>
            <a href="/admin/users" className="action-link">
              <span className="action-icon">👥</span>
              <span>View Users</span>
            </a>
            <a href="/admin/analytics" className="action-link">
              <span className="action-icon">📈</span>
              <span>View Analytics</span>
            </a>
          </div>
        </div>
      </div>
    </>
  );
}

export default AdminDashboard;
