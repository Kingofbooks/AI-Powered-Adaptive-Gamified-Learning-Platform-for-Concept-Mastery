/**
 * BACKEND API CLIENT
 * JavaScript client library for frontend to communicate with integrated backend
 * 
 * Usage:
 *   import { BackendClient } from './BackendClient'
 *   const api = new BackendClient("http://localhost:8000")
 *   const user = await api.registerUser("john_doe", "john@example.com", "password")
 */

class BackendClient {
    constructor(baseUrl = "http://localhost:8000") {
        this.baseUrl = baseUrl;
        this.userId = null;
        this.userToken = null;
    }

    /**
     * Helper function to make API calls
     */
    async request(endpoint, method = "GET", data = null, headers = {}) {
        try {
            const options = {
                method,
                headers: {
                    "Content-Type": "application/json",
                    ...headers
                }
            };

            if (data) {
                options.body = JSON.stringify(data);
            }

            const response = await fetch(`${this.baseUrl}${endpoint}`, options);

            if (!response.ok) {
                const error = await response.json();
                throw new Error(error.detail || `API Error: ${response.status}`);
            }

            return await response.json();
        } catch (error) {
            console.error(`❌ API Error: ${error.message}`);
            throw error;
        }
    }

    // ============================================
    // SYSTEM
    // ============================================

    /**
     * Check if backend is running
     */
    async healthCheck() {
        try {
            return await this.request("/health");
        } catch {
            return { status: "unhealthy" };
        }
    }

    // ============================================
    // USER MANAGEMENT
    // ============================================

    /**
     * Register a new user
     * @param {string} username - Unique username
     * @param {string} email - Valid email address
     * @param {string} password - Password
     * @param {string} level - Difficulty level (default: "beginner")
     */
    async registerUser(username, email, password, level = "beginner") {
        const response = await this.request("/users/register", "POST", {
            username,
            email,
            password,
            level
        });

        if (response.id) {
            this.userId = response.id;
        }

        return response;
    }

    /**
     * Login user
     * @param {string} username - Username
     * @param {string} password - Password
     */
    async loginUser(username, password) {
        const response = await this.request("/users/login", "POST", {
            username,
            password
        });

        if (response.id) {
            this.userId = response.id;
        }

        return response;
    }

    /**
     * Get current user profile
     */
    async getUserProfile(userId = this.userId) {
        if (!userId) throw new Error("User ID required");
        return await this.request(`/users/${userId}`);
    }

    /**
     * Get user statistics
     */
    async getUserStats(userId = this.userId) {
        if (!userId) throw new Error("User ID required");
        return await this.request(`/users/${userId}/stats`);
    }

    // ============================================
    // GAMES
    // ============================================

    /**
     * Generate a new game
     * @param {string} concept - Topic to learn
     * @param {string} gameType - "quiz", "puzzle", or "speed"
     * @param {string} difficulty - "beginner", "intermediate", or "advanced"
     * @param {number} numQuestions - Number of questions
     */
    async generateGame(concept, gameType = "quiz", difficulty = "intermediate", numQuestions = 5) {
        return await this.request("/games/generate", "POST", {
            concept,
            game_type: gameType,
            difficulty,
            num_questions: numQuestions,
            user_id: this.userId
        });
    }

    /**
     * Submit quiz answers
     * @param {string} gameId - Game ID
     * @param {Array} answers - Array of answer indices
     * @param {number} timeSpent - Time spent in seconds
     * @param {string} concept - Concept being tested
     */
    async submitQuiz(gameId, concept, answers, timeSpent) {
        if (!this.userId) throw new Error("User not logged in");

        return await this.request("/games/quiz/submit", "POST", {
            game_id: gameId,
            user_id: this.userId,
            concept,
            answers,
            time_spent: timeSpent
        });
    }

    /**
     * Submit puzzle solution
     */
    async submitPuzzle(gameId, concept, answer, timeSpent, hintsUsed = 0) {
        if (!this.userId) throw new Error("User not logged in");

        return await this.request("/games/puzzle/submit", "POST", {
            game_id: gameId,
            user_id: this.userId,
            concept,
            answer,
            time_spent: timeSpent,
            hints_used: hintsUsed
        });
    }

    /**
     * Submit speed challenge answers
     */
    async submitSpeedChallenge(gameId, concept, answers, timeSpent, completed = true) {
        if (!this.userId) throw new Error("User not logged in");

        return await this.request("/games/speed/submit", "POST", {
            game_id: gameId,
            user_id: this.userId,
            concept,
            answers,
            time_spent: timeSpent,
            completed
        });
    }

    /**
     * Get user's game results
     */
    async getGameResults(userId = this.userId, limit = 10) {
        if (!userId) throw new Error("User ID required");
        return await this.request(`/games/results/${userId}?limit=${limit}`);
    }

    // ============================================
    // PROGRESS & TRACKING
    // ============================================

    /**
     * Get user's progress across all concepts
     */
    async getUserProgress(userId = this.userId) {
        if (!userId) throw new Error("User ID required");
        return await this.request(`/progress/${userId}`);
    }

    /**
     * Get progress for a specific concept
     */
    async getConceptProgress(conceptName, userId = this.userId) {
        if (!userId) throw new Error("User ID required");
        return await this.request(`/progress/${userId}/concept/${conceptName}`);
    }

    // ============================================
    // DASHBOARD
    // ============================================

    /**
     * Get complete dashboard data
     * Returns: user, stats, recent results, progress, recommendations, learning paths
     */
    async getDashboard(userId = this.userId) {
        if (!userId) throw new Error("User ID required");
        return await this.request(`/dashboard/${userId}`);
    }

    /**
     * Get recommended next concepts
     */
    async getRecommendations(userId = this.userId) {
        if (!userId) throw new Error("User ID required");
        return await this.request(`/recommendations/${userId}`);
    }

    // ============================================
    // CONCEPTS
    // ============================================

    /**
     * Get all available concepts
     */
    async getAllConcepts(subject = null) {
        if (subject) {
            return await this.request(`/concepts?subject=${subject}`);
        }
        return await this.request("/concepts");
    }

    /**
     * Get detail about a specific concept
     */
    async getConceptDetail(conceptName, userId = this.userId) {
        const endpoint = userId
            ? `/concepts/${conceptName}?user_id=${userId}`
            : `/concepts/${conceptName}`;
        return await this.request(endpoint);
    }

    // ============================================
    // LEARNING PATHS
    // ============================================

    /**
     * Create a learning path
     */
    async createLearningPath(name, conceptIds, userId = this.userId) {
        if (!userId) throw new Error("User ID required");

        return await this.request("/learning-paths", "POST", {
            custom_name: name,
            concepts: conceptIds
        }, {
            "X-User-ID": userId
        });
    }

    /**
     * Get user's learning paths
     */
    async getLearningPaths(userId = this.userId) {
        if (!userId) throw new Error("User ID required");
        return await this.request(`/learning-paths/${userId}`);
    }

    // ============================================
    // UTILITY FUNCTIONS
    // ============================================

    /**
     * Calculate accuracy percentage
     */
    calculateAccuracy(correct, total) {
        if (total === 0) return 0;
        return Math.round((correct / total) * 100);
    }

    /**
     * Format time to MM:SS
     */
    formatTime(seconds) {
        const minutes = Math.floor(seconds / 60);
        const secs = seconds % 60;
        return `${minutes}:${secs < 10 ? '0' : ''}${secs}`;
    }

    /**
     * Calculate score with streak multiplier
     */
    calculateScore(basePoints, isCorrect, streak = 0) {
        if (!isCorrect) return 0;
        if (streak > 1) {
            return Math.floor(basePoints * (1 + (streak * 0.1)));
        }
        return basePoints;
    }
}

// ============================================
// REACT HOOK
// ============================================

/**
 * React hook for using BackendClient
 * Usage:
 *   const { registerUser, generateGame, loading, error } = useBackend()
 */
function useBackend(baseUrl = "http://localhost:8000") {
    const [loading, setLoading] = React.useState(false);
    const [error, setError] = React.useState(null);
    const [client] = React.useState(new BackendClient(baseUrl));

    const wrappedRequest = React.useCallback(
        async (method, ...args) => {
            setLoading(true);
            setError(null);
            try {
                return await method(...args);
            } catch (err) {
                setError(err.message);
                throw err;
            } finally {
                setLoading(false);
            }
        },
        []
    );

    return {
        // System
        healthCheck: () => wrappedRequest(() => client.healthCheck()),

        // User
        registerUser: (username, email, password, level) =>
            wrappedRequest(() => client.registerUser(username, email, password, level)),
        loginUser: (username, password) =>
            wrappedRequest(() => client.loginUser(username, password)),
        getUserProfile: (userId) =>
            wrappedRequest(() => client.getUserProfile(userId)),
        getUserStats: (userId) =>
            wrappedRequest(() => client.getUserStats(userId)),

        // Games
        generateGame: (concept, gameType, difficulty, numQuestions) =>
            wrappedRequest(() => client.generateGame(concept, gameType, difficulty, numQuestions)),
        submitQuiz: (gameId, concept, answers, timeSpent) =>
            wrappedRequest(() => client.submitQuiz(gameId, concept, answers, timeSpent)),
        submitPuzzle: (gameId, concept, answer, timeSpent, hintsUsed) =>
            wrappedRequest(() => client.submitPuzzle(gameId, concept, answer, timeSpent, hintsUsed)),
        submitSpeedChallenge: (gameId, concept, answers, timeSpent, completed) =>
            wrappedRequest(() => client.submitSpeedChallenge(gameId, concept, answers, timeSpent, completed)),
        getGameResults: (userId, limit) =>
            wrappedRequest(() => client.getGameResults(userId, limit)),

        // Progress
        getUserProgress: (userId) =>
            wrappedRequest(() => client.getUserProgress(userId)),
        getConceptProgress: (conceptName, userId) =>
            wrappedRequest(() => client.getConceptProgress(conceptName, userId)),

        // Dashboard
        getDashboard: (userId) =>
            wrappedRequest(() => client.getDashboard(userId)),
        getRecommendations: (userId) =>
            wrappedRequest(() => client.getRecommendations(userId)),

        // Concepts
        getAllConcepts: (subject) =>
            wrappedRequest(() => client.getAllConcepts(subject)),
        getConceptDetail: (conceptName, userId) =>
            wrappedRequest(() => client.getConceptDetail(conceptName, userId)),

        // Learning Paths
        createLearningPath: (name, conceptIds, userId) =>
            wrappedRequest(() => client.createLearningPath(name, conceptIds, userId)),
        getLearningPaths: (userId) =>
            wrappedRequest(() => client.getLearningPaths(userId)),

        // Utilities
        calculateAccuracy: (correct, total) =>
            client.calculateAccuracy(correct, total),
        formatTime: (seconds) =>
            client.formatTime(seconds),
        calculateScore: (basePoints, isCorrect, streak) =>
            client.calculateScore(basePoints, isCorrect, streak),

        // Client instance
        client,

        // State
        loading,
        error,
        userId: client.userId
    };
}

// ============================================
// CONTEXT PROVIDER
// ============================================

/**
 * React Context Provider for backend client
 * Usage:
 *   <BackendProvider baseUrl="http://localhost:8000">
 *     <App />
 *   </BackendProvider>
 */
const BackendContext = React.createContext();

function BackendProvider({ children, baseUrl = "http://localhost:8000" }) {
    const [client] = React.useState(() => new BackendClient(baseUrl));
    const [user, setUser] = React.useState(null);
    const [loading, setLoading] = React.useState(false);
    const [error, setError] = React.useState(null);

    const login = React.useCallback(async (username, password) => {
        setLoading(true);
        setError(null);
        try {
            const userData = await client.loginUser(username, password);
            setUser(userData);
            return userData;
        } catch (err) {
            setError(err.message);
            throw err;
        } finally {
            setLoading(false);
        }
    }, [client]);

    const register = React.useCallback(async (username, email, password, level) => {
        setLoading(true);
        setError(null);
        try {
            const userData = await client.registerUser(username, email, password, level);
            setUser(userData);
            return userData;
        } catch (err) {
            setError(err.message);
            throw err;
        } finally {
            setLoading(false);
        }
    }, [client]);

    const logout = React.useCallback(() => {
        setUser(null);
        client.userId = null;
    }, [client]);

    return (
        <BackendContext.Provider value={{ client, user, login, register, logout, loading, error }}>
            {children}
        </BackendContext.Provider>
    );
}

/**
 * Hook to use backend context
 */
function useBackendContext() {
    const context = React.useContext(BackendContext);
    if (!context) {
        throw new Error("useBackendContext must be used within BackendProvider");
    }
    return context;
}

export { BackendClient, useBackend, BackendProvider, useBackendContext };
