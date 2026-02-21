import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { useAuth } from '../AuthContext';
import AuthLayout from '../components/AuthLayout';
import AuthInput from '../components/AuthInput';
import '../styles/Auth.css';

/**
 * Register - User registration page
 * Supports Student and Teacher registration
 */
function Register() {
  const navigate = useNavigate();
  const { role, register } = useAuth();
  const [formData, setFormData] = useState({
    name: '',
    email: '',
    password: '',
    confirmPassword: '',
    // Student fields
    college: '',
    department: '',
    year: '',
    // Teacher fields
    experience: '',
    subjects: '',
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
    if (role === 'student') {
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

    // Teacher-specific validation
    if (role === 'teacher') {
      if (!formData.experience.trim()) {
        newErrors.experience = 'Experience is required';
      }
      if (!formData.subjects.trim()) {
        newErrors.subjects = 'At least one subject is required';
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
      // Prepare payload for registration
      const payload = {
        name: formData.name,
        email: formData.email,
        password: formData.password,
      };

      // Add role-specific fields
      if (role === 'student') {
        payload.college = formData.college;
        payload.department = formData.department;
        payload.year = formData.year;
      } else if (role === 'teacher') {
        payload.experience = formData.experience;
        // Convert subjects string to array
        payload.subjects = formData.subjects
          .split(',')
          .map((s) => s.trim())
          .filter((s) => s);
      }

      const result = await register(payload, role);

      if (result.success) {
        setSuccess('Registration successful! Logging you in...');
        // Auto-login successful, redirect to dashboard
        setTimeout(() => {
          if (role === 'student') {
            navigate('/dashboard');
          } else if (role === 'teacher') {
            navigate('/teacher/dashboard');
          }
        }, 1500);
      } else {
        setApiError(result.error || 'Registration failed');
      }
    } catch (error) {
      setApiError('An error occurred. Please try again.');
      console.error('Register error:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const roleLabel = {
    student: 'Student',
    teacher: 'Teacher',
  }[role] || 'User';

  const roleIcon = {
    student: '👤',
    teacher: '👨‍🏫',
  }[role] || '👤';

  return (
    <AuthLayout
      title="Create Account"
      subtitle={`${roleIcon} Sign up as ${roleLabel}`}
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

        {role === 'student' && (
          <>
            <AuthInput
              label="College/University"
              name="college"
              value={formData.college}
              onChange={handleChange}
              placeholder="e.g., MIT"
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

        {role === 'teacher' && (
          <>
            <AuthInput
              label="Teaching Experience"
              name="experience"
              value={formData.experience}
              onChange={handleChange}
              placeholder="e.g., 5 years"
              error={errors.experience}
              required
            />

            <AuthInput
              label="Subjects (comma-separated)"
              name="subjects"
              value={formData.subjects}
              onChange={handleChange}
              placeholder="e.g., Mathematics, Physics"
              error={errors.subjects}
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
