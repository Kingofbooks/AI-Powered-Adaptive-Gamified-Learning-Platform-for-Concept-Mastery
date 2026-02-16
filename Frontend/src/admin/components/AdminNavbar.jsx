import { Link, useLocation } from 'react-router-dom';
import '../styles/AdminNavbar.css';

function AdminNavbar() {
  const location = useLocation();

  const isActive = (path) => location.pathname === path ? 'active' : '';

  return (
    <nav className="admin-navbar">
      <div className="admin-nav-container">
        <Link to="/admin" className="admin-nav-logo">
          <span className="admin-logo-icon">⚙️</span>
          <span className="admin-logo-text">Admin Panel</span>
        </Link>

        <div className="admin-nav-menu">
          <Link 
            to="/admin" 
            className={`admin-nav-link ${isActive('/admin')}`}
          >
            📊 Dashboard
          </Link>
          <Link 
            to="/admin/subjects" 
            className={`admin-nav-link ${isActive('/admin/subjects')}`}
          >
            📚 Subjects
          </Link>
          <Link 
            to="/admin/concepts" 
            className={`admin-nav-link ${isActive('/admin/concepts')}`}
          >
            💡 Concepts
          </Link>
          <Link 
            to="/admin/questions" 
            className={`admin-nav-link ${isActive('/admin/questions')}`}
          >
            ❓ Questions
          </Link>
          <Link 
            to="/admin/games" 
            className={`admin-nav-link ${isActive('/admin/games')}`}
          >
            🎮 Games
          </Link>
          <Link 
            to="/admin/users" 
            className={`admin-nav-link ${isActive('/admin/users')}`}
          >
            👥 Users
          </Link>
          <Link 
            to="/admin/analytics" 
            className={`admin-nav-link ${isActive('/admin/analytics')}`}
          >
            📈 Analytics
          </Link>
          <Link 
            to="/" 
            className="admin-nav-link admin-exit"
          >
            🏠 Back to App
          </Link>
        </div>
      </div>
    </nav>
  );
}

export default AdminNavbar;
