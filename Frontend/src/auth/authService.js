// Mock Auth Service
// This simulates backend auth logic without a real server

// Mock user database
let users = [
  {
    id: 1,
    name: 'Admin User',
    email: 'admin@example.com',
    password: 'admin123', // In real app, would be hashed
    role: 'admin',
  },
  {
    id: 2,
    name: 'Student User',
    email: 'student@example.com',
    password: 'student123',
    role: 'user',
    college: 'ABC University',
    department: 'Computer Science',
    year: '2nd Year',
  },
];

/**
 * Login user with email and password
 * @param {string} email
 * @param {string} password
 * @param {string} role - 'admin' or 'user'
 * @returns {Promise} { success, user, error }
 */
export const login = (email, password, role) => {
  return new Promise((resolve) => {
    // Simulate network delay
    setTimeout(() => {
      const user = users.find(
        (u) => u.email === email && u.password === password && u.role === role
      );

      if (user) {
        // Remove password from response
        const { password, ...userWithoutPassword } = user;
        resolve({
          success: true,
          user: userWithoutPassword,
          token: 'mock-jwt-token-' + Date.now(),
        });
      } else {
        resolve({
          success: false,
          error: 'Invalid email, password, or role',
        });
      }
    }, 800);
  });
};

/**
 * Register new user
 * @param {object} formData - { name, email, password, confirmPassword, college?, department?, year? }
 * @param {string} role - 'admin' or 'user'
 * @returns {Promise} { success, message, error }
 */
export const register = (formData, role) => {
  return new Promise((resolve) => {
    setTimeout(() => {
      // Validation
      if (!formData.name || !formData.email || !formData.password) {
        resolve({
          success: false,
          error: 'All fields are required',
        });
        return;
      }

      if (formData.password !== formData.confirmPassword) {
        resolve({
          success: false,
          error: 'Passwords do not match',
        });
        return;
      }

      // Check if email already exists
      if (users.some((u) => u.email === formData.email)) {
        resolve({
          success: false,
          error: 'Email already registered',
        });
        return;
      }

      // Create new user
      const newUser = {
        id: users.length + 1,
        name: formData.name,
        email: formData.email,
        password: formData.password,
        role: role,
      };

      // Add student-specific fields
      if (role === 'user' && formData.college) {
        newUser.college = formData.college;
        newUser.department = formData.department;
        newUser.year = formData.year;
      }

      users.push(newUser);

      resolve({
        success: true,
        message: 'Registration successful! Please login.',
      });
    }, 800);
  });
};

/**
 * Forgot password - send reset link
 * @param {string} email
 * @returns {Promise} { success, message }
 */
export const forgotPassword = (email) => {
  return new Promise((resolve) => {
    setTimeout(() => {
      // In real app, would send email
      resolve({
        success: true,
        message: 'If this email exists, a reset link will be sent.',
      });
    }, 800);
  });
};

/**
 * Logout user
 * @returns {Promise} { success }
 */
export const logout = () => {
  return Promise.resolve({
    success: true,
  });
};

/**
 * Get current user from token (mock)
 * @param {string} token
 * @returns {Promise} { user } or null
 */
export const getCurrentUser = (token) => {
  return new Promise((resolve) => {
    setTimeout(() => {
      if (token) {
        // In a real app, would verify JWT token
        // For now, just return a mock user
        resolve({
          user: {
            id: 2,
            name: 'Student User',
            email: 'student@example.com',
            role: 'user',
          },
        });
      } else {
        resolve(null);
      }
    }, 300);
  });
};

export default {
  login,
  register,
  forgotPassword,
  logout,
  getCurrentUser,
};
