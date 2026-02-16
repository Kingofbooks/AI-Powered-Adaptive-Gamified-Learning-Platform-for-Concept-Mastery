# 🎮 LearnGame - Gamified Learning Platform

A modern, interactive web application designed to make learning programming concepts, data structures, databases, and logical reasoning fun through gamified challenges.

## ✨ Features

### Core Functionality
- **🎯 Multiple Game Types**: Quiz (multiple choice), Speed (timed challenges), and Puzzle (matching challenges)
- **📚 Comprehensive Curriculum**: 4 main subjects (Java, DSA, SQL, Aptitude) with multiple concepts each
- **📊 Progress Tracking**: Track your performance, accuracy, and level progression
- **🎓 Responsive Design**: Works seamlessly on desktop, tablet, and mobile devices
- **⚡ Real-time Navigation**: Fast page transitions with React Router
- **💾 Context-Based State Management**: Global game state using React Context

### Subjects & Concepts
- **Java**: Arrays, OOP, Strings, Exception Handling
- **DSA**: Linked Lists, Sorting Algorithms, Binary Search, Dynamic Programming
- **SQL**: SELECT Queries, JOINs, Aggregation, Indexes
- **Aptitude**: Numbers, Geometry, Logic Puzzles, Percentages

## 🏗️ Project Structure

```
frontend/
├── src/
│   ├── api/
│   │   ├── aiApi.js           # AI/Game generation API (mock)
│   │   └── httpClient.js      # HTTP client wrapper
│   ├── components/
│   │   ├── Button.jsx         # Reusable button component
│   │   ├── ConceptCard.jsx    # Concept card display
│   │   ├── GameBox.jsx        # Game iframe container
│   │   ├── Loader.jsx         # Loading spinner
│   │   ├── Navbar.jsx         # Navigation bar
│   │   └── SubjectCard.jsx    # Subject card display
│   ├── context/
│   │   └── GameContext.jsx    # Global game state management
│   ├── pages/
│   │   ├── Home.jsx           # Landing page
│   │   ├── Dashboard.jsx      # User dashboard
│   │   ├── Subjects.jsx       # Subject selection
│   │   ├── Concepts.jsx       # Concept selection
│   │   ├── PlayGame.jsx       # Game interface
│   │   └── Progress.jsx       # Progress & statistics
│   ├── styles/
│   │   ├── Button.css         # Button styling
│   │   ├── Card.css           # Card components styling
│   │   ├── GameBox.css        # Game container styling
│   │   ├── Loader.css         # Loader styling
│   │   ├── Navbar.css         # Navigation styling
│   │   └── Pages.css          # Page-specific styling
│   ├── utils/
│   │   └── constants.js       # Constants & mock data
│   ├── App.jsx                # Main app with routing
│   ├── App.css                # Global utilities & layout
│   ├── index.css              # Base styles & theme
│   └── main.jsx               # React entry point
├── public/
│   └── games/
│       ├── quiz/index.html    # Quiz game implementation
│       ├── speed/index.html   # Speed game implementation
│       └── puzzle/index.html  # Puzzle game implementation
├── .env                       # Environment variables
├── package.json               # Dependencies & scripts
├── vite.config.js            # Vite configuration
└── README.md                 # This file
```

## 🎮 Game Descriptions

### Quiz Game 🎯
- **Type**: Multiple choice questions
- **Duration**: No time limit
- **Mechanics**: Select answer, submit, get feedback
- **Questions**: 5 per session
- **Feedback**: Immediate correct/incorrect indication

### Speed Game ⚡
- **Type**: Fast-paced calculation/logic problems
- **Duration**: 60 seconds
- **Mechanics**: Type answer and press Enter
- **Questions**: As many as possible in 60 seconds
- **Scoring**: Points awarded for correct answers

### Puzzle Game 🧩
- **Type**: Concept-definition matching
- **Duration**: No time limit
- **Mechanics**: Click to select, match concepts with definitions
- **Levels**: Multiple puzzle sets
- **Completion**: All pairs must be matched

## 🎨 Design System

### Color Palette
- **Primary**: `#7c3aed` (Purple)
- **Secondary**: `#a78bfa` (Light Purple)
- **Background**: `#0f0f1e` (Dark Navy)
- **Text**: `#e0e0e0` (Light Gray)
- **Success**: `#10b981` (Green)
- **Warning**: `#f59e0b` (Amber)
- **Error**: `#ef4444` (Red)

### Typography
- **Font Family**: 'Segoe UI', 'Helvetica Neue', sans-serif
- **Headings**: Bold, 1.2 line-height
- **Body**: Regular weight, 1.5 line-height

### Components
- Cards with hover elevation and gradient backgrounds
- Rounded corners (0.5rem to 1rem border-radius)
- Smooth transitions (0.3s cubic-easing)
- Responsive grid layouts
- Shadow effects for depth

## 🚀 Getting Started

### Prerequisites
- Node.js (v14 or higher)
- npm or yarn package manager

### Installation

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd frontend
   ```

2. **Install dependencies**
   ```bash
   npm install
   ```

3. **Set environment variables**
   - Create or update `.env` file with:
     ```
     VITE_APP_API_URL=http://localhost:3001/api
     VITE_APP_NAME=LearnGame
     ```

### Development

1. **Start development server**
   ```bash
   npm run dev
   ```
   - Application will be available at `http://localhost:5173`
   - Hot Module Replacement (HMR) enabled for instant updates

2. **Build for production**
   ```bash
   npm run build
   ```

3. **Preview production build**
   ```bash
   npm run preview
   ```

4. **Run linter**
   ```bash
   npm run lint
   ```

## 🔄 Application Flow

### User Journey
1. **Home Page** → Introduction and call-to-action
2. **Dashboard** → Quick access to continue learning, choose subject, view progress
3. **Subjects Page** → Browse and select from 4 main subjects
4. **Concepts Page** → Choose specific topic within selected subject
5. **Play Game** → Set up and launch game
6. **Game Interface** → Play quiz, speed, or puzzle game
7. **Progress Page** → View statistics, achievements, and tips

### State Management
Uses React Context (`GameContext`) to manage:
- `selectedSubject`: Currently selected subject
- `selectedConcept`: Currently selected concept
- `currentGame`: Active game type
- `gameLoading`: Loading state for game generation

## 📡 API Layer

### httpClient.js
Wraps native `fetch` API with:
- Base URL configuration
- Automatic timeout handling
- Error handling and logging
- Request customization options

### aiApi.js
Provides mock implementations for:
- `getNextGame(subject, concept)` - Returns game type and metadata
- `getRecommendations(subject)` - Returns learning recommendations
- `getAdaptiveQuestions(subject, concept, userLevel)` - Returns difficulty-adaptive questions

## 💡 Key Concepts

### Responsive Design
- Mobile-first approach
- CSS Grid & Flexbox layouts
- Breakpoints at 768px and 600px
- Touch-friendly buttons and inputs

### Accessibility
- Semantic HTML structure
- Proper heading hierarchy
- Color contrast compliant
- Keyboard navigation support

### Performance
- Code splitting with React Router
- Lazy loading of game components
- Optimized CSS with CSS Grid
- Minified assets in production

## 🔧 Technologies Used

- **Framework**: React 19
- **Build Tool**: Vite
- **Routing**: React Router v6
- **Styling**: Custom CSS with theme variables
- **State Management**: React Context API
- **Package Manager**: npm

## 📝 Notes for Hackathon Demo

- ✅ All pages are fully functional with mock data
- ✅ Games have working implementations (Quiz, Speed, Puzzle)
- ✅ Responsive design tested on multiple screen sizes
- ✅ Dark theme with purple accent colors
- ✅ Smooth animations and transitions
- ⚠️ Backend API not required (uses mock data)
- ⚠️ Can be integrated with real API by updating `aiApi.js`

## 🎯 Future Enhancements

- Real backend integration with Node.js/Express
- User authentication and accounts
- Leaderboard system
- Achievement badges and rewards
- Daily challenges and streaks
- Multiplayer competitive modes
- Analytics and detailed performance insights
- Mobile app version

## 📄 License

This project is open source and available under the MIT License.

## 👥 Contributing

Contributions are welcome! Please follow these steps:
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

---

**Happy Learning! 🚀**
#   A I - P o w e r e d - A d a p t i v e - G a m i f i e d - L e a r n i n g - P l a t f o r m - f o r - C o n c e p t - M a s t e r y  
 