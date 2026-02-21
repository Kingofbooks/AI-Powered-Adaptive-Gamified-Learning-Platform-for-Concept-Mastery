import { createContext, useContext, useState, useEffect } from 'react';
import * as authService from './authService';

// Create Auth Context
const AuthContext = createContext();

/**
 * Auth Provider Component
 * Wraps the app and provides auth state and functions
 * Supports Student, Teacher, and Admin roles
 */
export function AuthProvider({ children }) {
  const [user, setUser] = useState(null);
  const [role, setRole] = useState(null); // 'student', 'teacher', 'admin'
  const [isAuthenticated, setIsAuthenticated] = useState(false);
  const [isLoading, setIsLoading] = useState(true);
  const [token, setToken] = useState(null);
  const [error, setError] = useState(null);

  // Initialize auth state from localStorage on mount
  useEffect(() => {
    const initializeAuth = async () => {
      try {
        const savedToken = authService.getAuthToken();
        const savedUser = authService.getStoredUser();

        if (savedToken && savedUser) {
          // Verify token is still valid
          const verification = await authService.verifyToken(savedToken);
          
          if (verification.valid) {
            setToken(savedToken);
            setUser(savedUser);
            setRole(savedUser.role);
            setIsAuthenticated(true);
            setError(null);
          } else {
            // Clear invalid token
            authService.clearAuth();
            setIsAuthenticated(false);
          }
        }
      } catch (err) {
        console.error('Error initializing auth:', err);
        authService.clearAuth();
        setIsAuthenticated(false);
      } finally {
        setIsLoading(false);
      }
    };

    initializeAuth();
  }, []);

  /**
   * Login user
   * Supports both email/password and token-based login
   * @param {object} loginData - Can be { user, role, token } or { token, user }
   */
  const login = (loginData) => {
    const userData = loginData.user || loginData;
    const userRole = userData.role || loginData.role;
    const authToken = loginData.token || loginData.access_token;

    setUser(userData);
    setRole(userRole);
    setToken(authToken);
    setIsAuthenticated(true);
    setError(null);

    // Save to localStorage
    authService.setAuthToken(authToken, userData);
  };

  /**
   * Logout user
   */
  const logout = () => {
    setUser(null);
    setRole(null);
    setToken(null);
    setIsAuthenticated(false);
    setError(null);

    // Clear localStorage
    authService.clearAuth();
  };

  /**
   * Register a new user
   * @param {object} formData - Registration form data
   * @param {string} userRole - 'student' or 'teacher'
   * @returns {object} { success, error }
   */
  const register = async (formData, userRole) => {
    try {
      setError(null);
      const result = await authService.register(formData, userRole);

      if (result.success) {
        // Auto-login after registration
        login({
          user: result.user,
          token: result.token,
        });
        return { success: true };
      } else {
        setError(result.error);
        return { success: false, error: result.error };
      }
    } catch (err) {
      const errorMsg = err.message || 'Registration failed';
      setError(errorMsg);
      return { success: false, error: errorMsg };
    }
  };

  /**
   * Login user with email and password
   * @param {string} email
   * @param {string} password
   * @param {string} userRole - 'student', 'teacher', or 'admin'
   * @returns {object} { success, error }
   */
  const loginWithEmail = async (email, password, userRole) => {
    try {
      setError(null);
      const result = await authService.login(email, password, userRole);

      if (result.success) {
        login({
          user: result.user,
          token: result.token,
        });
        return { success: true };
      } else {
        setError(result.error);
        return { success: false, error: result.error };
      }
    } catch (err) {
      const errorMsg = err.message || 'Login failed';
      setError(errorMsg);
      return { success: false, error: errorMsg };
    }
  };

  /**
   * Set selected role (for role selection page)
   * @param {string} selectedRole - 'student', 'teacher', 'user', or 'admin'
   */
  const setSelectedRole = (selectedRole) => {
    // Normalize role: 'user' -> 'student'
    const normalizedRole = selectedRole === 'user' ? 'student' : selectedRole;
    setRole(normalizedRole);
  };

  /**
   * Update user profile
   * @param {object} updatedUserData
   */
  const updateUser = (updatedUserData) => {
    const newUserData = { ...user, ...updatedUserData };
    setUser(newUserData);
    authService.setAuthToken(token, newUserData);
  };

  /**
   * Check if user is authenticated
   */
  const isLoggedIn = () => {
    return isAuthenticated && !!token && !!user;
  };

  /**
   * Check if user has a specific role
   */
  const hasRole = (checkRole) => {
    // Normalize role check
    const normalizedRole = checkRole === 'user' ? 'student' : checkRole;
    return role === normalizedRole;
  };

  const value = {
    user,
    role,
    token,
    error,
    isAuthenticated,
    isLoading,
    login,
    logout,
    register,
    loginWithEmail,
    setSelectedRole,
    updateUser,
    isLoggedIn,
    hasRole,
  };

  return (
    <AuthContext.Provider value={value}>
      {children}
    </AuthContext.Provider>
  );
}

/**
 * Hook to use auth context
 * @returns {object} auth context with all auth methods
 */
export function useAuth() {
  const context = useContext(AuthContext);
  if (!context) {
    throw new Error('useAuth must be used within AuthProvider');
  }
  return context;
}

export default AuthContext;
