// Mock data for admin dashboard
let subjects = [
  { id: 1, name: 'Java', description: 'Core Java Programming', questionCount: 45 },
  { id: 2, name: 'DSA', description: 'Data Structures and Algorithms', questionCount: 62 },
  { id: 3, name: 'SQL', description: 'Database Management', questionCount: 38 },
  { id: 4, name: 'Aptitude', description: 'Quantitative & Reasoning', questionCount: 52 },
  { id: 5, name: 'Web Development', description: 'Frontend and Backend', questionCount: 41 },
];

let concepts = [
  { id: 1, subjectId: 1, name: 'OOP Concepts', description: 'Classes, Objects, Inheritance' },
  { id: 2, subjectId: 1, name: 'Strings', description: 'String operations and methods' },
  { id: 3, subjectId: 1, name: 'Collections', description: 'Lists, Sets, Maps' },
  { id: 4, subjectId: 2, name: 'Arrays', description: 'Array operations and algorithms' },
  { id: 5, subjectId: 2, name: 'Sorting', description: 'Bubble, Quick, Merge Sort' },
  { id: 6, subjectId: 2, name: 'Searching', description: 'Linear and Binary Search' },
  { id: 7, subjectId: 3, name: 'SELECT Queries', description: 'Basic to advanced SELECT' },
  { id: 8, subjectId: 3, name: 'JOIN Operations', description: 'INNER, LEFT, RIGHT JOINs' },
  { id: 9, subjectId: 4, name: 'Time & Work', description: 'Time, work, and speed problems' },
  { id: 10, subjectId: 4, name: 'Probability', description: 'Basic probability concepts' },
];

let questions = [
  { id: 1, conceptId: 1, subjectId: 1, text: 'What is encapsulation?', difficulty: 'Medium', answered: 234 },
  { id: 2, conceptId: 1, subjectId: 1, text: 'Explain polymorphism in Java', difficulty: 'Hard', answered: 189 },
  { id: 3, conceptId: 2, subjectId: 1, text: 'How to reverse a string?', difficulty: 'Easy', answered: 456 },
  { id: 4, conceptId: 3, subjectId: 1, text: 'Difference between List and Set', difficulty: 'Medium', answered: 312 },
  { id: 5, conceptId: 4, subjectId: 2, text: 'What is an array?', difficulty: 'Easy', answered: 567 },
  { id: 6, conceptId: 5, subjectId: 2, text: 'Time complexity of QuickSort', difficulty: 'Hard', answered: 198 },
  { id: 7, conceptId: 6, subjectId: 2, text: 'Implement Binary Search', difficulty: 'Medium', answered: 289 },
  { id: 8, conceptId: 7, subjectId: 3, text: 'Write SELECT with GROUP BY', difficulty: 'Medium', answered: 276 },
  { id: 9, conceptId: 8, subjectId: 3, text: 'Explain INNER JOIN', difficulty: 'Medium', answered: 324 },
  { id: 10, conceptId: 9, subjectId: 4, text: 'If A works 5 days to complete a task...', difficulty: 'Easy', answered: 412 },
];

let users = [
  { id: 1, name: 'Ramesh Kumar', email: 'ramesh@example.com', level: 12, accuracy: 78, gamesPlayed: 45, joinDate: '2025-01-15' },
  { id: 2, name: 'Priya Sharma', email: 'priya@example.com', level: 8, accuracy: 65, gamesPlayed: 32, joinDate: '2025-02-08' },
  { id: 3, name: 'Arjun Singh', email: 'arjun@example.com', level: 15, accuracy: 82, gamesPlayed: 58, joinDate: '2024-12-20' },
  { id: 4, name: 'Sneha Patel', email: 'sneha@example.com', level: 10, accuracy: 71, gamesPlayed: 41, joinDate: '2025-01-28' },
  { id: 5, name: 'Vikram Desai', email: 'vikram@example.com', level: 6, accuracy: 58, gamesPlayed: 22, joinDate: '2025-02-10' },
  { id: 6, name: 'Ananya Roy', email: 'ananya@example.com', level: 14, accuracy: 79, gamesPlayed: 52, joinDate: '2025-01-05' },
  { id: 7, name: 'Nitin Verma', email: 'nitin@example.com', level: 9, accuracy: 68, gamesPlayed: 36, joinDate: '2025-02-02' },
  { id: 8, name: 'Divya Iyer', email: 'divya@example.com', level: 11, accuracy: 75, gamesPlayed: 48, joinDate: '2025-01-20' },
];

let games = [
  { id: 1, name: 'Quiz Mode', description: 'Multiple choice questions', template: 'quiz', assignedTo: 8, subjects: [1, 2, 3, 4, 5] },
  { id: 2, name: 'Speed Challenge', description: 'Answer quickly for points', template: 'speed', assignedTo: 6, subjects: [1, 2, 3] },
  { id: 3, name: 'Puzzle Game', description: 'Solve coding puzzles', template: 'puzzle', assignedTo: 5, subjects: [2, 5] },
];

// ========================
// ADMIN STATS/ANALYTICS
// ========================

export const getAdminStats = () => {
  return Promise.resolve({
    totalUsers: users.length,
    totalGames: games.length,
    totalSubjects: subjects.length,
    totalQuestions: questions.length,
    activeUsers: Math.floor(users.length * 0.7),
    averageAccuracy: (users.reduce((sum, u) => sum + u.accuracy, 0) / users.length).toFixed(1),
    mostPlayedGame: 'Quiz Mode',
    weakTopics: ['Polymorphism', 'Time Complexity', 'Complex JOINs'],
  });
};

// ========================
// SUBJECTS MANAGEMENT
// ========================

export const getSubjects = () => {
  return Promise.resolve([...subjects]);
};

export const addSubject = (subjectData) => {
  const newSubject = {
    id: Math.max(...subjects.map(s => s.id), 0) + 1,
    ...subjectData,
    questionCount: 0,
  };
  subjects.push(newSubject);
  return Promise.resolve(newSubject);
};

export const updateSubject = (id, subjectData) => {
  const index = subjects.findIndex(s => s.id === id);
  if (index !== -1) {
    subjects[index] = { ...subjects[index], ...subjectData };
    return Promise.resolve(subjects[index]);
  }
  return Promise.reject(new Error('Subject not found'));
};

export const deleteSubject = (id) => {
  const initialLength = subjects.length;
  subjects = subjects.filter(s => s.id !== id);
  if (subjects.length < initialLength) {
    return Promise.resolve({ success: true });
  }
  return Promise.reject(new Error('Subject not found'));
};

// ========================
// CONCEPTS MANAGEMENT
// ========================

export const getConceptsBySubject = (subjectId) => {
  return Promise.resolve(concepts.filter(c => c.subjectId === subjectId));
};

export const addConcept = (conceptData) => {
  const newConcept = {
    id: Math.max(...concepts.map(c => c.id), 0) + 1,
    ...conceptData,
  };
  concepts.push(newConcept);
  return Promise.resolve(newConcept);
};

export const updateConcept = (id, conceptData) => {
  const index = concepts.findIndex(c => c.id === id);
  if (index !== -1) {
    concepts[index] = { ...concepts[index], ...conceptData };
    return Promise.resolve(concepts[index]);
  }
  return Promise.reject(new Error('Concept not found'));
};

export const deleteConcept = (id) => {
  const initialLength = concepts.length;
  concepts = concepts.filter(c => c.id !== id);
  if (concepts.length < initialLength) {
    return Promise.resolve({ success: true });
  }
  return Promise.reject(new Error('Concept not found'));
};

// ========================
// QUESTIONS MANAGEMENT
// ========================

export const getQuestionsByConceptId = (conceptId) => {
  return Promise.resolve(questions.filter(q => q.conceptId === conceptId));
};

export const getQuestionsBySubjectId = (subjectId) => {
  return Promise.resolve(questions.filter(q => q.subjectId === subjectId));
};

export const addQuestion = (questionData) => {
  const newQuestion = {
    id: Math.max(...questions.map(q => q.id), 0) + 1,
    ...questionData,
    answered: 0,
  };
  questions.push(newQuestion);
  return Promise.resolve(newQuestion);
};

export const updateQuestion = (id, questionData) => {
  const index = questions.findIndex(q => q.id === id);
  if (index !== -1) {
    questions[index] = { ...questions[index], ...questionData };
    return Promise.resolve(questions[index]);
  }
  return Promise.reject(new Error('Question not found'));
};

export const deleteQuestion = (id) => {
  const initialLength = questions.length;
  questions = questions.filter(q => q.id !== id);
  if (questions.length < initialLength) {
    return Promise.resolve({ success: true });
  }
  return Promise.reject(new Error('Question not found'));
};

// ========================
// GAMES MANAGEMENT
// ========================

export const getGames = () => {
  return Promise.resolve([...games]);
};

export const addGame = (gameData) => {
  const newGame = {
    id: Math.max(...games.map(g => g.id), 0) + 1,
    ...gameData,
    assignedTo: 0,
  };
  games.push(newGame);
  return Promise.resolve(newGame);
};

export const updateGame = (id, gameData) => {
  const index = games.findIndex(g => g.id === id);
  if (index !== -1) {
    games[index] = { ...games[index], ...gameData };
    return Promise.resolve(games[index]);
  }
  return Promise.reject(new Error('Game not found'));
};

export const deleteGame = (id) => {
  const initialLength = games.length;
  games = games.filter(g => g.id !== id);
  if (games.length < initialLength) {
    return Promise.resolve({ success: true });
  }
  return Promise.reject(new Error('Game not found'));
};

// ========================
// USERS MANAGEMENT
// ========================

export const getUsers = () => {
  return Promise.resolve([...users]);
};

export const getUserById = (id) => {
  const user = users.find(u => u.id === id);
  if (user) {
    return Promise.resolve(user);
  }
  return Promise.reject(new Error('User not found'));
};

export const getUserProgress = (userId) => {
  return Promise.resolve({
    userId,
    level: 12,
    accuracy: 78,
    totalGamesPlayed: 45,
    strongTopics: ['Arrays', 'Strings', 'SELECT Queries'],
    weakTopics: ['Polymorphism', 'Complex Algorithms'],
    recentActivity: [
      { date: '2025-02-15', game: 'Quiz', score: 85 },
      { date: '2025-02-14', game: 'Speed', score: 72 },
      { date: '2025-02-13', game: 'Puzzle', score: 90 },
    ],
  });
};

// ========================
// ANALYTICS
// ========================

export const getAnalytics = () => {
  return Promise.resolve({
    totalUsers: users.length,
    activeUsers: Math.floor(users.length * 0.7),
    averageAccuracy: (users.reduce((sum, u) => sum + u.accuracy, 0) / users.length).toFixed(1),
    averageLevel: (users.reduce((sum, u) => sum + u.level, 0) / users.length).toFixed(1),
    topPerformers: users.slice().sort((a, b) => b.accuracy - a.accuracy).slice(0, 5),
    mostPlayedGames: [
      { name: 'Quiz Mode', plays: 245 },
      { name: 'Speed Challenge', plays: 189 },
      { name: 'Puzzle Game', plays: 156 },
    ],
    weakTopics: [
      { topic: 'Polymorphism', mentions: 45 },
      { topic: 'Time Complexity', mentions: 38 },
      { topic: 'Complex JOINs', mentions: 32 },
      { topic: 'Recursion', mentions: 28 },
    ],
    userGrowth: [
      { month: 'Dec', users: 45 },
      { month: 'Jan', users: 72 },
      { month: 'Feb', users: 98 },
    ],
  });
};

export default {
  getAdminStats,
  getSubjects,
  addSubject,
  updateSubject,
  deleteSubject,
  getConceptsBySubject,
  addConcept,
  updateConcept,
  deleteConcept,
  getQuestionsByConceptId,
  getQuestionsBySubjectId,
  addQuestion,
  updateQuestion,
  deleteQuestion,
  getGames,
  addGame,
  updateGame,
  deleteGame,
  getUsers,
  getUserById,
  getUserProgress,
  getAnalytics,
};
