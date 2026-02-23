# 🎮 EduEngine AI-to-Game Pipeline - Complete Guide

## What This Does

You now have a complete pipeline that:

1. **Generates scenes using AI** - Uses Claude API to create game scenes from natural language prompts
2. **Validates the JSON** - Ensures all generated scenes match the EduEngine specification
3. **Saves to the game engine** - Stores scenes in `GameEngine/data/scenes/`
4. **Launches the game** - Automatically runs the compiled game engine with your generated scene

## Quick Start (3 Commands)

```bash
cd AI-agents

# Command 1: Run tests to verify everything works
python3 test_pipeline.py

# Command 2: Generate and play a game
python3 run_game.py "teach photosynthesis for grade 5"

# Command 3: Generate without playing
python3 run_game.py "space adventure for grade 3" --save-only
```

## Files Created

### 1. **run_game.py** - Main Pipeline Script

Generates a scene using AI and launches the game engine.

**Usage:**
```bash
python3 run_game.py "your prompt here"
python3 run_game.py "photosynthesis for grade 5"
python3 run_game.py "space for year 3" --grade 3
python3 run_game.py "biology" --save-only              # Don't launch game
python3 run_game.py "math" --validate-only             # Validate JSON only
python3 run_game.py "history" --no-validate            # Skip validation
```

**What it does:**
1. Takes a natural language prompt
2. Sends it to the AI agent
3. Receives scene JSON
4. Validates the JSON structure
5. Saves to `GameEngine/data/scenes/`
6. Launches the game engine with that scene

**Output:**
```
════════════════════════════════════════════════════════════════════════
🤖 AI SCENE GENERATION
════════════════════════════════════════════════════════════════════════
📝 Prompt: teach photosynthesis for grade 5

⏳ Generating scene JSON...
✅ Scene saved to: /path/to/GameEngine/data/scenes/photosynthesis_grade5.json

════════════════════════════════════════════════════════════════════════
✅ SCENE VALIDATION
════════════════════════════════════════════════════════════════════════

✅ Scene structure: VALID
✅ Total entities: 15

📊 Entity Breakdown:
  • Platforms:    4
  • Collectibles: 5
  • Enemies:      0
  • Info Signs:   3

════════════════════════════════════════════════════════════════════════
🎮 LAUNCHING GAME ENGINE
════════════════════════════════════════════════════════════════════════

▶️  Starting game...
```

### 2. **test_pipeline.py** - Complete Test Suite

Tests every step of the pipeline.

**Usage:**
```bash
# Run all tests
python3 test_pipeline.py

# Verbose output
python3 test_pipeline.py --verbose

# Test specific theme/grade
python3 test_pipeline.py --theme space --grade 3

# Test specific prompt
python3 test_pipeline.py --prompt "teach photosynthesis for grade 5"
```

**What it tests:**
1. ✅ **AI Generation** - Scene JSON generation works
2. ✅ **Scene Structure** - All required entities present
3. ✅ **JSON Serialization** - Valid JSON format
4. ✅ **File Save** - Can save to disk
5. ✅ **File Load** - Can read back from disk
6. ✅ **Engine Binary** - Game engine exists and is executable
7. ✅ **Theme Detection** - Correct theme from prompts
8. ✅ **Full Pipeline** - Entire process works end-to-end

**Test Output Example:**
```
════════════════════════════════════════════════════════════════════════
TEST SUMMARY
════════════════════════════════════════════════════════════════════════

✅ PASS - Generate Scene: 'teach photosynthesis for grade 5'
✅ PASS - Validate Scene Structure
✅ PASS - JSON Serialization
✅ PASS - Save Scene to File
✅ PASS - Load Scene from File
✅ PASS - Game Engine Binary
✅ PASS - Theme Detection
✅ PASS - Full Pipeline: teach photosynthesis for grade 5

════════════════════════════════════════════════════════════════════════
RESULTS: 8/8 PASSED
════════════════════════════════════════════════════════════════════════

✅ All tests passed!
```

### 3. **run_game.sh** - Bash Wrapper Script

Quick bash script to run the pipeline.

**Usage:**
```bash
./run_game.sh "teach photosynthesis for grade 5"
./run_game.sh "space adventure"
```

## How the Pipeline Works

```
┌─────────────────────────────────────────────────────────────────┐
│ 1. USER ENTERS PROMPT                                           │
│    "teach photosynthesis for grade 5"                           │
└─────────────────┬───────────────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────────────────┐
│ 2. AI GENERATION (run_game.py)                                  │
│    - Send prompt to TeacherAIAgent                              │
│    - Detect theme: "photosynthesis"                             │
│    - Detect grade: "5"                                          │
│    - Generate complete scene JSON                               │
└─────────────────┬───────────────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────────────────┐
│ 3. VALIDATION (run_game.py)                                     │
│    - Check required entities (Background, Player, Ground, Goal) │
│    - Verify entity bounds (0-1280, 0-720)                       │
│    - Validate texture paths                                     │
│    - Check component structure                                  │
└─────────────────┬───────────────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────────────────┐
│ 4. SAVE JSON (run_game.py)                                      │
│    - Create file: GameEngine/data/scenes/SCENE_ID.json          │
│    - Save full scene JSON with indent=2                         │
└─────────────────┬───────────────────────────────────────────────┘
                  │
                  ▼
┌─────────────────────────────────────────────────────────────────┐
│ 5. LAUNCH GAME ENGINE (run_game.py)                             │
│    - Run: ./GameEngine/EduEngine /path/to/scene.json            │
│    - Engine loads and renders the scene                         │
│    - Player can interact with the game                          │
└─────────────────────────────────────────────────────────────────┘
```

## File Locations

**Input:**
- Prompts come from user (command line)

**Processing:**
- AI agent: `AI-agents/teacher_ai_agent.py`
- Pipeline: `AI-agents/run_game.py`
- Tests: `AI-agents/test_pipeline.py`

**Output:**
- Scenes saved to: `GameEngine/data/scenes/`
- Example: `GameEngine/data/scenes/photosynthesis_grade5.json`

**Execution:**
- Game engine: `GameEngine/EduEngine` (binary)
- Launches with: `./EduEngine /path/to/scene.json`

## Testing Guide

### Step 1: Test the Entire Pipeline

```bash
cd AI-agents
python3 test_pipeline.py --verbose
```

This runs 8 tests covering:
- Scene generation from various prompts
- JSON structure validation
- Theme detection
- File I/O
- Game engine availability

**Expected Result:** ✅ All tests pass

### Step 2: Test Theme Detection

```bash
python3 test_pipeline.py --verbose
```

Look for the "Theme Detection" test output. It should correctly identify:
- "photosynthesis for grade 5" → photosynthesis, grade 5
- "space adventure" → space, grade 5 (default)
- "biology cells" → biology, grade 5 (default)

### Step 3: Test Full End-to-End Pipeline

```bash
python3 run_game.py "teach photosynthesis for grade 5" --verbose
```

This should:
1. ✅ Generate scene JSON
2. ✅ Validate structure
3. ✅ Save file
4. ✅ Launch game
5. ✅ Close cleanly when you exit the game

### Step 4: Test Individual Components

**Test just generation:**
```bash
python3 run_game.py "space adventure" --save-only
```

**Test just validation:**
```bash
python3 run_game.py "biology cells" --validate-only
```

**Skip validation (faster):**
```bash
python3 run_game.py "history" --no-validate
```

### Step 5: Test Different Themes

```bash
# Test each theme
python3 run_game.py "teach photosynthesis"      # Green theme
python3 run_game.py "space adventure"           # Space theme
python3 run_game.py "water cycle"               # Water theme
python3 run_game.py "biology cells"             # Forest theme
python3 run_game.py "chemistry elements"        # Purple theme
python3 run_game.py "physics forces"            # Blue theme
python3 run_game.py "history rome"              # Brown theme
python3 run_game.py "mathematics algebra"       # Purple theme
python3 run_game.py "geography continents"      # Ocean theme
```

## Troubleshooting

### ❌ "OpenAI API key not found"

**Error:**
```
OpenAI API key not found. Set OPENAI_API_KEY environment variable.
```

**Fix:**
```bash
export OPENAI_API_KEY="sk-..."
python3 run_game.py "photosynthesis for grade 5"
```

Or create `.env` file in `AI-agents/`:
```
OPENAI_API_KEY=sk-...
```

### ❌ "Game engine binary not found"

**Error:**
```
Game engine binary not found at GameEngine/EduEngine
Please compile the game engine first
```

**Fix:**
Compile the C++ game engine:
```bash
cd GameEngine
g++ -o EduEngine src/main.cpp ... (full compile command)
```

Or check if it exists:
```bash
ls -l GameEngine/EduEngine
```

### ❌ Scene validation fails

**Error:**
```
❌ Expected 1 Background, found 0
```

**Fix:**
- Ensure the AI agent is generating valid scenes
- Check that scenes include all required entities
- Try a different prompt
- Run with `--no-validate` to skip validation and see the actual error

### ❌ Game crashes when launched

**Error:**
```
Game exited with code 1
```

**Possible causes:**
1. Scene JSON is invalid - validate with `--validate-only`
2. Missing texture files - check `GameEngine/assets/images/`
3. Game engine issue - try running manually:
   ```bash
   cd GameEngine
   ./EduEngine data/scenes/photosynthesis_grade5.json
   ```

## Advanced Usage

### Generate Multiple Scenes

```bash
# Create a batch of scenes
for topic in "photosynthesis" "space" "water cycle" "biology"; do
    python3 run_game.py "$topic" --save-only
done

# List generated scenes
ls GameEngine/data/scenes/
```

### Test with Specific Parameters

```bash
# Override grade level
python3 run_game.py "biology" --grade 8

# Test a specific theme
python3 test_pipeline.py --theme space --grade 3

# Verbose test output
python3 test_pipeline.py --verbose --theme chemistry
```

### Inspect Generated JSON

```bash
# Generate and view the JSON
python3 run_game.py "photosynthesis for grade 5" --save-only

# View the generated file
cat ../GameEngine/data/scenes/photosynthesis_grade5.json | python3 -m json.tool | less
```

### Chain With External Systems

```bash
# Use in a script
#!/bin/bash
python3 run_game.py "teach ${TOPIC} for grade ${GRADE}" --save-only

# Use in a Python script
import subprocess
result = subprocess.run(
    ["python3", "run_game.py", "photosynthesis for grade 5"],
    capture_output=True,
    text=True
)
print(result.stdout)
```

## Complete Example Workflow

```bash
cd /home/aryan/Desktop/AI_PROJECT/AI-Powered-Adaptive-Gamified-Learning-Platform-for-Concept-Mastery/AI-agents

# Step 1: Verify everything works
echo "Testing pipeline..."
python3 test_pipeline.py

# Step 2: Generate a game
echo "Generating photosynthesis game for grade 5..."
python3 run_game.py "teach photosynthesis for grade 5"

# Step 3: Check what was created
echo "Generated scene location:"
ls -lh ../GameEngine/data/scenes/photosynthesis_grade5.json

# Step 4: View the JSON structure
echo "Scene structure:"
cat ../GameEngine/data/scenes/photosynthesis_grade5.json | python3 -m json.tool | head -50
```

## Success Criteria

Your pipeline is working correctly when:

✅ `test_pipeline.py` shows "All 8/8 tests passed"

✅ `run_game.py "photosynthesis for grade 5"` produces:
- ✅ Scene generated successfully
- ✅ Scene structure: VALID
- ✅ Scene saved to file
- ✅ Game launches and runs

✅ Game window opens showing:
- A colorful background (theme colors)
- A player character
- Platforms to jump on
- Items to collect
- Information signs
- A goal flag

✅ You can:
- Move left/right with arrow keys
- Jump with spacebar
- Collect items
- Read info signs
- Reach the goal

## Next Steps

1. Run the tests: `python3 test_pipeline.py`
2. Generate your first game: `python3 run_game.py "photosynthesis for grade 5"`
3. Verify game runs correctly
4. Experiment with different prompts
5. Check the generated JSON files in `GameEngine/data/scenes/`

Enjoy! 🎮
