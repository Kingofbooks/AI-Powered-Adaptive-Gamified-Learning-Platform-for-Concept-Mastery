import { useState, useEffect } from 'react';
import * as adminApi from '../api/adminApi';
import AdminNavbar from '../components/AdminNavbar';
import Table from '../components/Table';
import AdminButton from '../components/AdminButton';
import '../styles/AdminPages.css';

function ManageGames() {
  const [games, setGames] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    fetchGames();
  }, []);

  const fetchGames = async () => {
    try {
      const data = await adminApi.getGames();
      setGames(data);
    } catch (error) {
      console.error('Error fetching games:', error);
    } finally {
      setLoading(false);
    }
  };

  const columns = [
    { key: 'name', label: 'Game Name' },
    { key: 'description', label: 'Description' },
    { key: 'template', label: 'Template' },
    { key: 'assignedTo', label: 'Assigned To' },
  ];

  return (
    <>
      <AdminNavbar />
      <div className="admin-container">
        <div className="admin-page-header">
          <h1>🎮 Manage Games</h1>
        </div>

        <div className="info-box">
          <p>Games are pre-configured templates (Quiz, Speed Challenge, Puzzle). Currently, the system uses these templates for all subjects and concepts.</p>
        </div>

        <Table
          columns={columns}
          data={games}
          isLoading={loading}
        />

        <div className="dashboard-card full-width">
          <h2>📋 Available Game Templates</h2>
          <div className="game-templates">
            <div className="template-card">
              <h3>🎯 Quiz Mode</h3>
              <p>Multiple choice questions with instant feedback</p>
              <ul>
                <li>Multiple choice format</li>
                <li>Instant correctness feedback</li>
                <li>Score tracking</li>
                <li>Best for comprehensive learning</li>
              </ul>
            </div>
            <div className="template-card">
              <h3>⚡ Speed Challenge</h3>
              <p>Answer quickly under time pressure</p>
              <ul>
                <li>Timed questions (30 seconds per question)</li>
                <li>Quick answering bonuses</li>
                <li>Leaderboard support</li>
                <li>Best for recall and speed</li>
              </ul>
            </div>
            <div className="template-card">
              <h3>🧩 Puzzle Game</h3>
              <p>Coding puzzles and problem-solving</p>
              <ul>
                <li>Code writing challenges</li>
                <li>Test case validation</li>
                <li>Difficulty levels</li>
                <li>Best for practical skills</li>
              </ul>
            </div>
          </div>
        </div>
      </div>
    </>
  );
}

export default ManageGames;
