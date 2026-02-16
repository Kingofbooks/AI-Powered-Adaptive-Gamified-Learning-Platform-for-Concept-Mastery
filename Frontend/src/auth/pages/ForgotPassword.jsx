import { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import * as authService from '../authService';
import AuthLayout from '../components/AuthLayout';
import AuthInput from '../components/AuthInput';
import '../styles/Auth.css';

/**
 * ForgotPassword - Password reset page
 */
function ForgotPassword() {
  const navigate = useNavigate();
  const [email, setEmail] = useState('');
  const [error, setError] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const [submitted, setSubmitted] = useState(false);

  const handleChange = (e) => {
    setEmail(e.target.value);
    if (error) {
      setError('');
    }
  };

  const validateEmail = () => {
    if (!email.trim()) {
      setError('Email is required');
      return false;
    }
    if (!/\S+@\S+\.\S+/.test(email)) {
      setError('Email is invalid');
      return false;
    }
    return true;
  };

  const handleSubmit = async (e) => {
    e.preventDefault();

    if (!validateEmail()) {
      return;
    }

    setIsLoading(true);
    setError('');

    try {
      const response = await authService.forgotPassword(email);

      if (response.success) {
        setSubmitted(true);
      } else {
        setError(response.error || 'Failed to send reset link');
      }
    } catch (error) {
      setError('An error occurred. Please try again.');
      console.error('Forgot password error:', error);
    } finally {
      setIsLoading(false);
    }
  };

  if (submitted) {
    return (
      <AuthLayout
        title="Check Your Email"
        subtitle="Password reset instructions sent"
      >
        <div className="forgot-success">
          <div className="success-icon">📧</div>
          <p className="success-message">
            If an account exists for <strong>{email}</strong>, you will receive
            a password reset link within 5 minutes.
          </p>
          <p className="success-hint">
            Check your spam folder if you don't see the email.
          </p>

          <button
            className="auth-btn primary"
            onClick={() => navigate('/auth/login')}
          >
            Back to Sign In
          </button>
        </div>
      </AuthLayout>
    );
  }

  return (
    <AuthLayout
      title="Forgot Password"
      subtitle="We'll help you reset your password"
      footerText="Remember your password?"
      footerLink={{ href: '/auth/login', text: 'Sign In' }}
    >
      <form onSubmit={handleSubmit} className="auth-form">
        {error && <div className="alert alert-error">{error}</div>}

        <p className="forgot-description">
          Enter your email address and we'll send you a link to reset your
          password.
        </p>

        <AuthInput
          type="email"
          label="Email Address"
          name="email"
          value={email}
          onChange={handleChange}
          placeholder="your.email@example.com"
          required
        />

        <button
          type="submit"
          className="auth-btn primary"
          disabled={isLoading}
        >
          {isLoading ? 'Sending...' : 'Send Reset Link'}
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

export default ForgotPassword;
