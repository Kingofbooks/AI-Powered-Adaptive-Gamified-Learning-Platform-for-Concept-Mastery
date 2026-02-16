// Sample subjects for the platform
export const SUBJECTS = [
  { id: 1, name: 'Java', description: 'Master OOP and Java fundamentals', icon: '☕' },
  { id: 2, name: 'DSA', description: 'Data Structures & Algorithms', icon: '📊' },
  { id: 3, name: 'SQL', description: 'Database management basics', icon: '🗄️' },
  { id: 4, name: 'Aptitude', description: 'Logical and quantitative reasoning', icon: '🧠' },
];

// Concepts mapped by subject
export const CONCEPTS_BY_SUBJECT = {
  Java: [
    { id: 1, name: 'Arrays', description: 'Learn about arrays and collections' },
    { id: 2, name: 'OOP', description: 'Object-Oriented Programming concepts' },
    { id: 3, name: 'Strings', description: 'String manipulation and operations' },
    { id: 4, name: 'Exception Handling', description: 'Error handling in Java' },
  ],
  DSA: [
    { id: 5, name: 'Linked Lists', description: 'Understand linked list structures' },
    { id: 6, name: 'Sorting Algorithms', description: 'Various sorting techniques' },
    { id: 7, name: 'Binary Search', description: 'Efficient search algorithms' },
    { id: 8, name: 'Dynamic Programming', description: 'Optimize recursive problems' },
  ],
  SQL: [
    { id: 9, name: 'SELECT Queries', description: 'Query data from databases' },
    { id: 10, name: 'JOINs', description: 'Combine data from multiple tables' },
    { id: 11, name: 'Aggregation', description: 'GROUP BY and aggregate functions' },
    { id: 12, name: 'Indexes', description: 'Optimize query performance' },
  ],
  Aptitude: [
    { id: 13, name: 'Numbers', description: 'Number systems and operations' },
    { id: 14, name: 'Geometry', description: 'Shapes, angles, and calculations' },
    { id: 15, name: 'Logic Puzzles', description: 'Reasoning and puzzles' },
    { id: 16, name: 'Percentages', description: 'Calculate and compare percentages' },
  ],
};

// Available games
export const GAMES = {
  QUIZ: 'quiz',
  SPEED: 'speed',
  PUZZLE: 'puzzle',
};

// Mock progress data
export const MOCK_PROGRESS = {
  totalGamesPlayed: 47,
  accuracy: 78.5,
  currentLevel: 8,
  subjectProgress: {
    Java: { played: 12, accuracy: 82, level: 5 },
    DSA: { played: 15, accuracy: 75, level: 7 },
    SQL: { played: 10, accuracy: 80, level: 6 },
    Aptitude: { played: 10, accuracy: 73, level: 4 },
  },
};
