# Quick Start Guide - Authentication System

## 5-Minute Setup

### Step 1: Backend Setup (2 minutes)

```bash
# Navigate to Backend
cd Backend

# Create .env file (if not exists)
# Copy contents from .env.example and update if needed

# Install dependencies (if not done)
pip install -r requirements.txt

# Run backend server
python -m uvicorn api:app --reload --host 0.0.0.0 --port 8000
```

✅ Backend running at `http://localhost:8000`

### Step 2: Frontend Setup (2 minutes)

```bash
# Navigate to Frontend
cd Frontend

# Create .env.local file (if not exists)
# Copy contents from .env.example

# Install dependencies (if not done)
npm install

# Run frontend server
npm run dev
```

✅ Frontend running at `http://localhost:5173`

### Step 3: Test the System (1 minute)

1. Open `http://localhost:5173` in browser
2. Click "Start"
3. Select "Student" or "Teacher"
4. Sign up with test data:
   - **Name**: Test User
   - **Email**: test@example.com
   - **Password**: TestPass123
   - **College**: MIT (for students)
   - **Department**: CS (for students)

5. Click "Sign Up"
6. You should be auto-logged in and redirected to dashboard

## Test Existing User Login

After first registration, test login with same credentials:

1. Go to `http://localhost:5173`
2. Click "Start" → Select role
3. Click "Sign In"
4. Enter email and password from registration
5. Click "Sign In"

## API Testing (Optional)

### Register via cURL
```bash
curl -X POST http://localhost:8000/auth/register \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Jane Doe",
    "email": "jane@example.com",
    "username": "janedoe",
    "password": "JanePass123",
    "role": "teacher",
    "experience": "5 years",
    "subjects": ["Math", "Physics"]
  }'
```

### Login via cURL
```bash
curl -X POST http://localhost:8000/auth/login \
  -H "Content-Type: application/json" \
  -d '{
    "email": "jane@example.com",
    "password": "JanePass123",
    "role": "teacher"
  }'
```

## Common Issues & Solutions

### "Connection refused on port 8000"
- ✅ Make sure backend is running: `python -m uvicorn api:app --reload`
- ✅ Check if port 8000 is available

### "Cannot find module" errors
- ✅ Install dependencies: 
  - Backend: `pip install -r requirements.txt`
  - Frontend: `npm install`

### "Token verification failed"
- ✅ Backend might be restarted
- ✅ Clear browser localStorage: F12 → Application → Clear Storage
- ✅ Refresh page and login again

### CORS errors
- ✅ Check `VITE_APP_AUTH_URL` in `.env.local` matches backend URL
- ✅ Default is `http://localhost:8000`

## What's New

✅ Backend JWT authentication endpoints:
- `POST /auth/register` - Register student or teacher
- `POST /auth/login` - Login with email
- `GET /auth/verify` - Verify token
- `GET /users/me` - Get profile

✅ Frontend connected to real backend:
- Auto-login after registration
- Real password verification
- JWT token storage
- Protected routes

✅ Database ready:
- SQLite (default) for testing
- PostgreSQL setup docs included
- Keep database folder separate ✅

## Next Steps

1. ✅ Register test user
2. ✅ Login with test credentials
3. ✅ Check database (game_platform.db created)
4. ⏳ Connect game endpoints to require authentication
5. ⏳ Set up PostgreSQL (see AUTHENTICATION_SETUP.md)
6. ⏳ Implement password reset
7. ⏳ Deploy to production

## File Reference

- **Backend Auth**: `Backend/auth.py`
- **API Routes**: `Backend/api.py` (lines with `/auth/`)
- **Frontend Auth**: `Frontend/src/auth/authService.js`
- **Context**: `Frontend/src/auth/AuthContext.jsx`
- **Full Setup**: `AUTHENTICATION_SETUP.md`
- **Implementation Details**: `AUTH_IMPLEMENTATION_SUMMARY.md`

## Support

Check `AUTHENTICATION_SETUP.md` for:
- Detailed configuration
- Database schema
- Full API documentation
- Troubleshooting guide
- Security best practices

---

**You're all set!** 🎉 The authentication system is ready to use.

Start both servers and test registration/login to complete the implementation.
