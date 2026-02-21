/**
 * Authentication Service
 * Handles all authentication requests to the backend API
 * Supports both Student and Teacher login/registration
 */

import { getApiUrl } from '../api/httpClient';

// Get the API base URL
const API_BASE_URL = getApiUrl();

/**
 * Register a new user (Student or Teacher)
 * @param {object} formData - {name, email, username, password, role, college?, department?, year?, experience?, subjects?}
 * @param {string} role - 'student' or 'teacher'
 * @returns {Promise} { success, token, user, error }
 */
export const register = async (formData, role) => {
  try {
    // Normalize role
    const normalizedRole = role === 'user' ? 'student' : role;
    
    // Build request payload
    const payload = {
      name: formData.name,
      email: formData.email,
      username: formData.email.split('@')[0], // Use email prefix as username if not provided
      password: formData.password,
      role: normalizedRole,
    };

    // Add role-specific fields
    if (normalizedRole === 'student') {
      payload.college = formData.college || null;
      payload.department = formData.department || null;
      payload.year = formData.year || null;
    } else if (normalizedRole === 'teacher') {
      payload.experience = formData.experience || null;
      payload.subjects = formData.subjects || [];
    }

    const response = await fetch(`${API_BASE_URL}/auth/register`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(payload),
    });

    if (!response.ok) {
      const errorData = await response.json();
      return {
        success: false,
        error: errorData.detail || 'Registration failed',
      };
    }

    const data = await response.json();

    return {
      success: true,
      token: data.access_token,
      user: data.user,
      message: 'Registration successful!',
    };
  } catch (error) {
    console.error('Registration error:', error);
    return {
      success: false,
      error: 'Network error: ' + error.message,
    };
  }
};

/**
 * Login user with email and password
 * @param {string} email
 * @param {string} password
 * @param {string} role - 'student', 'teacher', or 'admin'
 * @returns {Promise} { success, token, user, error }
 */
export const login = async (email, password, role) => {
  try {
    // Normalize role
    const normalizedRole = role === 'user' ? 'student' : role;

    const payload = {
      email: email,
      password: password,
      role: normalizedRole,
    };

    const response = await fetch(`${API_BASE_URL}/auth/login`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(payload),
    });

    if (!response.ok) {
      const errorData = await response.json();
      return {
        success: false,
        error: errorData.detail || 'Login failed',
      };
    }

    const data = await response.json();

    return {
      success: true,
      token: data.access_token,
      user: data.user,
    };
  } catch (error) {
    console.error('Login error:', error);
    return {
      success: false,
      error: 'Network error: ' + error.message,
    };
  }
};

/**
 * Verify JWT token with backend
 * @param {string} token
 * @returns {Promise} { valid, user, error }
 */
export const verifyToken = async (token) => {
  try {
    if (!token) {
      return {
        valid: false,
        error: 'No token provided',
      };
    }

    const response = await fetch(`${API_BASE_URL}/auth/verify`, {
      method: 'GET',
      headers: {
        'Authorization': `Bearer ${token}`,
        'Content-Type': 'application/json',
      },
    });

    if (!response.ok) {
      return {
        valid: false,
        error: 'Token verification failed',
      };
    }

    const data = await response.json();

    return {
      valid: true,
      user: data,
    };
  } catch (error) {
    console.error('Token verification error:', error);
    return {
      valid: false,
      error: 'Network error: ' + error.message,
    };
  }
};

/**
 * Get current user profile from backend
 * @param {string} token
 * @returns {Promise} { user, error }
 */
export const getCurrentUser = async (token) => {
  try {
    if (!token) {
      return null;
    }

    const response = await fetch(`${API_BASE_URL}/users/me`, {
      method: 'GET',
      headers: {
        'Authorization': `Bearer ${token}`,
        'Content-Type': 'application/json',
      },
    });

    if (!response.ok) {
      console.warn('Failed to fetch current user');
      return null;
    }

    const user = await response.json();
    return user;
  } catch (error) {
    console.error('Get current user error:', error);
    return null;
  }
};

/**
 * Forgot password - request reset link
 * @param {string} email
 * @returns {Promise} { success, message, error }
 */
export const forgotPassword = async (email) => {
  try {
    // This would be implemented on the backend
    const response = await fetch(`${API_BASE_URL}/auth/forgot-password`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify({ email }),
    });

    if (!response.ok) {
      return {
        success: false,
        error: 'Failed to process reset request',
      };
    }

    return {
      success: true,
      message: 'If this email exists, a reset link will be sent',
    };
  } catch (error) {
    console.error('Forgot password error:', error);
    return {
      success: true, // Don't reveal if email exists
      message: 'If this email exists, a reset link will be sent',
    };
  }
};

/**
 * Logout user (client-side only)
 * @returns {Promise} { success }
 */
export const logout = async () => {
  try {
    // Client-side logout: clear token
    localStorage.removeItem('authToken');
    localStorage.removeItem('user');
    return {
      success: true,
    };
  } catch (error) {
    console.error('Logout error:', error);
    return {
      success: false,
      error: error.message,
    };
  }
};

/**
 * Set auth token in localStorage
 * @param {string} token
 * @param {object} user
 */
export const setAuthToken = (token, user) => {
  localStorage.setItem('authToken', token);
  localStorage.setItem('user', JSON.stringify(user));
};

/**
 * Get auth token from localStorage
 * @returns {string} token or null
 */
export const getAuthToken = () => {
  return localStorage.getItem('authToken');
};

/**
 * Get stored user from localStorage
 * @returns {object} user or null
 */
export const getStoredUser = () => {
  const user = localStorage.getItem('user');
  return user ? JSON.parse(user) : null;
};

/**
 * Clear auth data from localStorage
 */
export const clearAuth = () => {
  localStorage.removeItem('authToken');
  localStorage.removeItem('user');
};

export default {
  register,
  login,
  verifyToken,
  getCurrentUser,
  forgotPassword,
  logout,
  setAuthToken,
  getAuthToken,
  getStoredUser,
  clearAuth,
};
