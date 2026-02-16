import { useNavigate } from 'react-router-dom';
import { useAuth } from '../AuthContext';
import AuthLayout from '../components/AuthLayout';
import '../styles/Auth.css';

/**
 * RoleSelect - First page where user chooses Admin or User role
 */
function RoleSelect() {
  const navigate = useNavigate();
  const { setSelectedRole } = useAuth();

  const handleRoleSelect = (selectedRole) => {
    setSelectedRole(selectedRole);
    navigate('/auth/login');
  };

  return (
    <AuthLayout
      title="Welcome to Learning Platform"
      subtitle="Choose your role to continue"
    >
      <div className="role-selection">
        <button
          className="role-card user-role"
          onClick={() => handleRoleSelect('user')}
        >
          <span className="role-icon">👤</span>
          <span className="role-title">Student</span>
          <span className="role-description">Learn and play games</span>
        </button>

        <button
          className="role-card admin-role"
          onClick={() => handleRoleSelect('admin')}
        >
          <span className="role-icon">⚙️</span>
          <span className="role-title">Admin</span>
          <span className="role-description">Manage content & analytics</span>
        </button>
      </div>
    </AuthLayout>
  );
}

export default RoleSelect;
