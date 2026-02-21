import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { useAuth } from '../AuthContext';
import AuthLayout from '../components/AuthLayout';
import AuthInput from '../components/AuthInput';
import '../styles/Auth.css';

/**
 * Login - User login page
 * Supports Student, Teacher, and Admin login
 */
function Login() {
  const navigate = useNavigate();
  const { role, loginWithEmail } = useAuth();
  const [formData, setFormData] = useState({ email: '', password: '' });
  const [errors, setErrors] = useState({});
  const [isLoading, setIsLoading] = useState(false);
  const [apiError, setApiError] = useState('');

  const handleChange = (e) => {
    const { name, value } = e.target;
    setFormData((prev) => ({ ...prev, [name]: value }));
    if (errors[name]) {
      setErrors((prev) => ({ ...prev, [name]: '' }));
    }
  };

  const validateForm = () => {
    const newErrors = {};
    if (!formData.email.trim()) {
      newErrors.email = 'Email is required';
    } else if (!/\S+@\S+\.\S+/.test(formData.email)) {
      newErrors.email = 'Email is invalid';
    }
    if (!formData.password) {
      newErrors.password = 'Password is required';
    }
    return newErrors;
  };

  const handleSubmit = async (e) => {
    e.preventDefault();
    const newErrors = validateForm();

    if (Object.keys(newErrors).length > 0) {
      setErrors(newErrors);
      return;
    }

    setIsLoading(true);
    setApiError('');

    try {
      const result = await loginWithEmail(
        formData.email,
        formData.password,
        role
      );

      if (result.success) {
        // Login successful - redirect based on role
        if (role === 'student') {
          navigate('/dashboard');
        } else if (role === 'teacher') {
          navigate('/teacher/dashboard');
        } else if (role === 'admin') {
          navigate('/admin');
        }
      } else {
        setApiError(result.error || 'Login failed');
      }
    } catch (error) {
      setApiError('An error occurred. Please try again.');
      console.error('Login error:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const roleLabel = {
    student: 'Student',
    teacher: 'Teacher',
    admin: 'Admin',
  }[role] || 'User';

  const roleIcon = {
    student: '👤',
    teacher: '👨‍🏫',
    admin: '⚙️',
  }[role] || '👤';

  return (
    <AuthLayout
      title="Sign In"
      subtitle={`${roleIcon} Login as ${roleLabel}`}
      footerText="Don't have an account?"
      footerLink={{ href: '/auth/register', text: 'Sign Up' }}
    >
      <form onSubmit={handleSubmit} className="auth-form">
        {apiError && <div className="alert alert-error">{apiError}</div>}

        <AuthInput
          type="email"
          label="Email"
          name="email"
          value={formData.email}
          onChange={handleChange}
          placeholder="your.email@example.com"
          error={errors.email}
          required
        />

        <AuthInput
          type="password"
          label="Password"
          name="password"
          value={formData.password}
          onChange={handleChange}
          placeholder="Enter your password"
          error={errors.password}
          required
        />

        <div className="form-remember-forgot">
          <label className="remember-checkbox">
            <input type="checkbox" defaultChecked />
            <span>Remember me</span>
          </label>
          <a href="/auth/forgot" className="forgot-link">
            Forgot Password?
          </a>
        </div>

        <button
          type="submit"
          className="auth-btn primary"
          disabled={isLoading}
        >
          {isLoading ? 'Signing in...' : 'Sign In'}
        </button>

        <div className="auth-divider">
          <span>or</span>
        </div>

        <button
          type="button"
          className="auth-btn secondary"
          onClick={() => {
            // Go back to role selection
            navigate('/auth/role');
          }}
        >
          Change Role
        </button>
      </form>
    </AuthLayout>
  );
}

export default Login;
