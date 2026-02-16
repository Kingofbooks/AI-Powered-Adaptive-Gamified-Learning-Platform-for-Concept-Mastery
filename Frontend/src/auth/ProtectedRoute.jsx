import { Navigate } from 'react-router-dom';
import { useAuth } from './AuthContext';

/**
 * ProtectedRoute - Protects routes based on authentication and role
 */
function ProtectedRoute({ 
  children, 
  requiredRole = null 
}) {
  const { isAuthenticated, isLoading, role } = useAuth();

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

  // Not authenticated
  if (!isAuthenticated) {
    return <Navigate to="/auth/role" replace />;
  }

  // Check if user has required role
  if (requiredRole && role !== requiredRole) {
    // Redirect to appropriate dashboard based on role
    if (role === 'admin') {
      return <Navigate to="/admin" replace />;
    } else {
      return <Navigate to="/dashboard" replace />;
    }
  }

  return children;
}

export default ProtectedRoute;
