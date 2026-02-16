/**
 * TEACHER AI AGENT - JavaScript Client Library
 * 
 * Easy-to-use client for consuming the Teacher AI API from React, Vue, or vanilla JS
 * 
 * Usage:
 *   import { TeacherAI } from './TeacherAIClient'
 *   const ai = new TeacherAI("http://localhost:8000")
 *   const quiz = await ai.generateQuiz("Photosynthesis", 5)
 */

class TeacherAI {
    constructor(apiUrl = "http://localhost:8000") {
        this.apiUrl = apiUrl;
        this.timeout = 60000; // 60 second timeout
    }

    /**
     * Helper function to make API calls
     */
    async request(endpoint, method = "GET", data = null) {
        try {
            const options = {
                method,
                headers: {
                    "Content-Type": "application/json",
                    "Accept": "application/json"
                }
            };

            if (data) {
                options.body = JSON.stringify(data);
            }

            const response = await fetch(`${this.apiUrl}${endpoint}`, options);
            
            if (!response.ok) {
                throw new Error(`API Error: ${response.status} ${response.statusText}`);
            }

            return await response.json();
        } catch (error) {
            console.error(`❌ ${error.message}`);
            throw error;
        }
    }

    // ============================================
    // STATUS & METADATA
    // ============================================

    /**
     * Check if API is running
     * @returns {Promise<boolean>}
     */
    async isHealthy() {
        try {
            const result = await this.request("/health");
            return result.status === "healthy";
        } catch {
            return false;
        }
    }

    /**
     * Get all available game types
     * @returns {Promise<Array>} Array of game types with descriptions
     */
    async getGameTypes() {
        const result = await this.request("/game-types");
        return result.available_games;
    }

    // ============================================
    // SUMMARIZATION
    // ============================================

    /**
     * Get a concept summary with key points
     * @param {string} concept - What to summarize (e.g., "Photosynthesis")
     * @param {string} level - "beginner", "intermediate", or "advanced"
     * @returns {Promise<Object>} Summary with title, key points, objectives
     */
    async summarizeConcept(concept, level = "intermediate") {
        const result = await this.request("/summarize", "POST", {
            concept,
            level
        });
        return result.data;
    }

    // ============================================
    // GAME GENERATION
    // ============================================

    /**
     * Generate a multiple-choice quiz
     * @param {string} concept - What to quiz about
     * @param {number} numQuestions - How many questions (1-20)
     * @param {string} level - "beginner", "intermediate", or "advanced"
     * @returns {Promise<Object>} Quiz with questions and scoring rules
     */
    async generateQuiz(concept, numQuestions = 5, level = "intermediate") {
        const result = await this.request("/generate-quiz", "POST", {
            concept,
            game_type: "quiz",
            num_questions: numQuestions,
            level
        });
        return result.data;
    }

    /**
     * Generate a logic puzzle
     * @param {string} concept - What the puzzle is about
     * @param {string} level - "beginner", "intermediate", or "advanced"
     * @returns {Promise<Object>} Puzzle with problem, answer, hints
     */
    async generatePuzzle(concept, level = "intermediate") {
        const result = await this.request("/generate-puzzle", "POST", {
            concept,
            level
        });
        return result.data;
    }

    /**
     * Generate a speed challenge (timed rapid-fire questions)
     * @param {string} concept - What to challenge about
     * @param {number} numQuestions - How many challenges (5-20)
     * @param {string} level - "beginner", "intermediate", or "advanced"
     * @returns {Promise<Object>} Speed challenge with time limits
     */
    async generateSpeedChallenge(concept, numQuestions = 10, level = "intermediate") {
        const result = await this.request("/generate-speed", "POST", {
            concept,
            game_type: "speed",
            num_questions: numQuestions,
            level
        });
        return result.data;
    }

    // ============================================
    // COMPLETE GAME DETAILS (RECOMMENDED)
    // ============================================

    /**
     * Get COMPLETE game details including:
     * - Learning objectives
     * - Format specification (JSON structure)
     * - Execution instructions
     * - Ready-to-use game data
     * 
     * This is the RECOMMENDED endpoint to use!
     * 
     * @param {string} concept - What to learn
     * @param {string} gameType - "quiz", "puzzle", or "speed"
     * @param {number} numQuestions - How many questions
     * @param {string} level - "beginner", "intermediate", or "advanced"
     * @returns {Promise<Object>} Complete game details with all info needed
     */
    async getGameDetails(
        concept,
        gameType = "quiz",
        numQuestions = 5,
        level = "intermediate"
    ) {
        const result = await this.request("/game", "POST", {
            concept,
            game_type: gameType,
            num_questions: numQuestions,
            level
        });
        return result;
    }

    // ============================================
    // COMPLETE LEARNING MODULES
    // ============================================

    /**
     * Generate a COMPLETE learning module with all game types:
     * 1. Concept Summary
     * 2. Quiz Game
     * 3. Puzzle Game
     * 4. Speed Challenge
     * 
     * Note: This takes 30-40 seconds to generate
     * 
     * @param {string} concept - The main concept to learn
     * @param {string} level - "beginner", "intermediate", or "advanced"
     * @returns {Promise<Object>} Complete module with all games
     */
    async generateCompleteModule(concept, level = "intermediate") {
        const result = await this.request("/generate-module", "POST", {
            concept,
            level
        });
        return result.data;
    }

    // ============================================
    // UTILITY FUNCTIONS
    // ============================================

    /**
     * Score a quiz answer
     * @param {boolean} isCorrect - Whether answer is correct
     * @param {number} basePoints - Points for correct answer
     * @param {number} streak - How many in a row correct
     * @returns {number} Points earned
     */
    scoreAnswer(isCorrect, basePoints = 10, streak = 0) {
        if (!isCorrect) return 0;
        
        // Apply streak multiplier
        if (streak > 1) {
            return Math.floor(basePoints * (1 + (streak * 0.1)));
        }
        
        return basePoints;
    }

    /**
     * Calculate accuracy percentage
     * @param {number} correct - Number of correct answers
     * @param {number} total - Total number of questions
     * @returns {number} Percentage (0-100)
     */
    calculateAccuracy(correct, total) {
        if (total === 0) return 0;
        return Math.round((correct / total) * 100);
    }

    /**
     * Format time to MM:SS format
     * @param {number} seconds - Total seconds
     * @returns {string} Formatted time string
     */
    formatTime(seconds) {
        const minutes = Math.floor(seconds / 60);
        const secs = seconds % 60;
        return `${minutes}:${secs < 10 ? '0' : ''}${secs}`;
    }
}

export { TeacherAI };
