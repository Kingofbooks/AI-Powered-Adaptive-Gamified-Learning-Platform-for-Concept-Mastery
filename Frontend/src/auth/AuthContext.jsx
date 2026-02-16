import { createContext, useContext, useState, useEffect } from 'react';

// Create Auth Context
const AuthContext = createContext();

/**
 * Auth Provider Component
 * Wraps the app and provides auth state and functions
 */
export function AuthProvider({ children }) {
  const [user, setUser] = useState(null);
  const [role, setRole] = useState(null); // 'admin' or 'user'
  const [isAuthenticated, setIsAuthenticated] = useState(false);
  const [isLoading, setIsLoading] = useState(true);
  const [token, setToken] = useState(null);

  // Initialize auth state from localStorage on mount
  useEffect(() => {
    const savedToken = localStorage.getItem('authToken');
    const savedUser = localStorage.getItem('authUser');
    const savedRole = localStorage.getItem('authRole');

    if (savedToken && savedUser) {
      try {
        setToken(savedToken);
        setUser(JSON.parse(savedUser));
        setRole(savedRole);
        setIsAuthenticated(true);
      } catch (error) {
        console.error('Error loading auth state:', error);
        localStorage.clear();
      }
    }

    setIsLoading(false);
  }, []);

  /**
   * Login user
   * @param {object} loginData - { user, role, token }
   */
  const login = (loginData) => {
    setUser(loginData.user);
    setRole(loginData.role);
    setToken(loginData.token);
    setIsAuthenticated(true);

    // Save to localStorage
    localStorage.setItem('authToken', loginData.token);
    localStorage.setItem('authUser', JSON.stringify(loginData.user));
    localStorage.setItem('authRole', loginData.role);
  };

  /**
   * Logout user
   */
  const logout = () => {
    setUser(null);
    setRole(null);
    setToken(null);
    setIsAuthenticated(false);

    // Clear localStorage
    localStorage.removeItem('authToken');
    localStorage.removeItem('authUser');
    localStorage.removeItem('authRole');
  };

  /**
   * Set selected role (for role selection page)
   * @param {string} selectedRole - 'admin' or 'user'
   */
  const setSelectedRole = (selectedRole) => {
    setRole(selectedRole);
  };

  const value = {
    user,
    role,
    token,
    isAuthenticated,
    isLoading,
    login,
    logout,
    setSelectedRole,
  };

  return (
    <AuthContext.Provider value={value}>
      {children}
    </AuthContext.Provider>
  );
}

/**
 * Hook to use auth context
 * @returns {object} auth context
 */
export function useAuth() {
  const context = useContext(AuthContext);
  if (!context) {
    throw new Error('useAuth must be used within AuthProvider');
  }
  return context;
}

export default AuthContext;
