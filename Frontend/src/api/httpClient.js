/**
 * HTTP Client for making API requests
 * Wraps fetch API with common configurations
 */

class HttpClient {
  constructor() {
    // Use either VITE env variable or default to localhost
    // For authentication, we use a different base URL (no /api path)
    this.baseURL = import.meta.env.VITE_APP_API_URL || 'http://localhost:8000/api';
    this.authBaseURL = import.meta.env.VITE_APP_AUTH_URL || 'http://localhost:8000';
    this.timeout = 30000;
  }

  /**
   * Get the base API URL for auth endpoints
   */
  getAuthBaseURL() {
    return this.authBaseURL;
  }

  /**
   * Make a GET request
   */
  async get(endpoint, options = {}) {
    return this.request(endpoint, { ...options, method: 'GET' });
  }

  /**
   * Make a POST request
   */
  async post(endpoint, data, options = {}) {
    return this.request(endpoint, {
      ...options,
      method: 'POST',
      body: JSON.stringify(data),
    });
  }

  /**
   * Make a PUT request
   */
  async put(endpoint, data, options = {}) {
    return this.request(endpoint, {
      ...options,
      method: 'PUT',
      body: JSON.stringify(data),
    });
  }

  /**
   * Make a general request
   */
  async request(endpoint, options = {}) {
    const url = `${this.baseURL}${endpoint}`;
    const defaultHeaders = {
      'Content-Type': 'application/json',
    };

    const fetchOptions = {
      ...options,
      headers: {
        ...defaultHeaders,
        ...options.headers,
      },
    };

    try {
      const controller = new AbortController();
      const timeoutId = setTimeout(() => controller.abort(), this.timeout);

      const response = await fetch(url, {
        ...fetchOptions,
        signal: controller.signal,
      });

      clearTimeout(timeoutId);

      if (!response.ok) {
        throw new Error(`HTTP Error: ${response.status}`);
      }

      return await response.json();
    } catch (error) {
      console.error('API Request Error:', error);
      throw error;
    }
  }
}

const httpClient = new HttpClient();

/**
 * Get the API URL for authentication endpoints
 */
export const getApiUrl = () => {
  return httpClient.getAuthBaseURL();
};

export default httpClient;
