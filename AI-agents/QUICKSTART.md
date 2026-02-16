# Quick Start Guide for Teacher AI Agent

## 🚀 Get Started in 5 Minutes

### Step 1: Get Your API Key
1. Go to https://platform.openai.com/api-keys
2. Sign up or log in
3. Create a new API key

### Step 2: Set Up Environment
```bash
# Option A: Windows Users
start.bat

# Option B: macOS/Linux Users
chmod +x start.sh
./start.sh
```

### Step 3: Configure API Key
1. Open `.env` file
2. Add your API key:
   ```
   OPENAI_API_KEY=sk-proj-xxxxxxxxxxxxx
   ```

### Step 4: Start the Server
The startup script will:
- Create a virtual environment
- Install dependencies
- Start the API server

Server will be available at: **http://localhost:8000**

### Step 5: Test It
```bash
# In a new terminal
python test_agent.py
```

## 📚 Basic Usage Examples

### Example 1: Generate a Quiz
```python
from client import TeacherAIClient

client = TeacherAIClient()
quiz = client.generate_quiz("Photosynthesis", num_questions=5)
print(quiz)
```

### Example 2: Generate Complete Module
```python
module = client.generate_learning_module(
    "Newton's Laws of Motion",
    level="intermediate"
)
```

### Example 3: Use in React
```javascript
import TeacherAIClient from './TeacherAIClient';

const client = new TeacherAIClient('http://localhost:8000');
const module = await client.generateLearningModule(concept, level);
```

## 🎮 What You Can Generate

| Content | Time | Description |
|---------|------|-------------|
| Quiz | 10-15s | Multiple choice questions with explanations |
| Puzzle | 8-12s | Logic puzzles with hints |
| Speed Challenge | 8-12s | Fast-paced challenges with time limits |
| Complete Module | 30-40s | All of the above combined |

## 🔌 API Endpoints

```
GET  /health              - Check server status
POST /summarize           - Summarize a concept
POST /generate-quiz       - Create quiz game
POST /generate-puzzle     - Create puzzle game
POST /generate-speed      - Create speed challenge
POST /generate-module     - Create complete module (⭐ main endpoint)
```

## 📖 Interactive Documentation

Once the server is running, visit:
```
http://localhost:8000/docs
```

Swagger UI will let you test all endpoints directly!

## ⚠️ Common Issues

**Issue**: "Connection refused"
- **Solution**: Make sure server is running with `start.bat` or `start.sh`

**Issue**: "401 Unauthorized"
- **Solution**: Check your ANTHROPIC_API_KEY in .env file

**Issue**: "Empty response"
- **Solution**: Verify your API key has available credits

## 🎯 Next Steps

1. ✅ Start the server
2. ✅ Test with `test_agent.py`
3. ✅ Review the generated `generated_module.json`
4. ✅ Integrate client into your React app
5. ✅ Create custom game types
6. ✅ Add database storage
7. ✅ Deploy to production

## 📚 Full Documentation

See `README.md` for comprehensive documentation including:
- Complete API reference
- Integration examples
- Performance optimization
- Troubleshooting guide
- Architecture diagrams

## 💡 Tips

- Start with simple concepts (Photosynthesis, Colors, etc.)
- For advanced content, use level="advanced"
- Cache generated modules to save API calls
- Test different concepts to see variety in output
- Check the `/docs` endpoint for real-time API testing

## 🆘 Need Help?

1. Check the [Full README.md](./README.md)
2. Review [example_react_integration.jsx](./example_react_integration.jsx)
3. Check API response in browser's Network tab
4. Enable verbose logging in `main.py`

---

**Ready to create amazing educational games?** 🎮✨
