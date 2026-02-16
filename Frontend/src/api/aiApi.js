import HttpClient from './httpClient';
import { GAMES } from '../utils/constants';

/**
 * AI API calls for game generation and recommendations
 */

/**
 * Get next game for a subject and concept
 * @param {string} subject - Subject name
 * @param {string} concept - Concept name
 * @returns {Promise} - Game data with type and metadata
 */
export const getNextGame = async (subject, concept) => {
  try {
    // In production, this would call the backend API
    // For now, return mock data
    return mockGetNextGame(subject, concept);
  } catch (error) {
    console.error('Error getting next game:', error);
    return mockGetNextGame(subject, concept);
  }
};

/**
 * Get personalized recommendations based on user progress
 * @param {string} subject - Subject name
 * @returns {Promise} - Recommended concepts
 */
export const getRecommendations = async (subject) => {
  try {
    return mockGetRecommendations(subject);
  } catch (error) {
    console.error('Error getting recommendations:', error);
    return mockGetRecommendations(subject);
  }
};

/**
 * Get difficulty level for a concept based on performance
 */
export const getAdaptiveQuestions = async (subject, concept, userLevel) => {
  try {
    return mockGetAdaptiveQuestions(subject, concept, userLevel);
  } catch (error) {
    console.error('Error getting adaptive questions:', error);
    return mockGetAdaptiveQuestions(subject, concept, userLevel);
  }
};

// ==================== MOCK FUNCTIONS ====================

/**
 * Mock implementation of getNextGame
 */
const mockGetNextGame = (subject, concept) => {
  const games = [GAMES.QUIZ, GAMES.SPEED, GAMES.PUZZLE];
  const randomGame = games[Math.floor(Math.random() * games.length)];

  return Promise.resolve({
    success: true,
    game: randomGame,
    subject,
    concept,
    difficulty: 'medium',
    questions: 5,
    timeLimit: 300,
    rewards: {
      points: 100,
      badge: null,
    },
    metadata: {
      generatedAt: new Date().toISOString(),
      gameType: randomGame,
      path: `/games/${randomGame}/index.html`,
    },
  });
};

/**
 * Mock implementation of getRecommendations
 */
const mockGetRecommendations = (subject) => {
  const recommendations = {
    Java: ['Improve OOP basics', 'Practice Exception Handling', 'Review String methods'],
    DSA: ['Master sorting algorithms', 'Practice Binary Search', 'Learn Dynamic Programming'],
    SQL: ['Practice JOINs', 'Learn aggregation functions', 'Understand indexing'],
    Aptitude: ['Improve number problems', 'Practice geometry', 'Learn probability'],
  };

  return Promise.resolve({
    success: true,
    subject,
    recommendations: recommendations[subject] || [],
    focusAreas: ['Weak topics', 'Implementation practice', 'Real-world scenarios'],
  });
};

/**
 * Mock implementation of getAdaptiveQuestions
 */
const mockGetAdaptiveQuestions = (subject, concept, userLevel = 1) => {
  const difficulties = { 1: 'easy', 2: 'medium', 3: 'hard', 4: 'expert' };
  const difficulty = difficulties[Math.min(userLevel, 4)] || 'medium';

  return Promise.resolve({
    success: true,
    subject,
    concept,
    difficulty,
    numberOfQuestions: 5,
    estimatedTime: 600,
    userLevel,
    nextLevelAt: 80,
  });
};
