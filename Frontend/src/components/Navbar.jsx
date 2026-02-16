import { Link, useLocation } from 'react-router-dom';
import '../styles/Navbar.css';

/**
 * Navigation bar component - present on all pages
 */
const Navbar = () => {
  const location = useLocation();

  const isActive = (path) => location.pathname === path ? 'active' : '';

  return (
    <nav className="navbar">
      <div className="nav-container">
        <Link to="/" className="nav-logo">
          <span className="logo-icon">🎮</span>
          LearnGame
        </Link>

        <ul className="nav-menu">
          <li className="nav-item">
            <Link to="/" className={`nav-link ${isActive('/')}`}>
              Home
            </Link>
          </li>
          <li className="nav-item">
            <Link to="/dashboard" className={`nav-link ${isActive('/dashboard')}`}>
              Dashboard
            </Link>
          </li>
          <li className="nav-item">
            <Link to="/subjects" className={`nav-link ${isActive('/subjects')}`}>
              Subjects
            </Link>
          </li>
          <li className="nav-item">
            <Link to="/progress" className={`nav-link ${isActive('/progress')}`}>
              Progress
            </Link>
          </li>
        </ul>
      </div>
    </nav>
  );
};

export default Navbar;
