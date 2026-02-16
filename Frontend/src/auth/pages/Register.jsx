import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { useAuth } from '../AuthContext';
import * as authService from '../authService';
import AuthLayout from '../components/AuthLayout';
import AuthInput from '../components/AuthInput';
import '../styles/Auth.css';

/**
 * Register - User registration page
 */
function Register() {
  const navigate = useNavigate();
  const { role } = useAuth();
  const [formData, setFormData] = useState({
    name: '',
    email: '',
    password: '',
    confirmPassword: '',
    college: '',
    department: '',
    year: '',
  });
  const [errors, setErrors] = useState({});
  const [isLoading, setIsLoading] = useState(false);
  const [apiError, setApiError] = useState('');
  const [success, setSuccess] = useState('');

  const handleChange = (e) => {
    const { name, value } = e.target;
    setFormData((prev) => ({ ...prev, [name]: value }));
    if (errors[name]) {
      setErrors((prev) => ({ ...prev, [name]: '' }));
    }
  };

  const validateForm = () => {
    const newErrors = {};

    if (!formData.name.trim()) {
      newErrors.name = 'Name is required';
    }

    if (!formData.email.trim()) {
      newErrors.email = 'Email is required';
    } else if (!/\S+@\S+\.\S+/.test(formData.email)) {
      newErrors.email = 'Email is invalid';
    }

    if (!formData.password) {
      newErrors.password = 'Password is required';
    } else if (formData.password.length < 6) {
      newErrors.password = 'Password must be at least 6 characters';
    }

    if (!formData.confirmPassword) {
      newErrors.confirmPassword = 'Please confirm your password';
    }

    if (formData.password !== formData.confirmPassword) {
      newErrors.confirmPassword = 'Passwords do not match';
    }

    // Student-specific validation
    if (role === 'user') {
      if (!formData.college.trim()) {
        newErrors.college = 'College name is required';
      }
      if (!formData.department.trim()) {
        newErrors.department = 'Department is required';
      }
      if (!formData.year.trim()) {
        newErrors.year = 'Year of study is required';
      }
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
    setSuccess('');

    try {
      const response = await authService.register(formData, role);

      if (response.success) {
        setSuccess(response.message);
        // Redirect to login after brief delay
        setTimeout(() => {
          navigate('/auth/login');
        }, 1500);
      } else {
        setApiError(response.error || 'Registration failed');
      }
    } catch (error) {
      setApiError('An error occurred. Please try again.');
      console.error('Register error:', error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <AuthLayout
      title="Create Account"
      subtitle={`Sign up as ${role === 'admin' ? 'Admin' : 'Student'}`}
      footerText="Already have an account?"
      footerLink={{ href: '/auth/login', text: 'Sign In' }}
    >
      <form onSubmit={handleSubmit} className="auth-form">
        {apiError && <div className="alert alert-error">{apiError}</div>}
        {success && <div className="alert alert-success">{success}</div>}

        <AuthInput
          label="Full Name"
          name="name"
          value={formData.name}
          onChange={handleChange}
          placeholder="John Doe"
          error={errors.name}
          required
        />

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

        {role === 'user' && (
          <>
            <AuthInput
              label="College/University"
              name="college"
              value={formData.college}
              onChange={handleChange}
              placeholder="e.g., ABC University"
              error={errors.college}
              required
            />

            <AuthInput
              label="Department"
              name="department"
              value={formData.department}
              onChange={handleChange}
              placeholder="e.g., Computer Science"
              error={errors.department}
              required
            />

            <AuthInput
              label="Year of Study"
              name="year"
              value={formData.year}
              onChange={handleChange}
              placeholder="e.g., 2nd Year"
              error={errors.year}
              required
            />
          </>
        )}

        <AuthInput
          type="password"
          label="Password"
          name="password"
          value={formData.password}
          onChange={handleChange}
          placeholder="At least 6 characters"
          error={errors.password}
          required
        />

        <AuthInput
          type="password"
          label="Confirm Password"
          name="confirmPassword"
          value={formData.confirmPassword}
          onChange={handleChange}
          placeholder="Re-enter your password"
          error={errors.confirmPassword}
          required
        />

        <div className="form-agreement">
          <label className="checkbox">
            <input type="checkbox" required />
            <span>I agree to the Terms & Conditions</span>
          </label>
        </div>

        <button
          type="submit"
          className="auth-btn primary"
          disabled={isLoading}
        >
          {isLoading ? 'Creating account...' : 'Sign Up'}
        </button>

        <button
          type="button"
          className="auth-btn secondary"
          onClick={() => navigate('/auth/login')}
        >
          Back to Sign In
        </button>
      </form>
    </AuthLayout>
  );
}

export default Register;
