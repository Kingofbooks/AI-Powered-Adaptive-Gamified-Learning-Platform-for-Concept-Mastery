import { useState, useEffect } from 'react';
import * as adminApi from '../api/adminApi';
import AdminNavbar from '../components/AdminNavbar';
import Table from '../components/Table';
import AdminButton from '../components/AdminButton';
import '../styles/AdminPages.css';

function Users() {
  const [users, setUsers] = useState([]);
  const [loading, setLoading] = useState(true);
  const [selectedUser, setSelectedUser] = useState(null);
  const [userProgress, setUserProgress] = useState(null);

  useEffect(() => {
    fetchUsers();
  }, []);

  const fetchUsers = async () => {
    try {
      const data = await adminApi.getUsers();
      setUsers(data);
    } catch (error) {
      console.error('Error fetching users:', error);
    } finally {
      setLoading(false);
    }
  };

  const handleViewProgress = async (user) => {
    try {
      const progress = await adminApi.getUserProgress(user.id);
      setSelectedUser(user);
      setUserProgress(progress);
    } catch (error) {
      console.error('Error fetching user progress:', error);
    }
  };

  const columns = [
    { key: 'name', label: 'Name' },
    { key: 'email', label: 'Email' },
    { key: 'level', label: 'Level' },
    { key: 'accuracy', label: 'Accuracy (%)', render: (val) => val + '%' },
    { key: 'gamesPlayed', label: 'Games Played' },
    { 
      key: 'joinDate', 
      label: 'Join Date',
      render: (val) => new Date(val).toLocaleDateString()
    },
  ];

  return (
    <>
      <AdminNavbar />
      <div className="admin-container">
        <div className="admin-page-header">
          <h1>👥 Manage Users</h1>
        </div>

        <Table
          columns={columns}
          data={users}
          isLoading={loading}
          onEdit={handleViewProgress}
        />

        {selectedUser && userProgress && (
          <div className="user-progress-modal">
            <div className="modal-overlay" onClick={() => setSelectedUser(null)}></div>
            <div className="user-progress-card">
              <button 
                className="close-btn"
                onClick={() => setSelectedUser(null)}
              >
                ×
              </button>
              <h2>{selectedUser.name}'s Progress</h2>
              
              <div className="progress-section">
                <h3>📊 Statistics</h3>
                <div className="progress-grid">
                  <div className="progress-item">
                    <span className="progress-label">Level:</span>
                    <span className="progress-value">{userProgress.level}</span>
                  </div>
                  <div className="progress-item">
                    <span className="progress-label">Accuracy:</span>
                    <span className="progress-value">{userProgress.accuracy}%</span>
                  </div>
                  <div className="progress-item">
                    <span className="progress-label">Games Played:</span>
                    <span className="progress-value">{userProgress.totalGamesPlayed}</span>
                  </div>
                </div>
              </div>

              <div className="progress-section">
                <h3>💪 Strong Topics</h3>
                <ul className="topics-list">
                  {userProgress.strongTopics?.map((topic, idx) => (
                    <li key={idx}>✅ {topic}</li>
                  ))}
                </ul>
              </div>

              <div className="progress-section">
                <h3>📌 Weak Topics</h3>
                <ul className="topics-list">
                  {userProgress.weakTopics?.map((topic, idx) => (
                    <li key={idx}>⚠️ {topic}</li>
                  ))}
                </ul>
              </div>

              <div className="progress-section">
                <h3>📅 Recent Activity</h3>
                <div className="activity-list">
                  {userProgress.recentActivity?.map((activity, idx) => (
                    <div key={idx} className="activity-item">
                      <span className="activity-date">{activity.date}</span>
                      <span className="activity-game">{activity.game}</span>
                      <span className="activity-score">Score: {activity.score}</span>
                    </div>
                  ))}
                </div>
              </div>
            </div>
          </div>
        )}
      </div>
    </>
  );
}

export default Users;
