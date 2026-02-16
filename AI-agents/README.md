# Teacher AI Agent - AI-Powered Learning Module Generator

An intelligent AI agent that helps teachers create interactive, customized mini-games for teaching concepts. Uses advanced LLM technology (OpenAI GPT-4) to generate educational content, summaries, and interactive games automatically.

## Features

✨ **Core Capabilities:**
- **Concept Summarization**: Break down complex teaching concepts into digestible key points
- **Quiz Generation**: Create multiple-choice quizzes with explanations
- **Puzzle Games**: Generate pattern-matching and logic puzzles  
- **Speed Challenges**: Create rapid-fire challenges to reinforce learning
- **Complete Learning Modules**: Generate all games + summary in one request
- **Multi-level Support**: Create content for beginner, intermediate, and advanced students

🎮 **Game Types:**
1. **Quiz Games** - Traditional multiple-choice with scoring and feedback
2. **Puzzle Games** - Logic puzzles with hints for deeper understanding
3. **Speed Challenges** - Fast-paced challenges with time-based scoring

## Architecture

```
AI Agent
├── Core Agent (teacher_ai_agent.py)
│   ├── Concept Summarization
│   ├── Quiz Generation
│   ├── Puzzle Generation
│   └── Speed Challenge Generation
├── FastAPI Server (main.py)
│   ├── REST API Endpoints
│   └── CORS Support
├── Python Client (client.py)
│   └── SDK for Python Integration
└── JavaScript Client (TeacherAIClient.js)
    └── SDK for Frontend Integration
```

## Installation

### Prerequisites
- Python 3.8+
- OpenAI API Key (from https://platform.openai.com/api-keys)
- Node.js (for frontend integration)

### Setup

1. **Clone/Navigate to the project:**
```bash
cd AI-agents
```

2. **Create virtual environment (optional but recommended):**
```bash
python -m venv venv
# Windows
venv\Scripts\activate
# macOS/Linux
source venv/bin/activate
```

3. **Install dependencies:**
```bash
pip install -r requirements.txt
```

4. **Configure API Key:**
```bash
# Copy the example env file
cp .env.example .env

# Edit .env and add your OpenAI API key
OPENAI_API_KEY=your_api_key_here
```

5. **Run the server:**
```bash
python main.py
```

The API will be available at `http://localhost:8000`

## Usage

### Method 1: Using the Python Client

```python
from client import TeacherAIClient

client = TeacherAIClient()

# Generate a complete learning module
module = client.generate_learning_module(
    concept="Photosynthesis",
    level="intermediate"
)

print(f"Title: {module['data']['summary']['title']}")
print(f"Games: {list(module['data']['games'].keys())}")
```

### Method 2: Using the JavaScript Client (React/Frontend)

```javascript
import TeacherAIClient from './TeacherAIClient';

const client = new TeacherAIClient('http://localhost:8000');

// Generate a module
const result = await client.generateLearningModule(
  'Photosynthesis',
  'intermediate'
);

console.log(result.data.games);
```

### Method 3: Direct REST API

```bash
# Health check
curl http://localhost:8000/health

# Generate a complete module
curl -X POST http://localhost:8000/generate-module \
  -H "Content-Type: application/json" \
  -d '{"concept": "Photosynthesis", "level": "intermediate"}'

# Generate only a quiz
curl -X POST http://localhost:8000/generate-quiz \
  -H "Content-Type: application/json" \
  -d '{"concept": "Photosynthesis", "game_type": "quiz", "num_questions": 5}'
```

## API Endpoints

### 1. Health Check
```
GET /health
```
Check if the API is running and healthy.

**Response:**
```json
{
  "status": "healthy",
  "message": "Teacher AI Agent API is running"
}
```

### 2. Summarize Concept
```
POST /summarize
```
Summarize a teaching concept into key points.

**Request:**
```json
{
  "concept": "Photosynthesis",
  "level": "intermediate"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "title": "Photosynthesis",
    "level": "intermediate",
    "summary": "...",
    "key_points": [...],
    "learning_objectives": [...],
    "examples": [...]
  }
}
```

### 3. Generate Quiz
```
POST /generate-quiz
```
Generate an interactive quiz game.

**Request:**
```json
{
  "concept": "Photosynthesis",
  "game_type": "quiz",
  "num_questions": 5
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "game_type": "quiz",
    "title": "Quiz: Photosynthesis",
    "questions": [
      {
        "id": 1,
        "question": "What is photosynthesis?",
        "options": ["A", "B", "C", "D"],
        "correct_answer": 0,
        "explanation": "..."
      }
    ],
    "scoring_rules": {...},
    "estimated_duration": 300
  }
}
```

### 4. Generate Puzzle
```
POST /generate-puzzle
```
Generate a puzzle-based game.

**Request:**
```json
{
  "concept": "Photosynthesis"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "game_type": "puzzle",
    "title": "Puzzle: Photosynthesis",
    "puzzle_type": "pattern_matching",
    "content": {...},
    "difficulty_level": "easy",
    "estimated_duration": 600
  }
}
```

### 5. Generate Speed Challenge
```
POST /generate-speed
```
Generate a speed challenge game.

**Request:**
```json
{
  "concept": "Photosynthesis",
  "game_type": "speed",
  "num_questions": 10
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "game_type": "speed",
    "title": "Speed Challenge: Photosynthesis",
    "challenges": [...],
    "scoring_rules": {...},
    "total_duration": 120
  }
}
```

### 6. Generate Complete Learning Module ⭐
```
POST /generate-module
```
The main endpoint - generates a complete learning module with summary and all game types.

**Request:**
```json
{
  "concept": "Photosynthesis",
  "level": "intermediate"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "concept": "Photosynthesis",
    "level": "intermediate",
    "summary": {...},
    "games": {
      "quiz": {...},
      "puzzle": {...},
      "speed": {...}
    },
    "estimated_total_duration": 1020,
    "teaching_notes": "..."
  }
}
```

## Integration Examples

### Example 1: React Component

See `example_react_integration.jsx` for a complete React component that uses the AI Agent.

```bash
# Copy the client to your React project
cp TeacherAIClient.js ../Frontend/src/api/
```

Then in your React component:

```jsx
import TeacherAIClient from './api/TeacherAIClient';

const client = new TeacherAIClient('http://localhost:8000');

// Generate content
const result = await client.generateLearningModule(concept, level);
```

### Example 2: Direct Python Integration

```python
from teacher_ai_agent import TeacherAIAgent

agent = TeacherAIAgent()
module = agent.generate_full_learning_module(
    concept="Newton's Laws of Motion",
    level="advanced"
)

# Access the module
print(module['summary'])
print(module['games']['quiz'])
print(module['games']['puzzle'])
print(module['games']['speed'])
```

### Example 3: Batch Game Generation

```python
from client import TeacherAIClient

client = TeacherAIClient()

concepts = [
    "Photosynthesis",
    "Newton's Laws",
    "Fractions",
    "World War 2"
]

for concept in concepts:
    module = client.generate_learning_module(concept)
    # Save or process the module
    print(f"✅ Generated module for {concept}")
```

## Configuration

### Environment Variables

Create a `.env` file with:
```bash
OPENAI_API_KEY=your_api_key_here
```

### Server Configuration

In `main.py`, you can modify:
- Host and port (default: 0.0.0.0:8000)
- CORS settings
- Request timeouts
- Model selection (default: gpt-4-turbo)

## Performance Notes

- Quiz generation: ~10-15 seconds
- Puzzle generation: ~8-12 seconds
- Speed challenge generation: ~8-12 seconds
- Complete module generation: ~30-40 seconds

Response times depend on:
- API latency
- Network speed
- Concept complexity
- Number of questions/challenges

## Extending the Agent

### Add a New Game Type

1. Add method to `TeacherAIAgent`:
```python
def generate_custom_game(self, concept: str) -> dict:
    prompt = f"Generate a custom game for: {concept}"
    response = self.client.messages.create(...)
    return game_data
```

2. Add endpoint to `main.py`:
```python
@app.post("/generate-custom")
async def generate_custom(request: ConceptRequest):
    game = ai_agent.generate_custom_game(request.concept)
    return {"success": True, "data": game}
```

3. Add method to JavaScript client:
```javascript
async generateCustomGame(concept) {
    const response = await fetch(`${this.baseUrl}/generate-custom`, {...});
    return response.json();
}
```

## Troubleshooting

### API Connection Error
- Ensure the server is running: `python main.py`
- Check the port is 8000 or correct the client base URL
- Verify firewall allows localhost connections

### Authentication Error
- Check your OPENAI_API_KEY is valid
- Ensure it's set in the .env file
- Verify you have API credits available

### Game Generation Fails
- Check the API response in server logs
- Ensure the concept is clearly described
- Try a simpler concept first
- Timeout issues
  Check internet connection stability

### Timeout Issues
- Increase timeout in HTTP client
- Try with fewer questions/challenges
- Check internet connection stability

## Project Structure

```
AI-agents/
├── main.py                      # FastAPI server
├── teacher_ai_agent.py          # Core AI agent logic
├── client.py                    # Python client SDK
├── TeacherAIClient.js          # JavaScript client SDK
├── example_react_integration.jsx # React integration example
├── requirements.txt            # Python dependencies
├── .env.example                # Environment config template
└── README.md                   # This file
```

## Best Practices

1. **Input Validation**: Always validate concept prompts
2. **Error Handling**: Handle network errors gracefully
3. **Rate Limiting**: Implement rate limiting in production
4. **Caching**: Cache generated modules to reduce API calls
5. **User Feedback**: Show loading states during generation
6. **Testing**: Test with various concept types and levels

## Performance Optimization

1. **Parallel Requests**: Generate multiple games in parallel
2. **Caching Strategy**: Store frequently used modules
3. **Chunking**: Break large concepts into smaller modules
4. **Queue System**: Implement job queue for batch generation

## License

This AI agent is part of the AI-Powered Adaptive Gamified Learning Platform.

## Support

For issues or questions:
1. Check the troubleshooting section
2. Review API endpoint documentation
3. Consult example integration files
4. Check Anthropic's documentation

## Future Enhancements

- [ ] Database integration for module storage
- [ ] User progress tracking
- [ ] Adaptive difficulty based on performance
- [ ] Multi-language support
- [ ] Custom game templates
- [ ] Analytics and reporting
- [ ] Real-time collaboration features
- [ ] Mobile app support

---

**Created for the AI-Powered Adaptive Gamified Learning Platform**
