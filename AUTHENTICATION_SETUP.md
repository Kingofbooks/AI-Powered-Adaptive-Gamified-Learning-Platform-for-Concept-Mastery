# Authentication System Setup Guide

## Overview

This document explains the complete authentication system that connects the Frontend sign-in and signup pages to the Backend with JWT tokens and PostgreSQL database support.

## Architecture

```
Frontend (React)
    ↓
AuthContext + authService.js
    ↓
HTTP Client (Fetch)
    ↓
Backend (FastAPI)
    ↓
JWT Auth Middleware
    ↓
Database (PostgreSQL/SQLite)
```

## Key Features

✅ **Role-Based Authentication** (Student, Teacher, Admin)
✅ **Password Hashing** with bcrypt
✅ **JWT Token Generation & Verification**
✅ **Token-Based API Protection**
✅ **Email-Based Login**
✅ **Auto-Login After Registration**
✅ **Token Refresh Support**

## Backend Setup

### 1. Install Dependencies

```bash
cd Backend
pip install -r requirements.txt
```

Required packages:
- `fastapi` - Web framework
- `sqlalchemy` - ORM
- `passlib[bcrypt]` - Password hashing
- `python-jose[cryptography]` - JWT tokens
- `psycopg2-binary` - PostgreSQL driver

### 2. Configure Environment Variables

Create a `.env` file in the `Backend` folder:

```env
# Database - For PostgreSQL
DATABASE_URL=postgresql://user:password@localhost:5432/learning_platform

# Or for SQLite (development)
DATABASE_URL=sqlite:///./game_platform.db

# JWT Configuration
SECRET_KEY=your-super-secret-key-change-this-in-production
ACCESS_TOKEN_EXPIRE_MINUTES=480

# OpenAI API
OPENAI_API_KEY=your_api_key

# Server
HOST=0.0.0.0
PORT=8000
```

### 3. Set Up Database

#### Option A: PostgreSQL (Recommended)

```bash
# Create database
createdb learning_platform

# The tables will be created automatically when the app runs
```

#### Option B: SQLite (Development)

SQLite database is created automatically at `game_platform.db`

### 4. Run Backend Server

```bash
# From Backend directory
python -m uvicorn api:app --reload --host 0.0.0.0 --port 8000
```

Server will be available at `http://localhost:8000`

## Frontend Setup

### 1. Install Dependencies

```bash
cd Frontend
npm install
```

### 2. Configure Environment (.env)

Create a `.env.local` file in the `Frontend` folder:

```env
VITE_APP_API_URL=http://localhost:8000/api
VITE_APP_AUTH_URL=http://localhost:8000
```

### 3. Run Frontend Server

```bash
npm run dev
```

Frontend will be available at `http://localhost:5173`

## Authentication Flow

### Registration Flow

```
1. User clicks "Sign Up"
   ↓
2. Selects role (Student/Teacher)
   ↓
3. Fills registration form
   ↓
4. Frontend calls POST /auth/register
   ↓
5. Backend validates & creates user with hashed password
   ↓
6. Backend generates JWT token
   ↓
7. Frontend stores token + user in localStorage
   ↓
8. Auto-login & redirect to dashboard
```

### Login Flow

```
1. User clicks "Sign In"
   ↓
2. Selects role (Student/Teacher/Admin)
   ↓
3. Enters email & password
   ↓
4. Frontend calls POST /auth/login
   ↓
5. Backend verifies email/password/role
   ↓
6. Backend generates JWT token
   ↓
7. Frontend stores token + user in localStorage
   ↓
8. Redirect to dashboard
```

## API Endpoints

### Public Endpoints

#### Register
```
POST /auth/register

Request:
{
  "name": "John Doe",
  "email": "john@example.com",
  "username": "johndoe",
  "password": "SecurePassword123!",
  "role": "student",
  "college": "MIT",
  "department": "Computer Science",
  "year": "2nd Year"
}

Response:
{
  "access_token": "eyJhbGc...",
  "token_type": "bearer",
  "user": {
    "id": 1,
    "username": "johndoe",
    "email": "john@example.com",
    "name": "John Doe",
    "role": "student",
    "college": "MIT",
    "department": "Computer Science",
    "year": "2nd Year"
  }
}
```

#### Login
```
POST /auth/login

Request:
{
  "email": "john@example.com",
  "password": "SecurePassword123!",
  "role": "student"
}

Response:
{
  "access_token": "eyJhbGc...",
  "token_type": "bearer",
  "user": { ... }
}
```

#### Verify Token
```
GET /auth/verify

Headers:
Authorization: Bearer <token>

Response:
{
  "valid": true,
  "user_id": 1,
  "username": "johndoe",
  "email": "john@example.com",
  "role": "student"
}
```

### Protected Endpoints

#### Get Current User Profile
```
GET /users/me

Headers:
Authorization: Bearer <token>

Response:
{
  "id": 1,
  "username": "johndoe",
  "email": "john@example.com",
  "name": "John Doe",
  "role": "student",
  ...
}
```

All other endpoints require `Authorization: Bearer <token>` header.

## User Roles

### Student
- Can play games
- Tracks progress
- Views stats
- Fields: `college`, `department`, `year`, `level`

### Teacher
- Can create content
- Manages classes
- Views class progress
- Fields: `experience`, `subjects`

### Admin
- Full platform access
- Manage users
- View analytics

## Frontend Components

### AuthContext (`src/auth/AuthContext.jsx`)

Main state management for authentication:

```jsx
const { 
  user,              // Current user object
  role,              // 'student', 'teacher', 'admin'
  token,             // JWT token
  isAuthenticated,   // Boolean
  isLoading,         // Loading state
  login,             // Manual login method
  logout,            // Logout method
  register,          // Register method
  loginWithEmail,    // Email login method
  setSelectedRole,   // Set role for new registration
} = useAuth();
```

### AuthService (`src/auth/authService.js`)

API communication layer:

```javascript
import * as authService from './auth/authService';

// Register
await authService.register(formData, 'student');

// Login
await authService.login(email, password, 'student');

// Verify Token
await authService.verifyToken(token);

// Get Current User
await authService.getCurrentUser(token);

// Token Management
authService.setAuthToken(token, user);
authService.getAuthToken();
authService.getStoredUser();
authService.clearAuth();
```

## Database Schema

### Users Table

```sql
CREATE TABLE users (
  id INTEGER PRIMARY KEY,
  username VARCHAR UNIQUE,
  email VARCHAR UNIQUE,
  name VARCHAR,
  password_hash VARCHAR,
  role ENUM (student, teacher, admin),
  created_at TIMESTAMP,
  last_login TIMESTAMP,
  is_active BOOLEAN DEFAULT true,
  
  -- Student fields
  level VARCHAR,
  total_score INTEGER DEFAULT 0,
  total_games_played INTEGER DEFAULT 0,
  college VARCHAR,
  department VARCHAR,
  year VARCHAR,
  
  -- Teacher fields
  experience VARCHAR,
  subjects JSON,
  classes JSON
)
```

## Security Best Practices

✅ **Implemented**
- bcrypt password hashing (passlib)
- JWT tokens with expiration
- HTTPS-ready CORS configuration
- Token in Authorization header
- Secure password validation

⚠️ **To Implement**
- Use HTTPS in production
- Store SECRET_KEY securely (not in code)
- Implement password reset email flow
- Add rate limiting
- Add CSRF protection
- Implement refresh tokens
- Add 2FA support

## Testing the System

### Test Student Registration

```bash
curl -X POST http://localhost:8000/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Test Student",
    "email": "student@test.com",
    "username": "teststudent",
    "password": "TestPass123!",
    "role": "student",
    "college": "MIT",
    "department": "CS",
    "year": "2nd Year"
  }'
```

### Test Student Login

```bash
curl -X POST http://localhost:8000/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "email": "student@test.com",
    "password": "TestPass123!",
    "role": "student"
  }'
```

### Test Protected Endpoint

```bash
curl -X GET http://localhost:8000/users/me \
  -H "Authorization: Bearer <your_token_here>"
```

## Troubleshooting

### "Token verification failed"
- Check if token is expired
- Verify SECRET_KEY matches between registration and login
- Check Authorization header format: `Bearer <token>`

### "Invalid email, password, or role"
- Verify email is registered
- Check password is correct
- Ensure role matches (student/teacher/admin)

### CORS errors
- Check CORS_ORIGINS in .env
- Update httpClient.js API URLs

### Database connection error
- Verify DATABASE_URL in .env
- For PostgreSQL: Ensure PostgreSQL is running
- For SQLite: Check file permissions

## Next Steps

1. ✅ Set up environment variables
2. ✅ Configure database
3. ✅ Start backend server
4. ✅ Start frontend server
5. ✅ Test registration and login
6. ⏳ Implement password reset
7. ⏳ Add refresh token logic
8. ⏳ Implement role-based access control
9. ⏳ Add email verification
10. ⏳ Deploy to production

## Support

For issues or questions:
1. Check the troubleshooting section
2. Review API response logs
3. Check database transaction logs
4. Verify environment variables are set correctly
