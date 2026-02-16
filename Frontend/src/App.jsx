import { Routes, Route, Navigate } from 'react-router-dom';
import { useAuth } from './auth/AuthContext';
import ProtectedRoute from './auth/ProtectedRoute';
import Navbar from './components/Navbar';
import Home from './pages/Home';
import Dashboard from './pages/Dashboard';
import Subjects from './pages/Subjects';
import Concepts from './pages/Concepts';
import PlayGame from './pages/PlayGame';
import Progress from './pages/Progress';
import AdminDashboard from './admin/pages/AdminDashboard';
import ManageSubjects from './admin/pages/ManageSubjects';
import ManageConcepts from './admin/pages/ManageConcepts';
import ManageQuestions from './admin/pages/ManageQuestions';
import ManageGames from './admin/pages/ManageGames';
import Users from './admin/pages/Users';
import Analytics from './admin/pages/Analytics';
import RoleSelect from './auth/pages/RoleSelect';
import Login from './auth/pages/Login';
import Register from './auth/pages/Register';
import ForgotPassword from './auth/pages/ForgotPassword';
import './App.css';

function AppContent() {
  const { isAuthenticated, isLoading } = useAuth();

  if (isLoading) {
    return (
      <div style={{
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        minHeight: '100vh',
        fontSize: '18px',
        color: '#718096'
      }}>
        Loading...
      </div>
    );
  }

  return (
    <>
      {isAuthenticated && <Navbar />}
      <main className={isAuthenticated ? 'main-content' : ''}>
        <Routes>
          {/* Auth Routes */}
          <Route path="/auth/role" element={<RoleSelect />} />
          <Route path="/auth/login" element={<Login />} />
          <Route path="/auth/register" element={<Register />} />
          <Route path="/auth/forgot" element={<ForgotPassword />} />

          {/* Student Routes - Protected */}
          <Route
            path="/"
            element={
              <ProtectedRoute>
                <Home />
              </ProtectedRoute>
            }
          />
          <Route
            path="/dashboard"
            element={
              <ProtectedRoute requiredRole="user">
                <Dashboard />
              </ProtectedRoute>
            }
          />
          <Route
            path="/subjects"
            element={
              <ProtectedRoute requiredRole="user">
                <Subjects />
              </ProtectedRoute>
            }
          />
          <Route
            path="/concepts"
            element={
              <ProtectedRoute requiredRole="user">
                <Concepts />
              </ProtectedRoute>
            }
          />
          <Route
            path="/play-game"
            element={
              <ProtectedRoute requiredRole="user">
                <PlayGame />
              </ProtectedRoute>
            }
          />
          <Route
            path="/progress"
            element={
              <ProtectedRoute requiredRole="user">
                <Progress />
              </ProtectedRoute>
            }
          />

          {/* Admin Routes - Protected */}
          <Route
            path="/admin"
            element={
              <ProtectedRoute requiredRole="admin">
                <AdminDashboard />
              </ProtectedRoute>
            }
          />
          <Route
            path="/admin/subjects"
            element={
              <ProtectedRoute requiredRole="admin">
                <ManageSubjects />
              </ProtectedRoute>
            }
          />
          <Route
            path="/admin/concepts"
            element={
              <ProtectedRoute requiredRole="admin">
                <ManageConcepts />
              </ProtectedRoute>
            }
          />
          <Route
            path="/admin/questions"
            element={
              <ProtectedRoute requiredRole="admin">
                <ManageQuestions />
              </ProtectedRoute>
            }
          />
          <Route
            path="/admin/games"
            element={
              <ProtectedRoute requiredRole="admin">
                <ManageGames />
              </ProtectedRoute>
            }
          />
          <Route
            path="/admin/users"
            element={
              <ProtectedRoute requiredRole="admin">
                <Users />
              </ProtectedRoute>
            }
          />
          <Route
            path="/admin/analytics"
            element={
              <ProtectedRoute requiredRole="admin">
                <Analytics />
              </ProtectedRoute>
            }
          />

          {/* Catch all - redirect to role select if not authenticated, otherwise home */}
          <Route path="*" element={<Navigate to={isAuthenticated ? "/" : "/auth/role"} replace />} />
        </Routes>
      </main>
    </>
  );
}

function App() {
  return <AppContent />;
}

export default App;
