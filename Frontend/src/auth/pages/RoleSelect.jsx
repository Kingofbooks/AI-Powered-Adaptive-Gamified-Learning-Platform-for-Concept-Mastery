import { useNavigate } from 'react-router-dom';
import { useAuth } from '../AuthContext';
import AuthLayout from '../components/AuthLayout';
import '../styles/Auth.css';

/**
 * RoleSelect - First page where user chooses their role
 * Supports Student, Teacher, and Admin
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
          className="role-card student-role"
          onClick={() => handleRoleSelect('student')}
          title="I am a student learning concepts"
        >
          <span className="role-icon">👤</span>
          <span className="role-title">Student</span>
          <span className="role-description">Learn and play games</span>
        </button>

        <button
          className="role-card teacher-role"
          onClick={() => handleRoleSelect('teacher')}
          title="I am a teacher creating content"
        >
          <span className="role-icon">👨‍🏫</span>
          <span className="role-title">Teacher</span>
          <span className="role-description">Create & manage content</span>
        </button>

        <button
          className="role-card admin-role"
          onClick={() => handleRoleSelect('admin')}
          title="I am an administrator"
        >
          <span className="role-icon">⚙️</span>
          <span className="role-title">Admin</span>
          <span className="role-description">Manage platform & analytics</span>
        </button>
      </div>
    </AuthLayout>
  );
}

export default RoleSelect;
