# EduEngine AI Agent - Scene Generation API

## Overview

The AI Agent has been completely refactored to generate **EduEngine Scene JSON** files matching the official EduEngine specification. Instead of creating quizzes, puzzles, and speed games, it now generates complete interactive 2D educational platformer scenes ready to load in the game engine.

## What Changed

### Before (Old)
- Generated quiz games (multiple choice questions)
- Generated puzzle games (logic puzzles)
- Generated speed challenge games (rapid-fire questions)
- Output: Game JSON for browser-based games

### After (New) ✨
- Generates complete EduEngine scene JSON
- Full scene with platforms, collectibles, enemies, and educational content
- Automatic theme detection (10 subjects: photosynthesis, space, biology, etc.)
- Automatic grade level detection (grades 1-12)
- Level design parameters based on grade difficulty
- Output: Scene files ready for the C++ game engine

## Core Files Modified

### 1. **teacher_ai_agent.py** (Completely Rewritten)

#### New Main Method: `generate_scene(prompt: str) -> Dict`

```python
agent = TeacherAIAgent()
scene = agent.generate_scene("teach photosynthesis for grade 5")
```

**Parameters:**
- `prompt`: Natural language description of the educational content
  - Examples: "photosynthesis for grade 5", "space adventure game for year 3"

**Returns:**
- Complete EduEngine scene JSON with all required components

#### New Helper Methods:
- `detect_theme(prompt)` - Automatically identifies the educational subject
- `detect_grade(prompt)` - Automatically extracts grade level (1-12)
- `calculate_level_params(grade)` - Calculates difficulty parameters
- `_validate_and_fix_scene()` - Ensures compliance with specification
- `_create_*_entity()` - Helper methods for creating standard entities

#### Theme Configuration

Built-in support for 10 educational themes:

1. **photosynthesis** - Plant biology and photosynthesis
2. **space** - Astronomy and the solar system
3. **water_cycle** - Hydrological cycle
4. **mathematics** - Math concepts and equations
5. **history** - Historical events and figures
6. **chemistry** - Elements and chemical reactions
7. **physics** - Forces, energy, and motion
8. **biology** - Cells, DNA, and life processes
9. **geography** - Continents, climate, and landmarks
10. **default** - Generic educational content

Each theme includes:
- Colors (background, platform, collectible)
- Collectible vocabulary words
- Educational facts for info signs
- Detection keywords

### 2. **main.py** (API Endpoints Refactored)

New FastAPI endpoints for EduEngine scene generation:

#### `POST /generate-scene`
Generate an EduEngine scene from a natural language prompt.

```json
{
  "prompt": "teach photosynthesis for grade 5"
}
```

Returns complete scene JSON with all entities, components, and metadata.

#### `POST /generate-scene-file`
Generate a scene and save it to the filesystem.

```json
{
  "prompt": "space adventure for year 3"
}
```

Returns filepath and scene data.

#### `GET /themes`
List all available educational themes and their properties.

#### `GET /theme/{theme_name}`
Get detailed information about a specific theme:
- Colors (RGB values)
- Collectible vocabulary
- Educational facts
- Detection keywords

#### `POST /validate-scene`
Validate a scene JSON against the EduEngine specification.

```json
{
  "scene_meta": {...},
  "world_settings": {...},
  "entities": [...]
}
```

Returns validation results with error/warning details.

### 3. **client.py** (Refactored Python Client)

New `EduEngineAIClient` class with methods:

```python
client = EduEngineAIClient("http://localhost:8000")

# Generate a scene
scene = client.generate_scene("photosynthesis for grade 5")

# Generate and save
result = client.generate_scene_and_save(prompt)

# List themes
themes = client.list_themes()

# Get theme details
details = client.get_theme_details("photosynthesis")

# Validate a scene
validation = client.validate_scene(scene)
```

## How It Works

### Scene Generation Flow

1. **Parse Prompt**
   - Extract educational subject using keyword matching
   - Detect grade level (1-12) from prompt
   - Default to grade 5 and generic theme if unclear

2. **Load Theme Configuration**
   - Get colors, collectibles, and facts for the detected theme
   - Calculate level design parameters based on grade:
     - Number of platforms: 3-7
     - Number of enemies: 0-3 (grade 5+)
     - Number of info signs: 3

3. **Generate Scene via Claude**
   - Send detailed specification prompt to Claude API
   - Provide theme colors, collectible vocabulary, educational facts
   - Request comprehensive scene JSON matching specification

4. **Validate & Fix**
   - Parse returned JSON
   - Validate against EduEngine specification
   - Fix common issues (entity bounds, missing components)
   - Create minimal scene if generation fails

5. **Return Scene**
   - Complete, valid EduEngine scene JSON
   - Ready to load in game engine

## Entity Composition

Every generated scene contains:

| Entity | Count | Required | Details |
|--------|-------|----------|---------|
| Background | 1 | ✓ | Full-screen backdrop image |
| Player | 1 | ✓ | Controllable character at x=80, y=520 |
| Ground | 1 | ✓ | Floor platform at y=620 |
| Platforms | 3-7 | ✓ | Floating platforms to navigate |
| Collectibles | 4-8 | ✓ | Theme vocabulary items to collect |
| Info Signs | 3 | ✓ | Educational facts on signs |
| Enemies | 0-3 | ✓ | Obstacles (grade 5+) |
| Goal | 1 | ✓ | Level exit flag |

## Example Usage

### Direct Python Usage

```python
from teacher_ai_agent import TeacherAIAgent

agent = TeacherAIAgent()

# Generate a photosynthesis scene for grade 5
scene = agent.generate_scene("teach photosynthesis for grade 5 students")

# The scene includes:
# - Correct colors and theme
# - 4 platforms for navigation
# - 5 collectibles with CO₂, H₂O, etc.
# - 3 info signs with facts about photosynthesis
# - 0 enemies (grade 5)
# - Goal flag to reach the end

print(f"Scene ID: {scene['scene_meta']['id']}")
print(f"Total Entities: {len(scene['entities'])}")
```

### API Usage (FastAPI)

```bash
# Start the server
python main.py

# Generate a scene
curl -X POST http://localhost:8000/generate-scene \
  -H "Content-Type: application/json" \
  -d '{"prompt": "space adventure for grade 3"}'

# List available themes
curl http://localhost:8000/themes

# Get theme details
curl http://localhost:8000/theme/photosynthesis
```

### Python Client

```python
from client import EduEngineAIClient

client = EduEngineAIClient()

# Generate scene
scene_result = client.generate_scene("biology cells for grade 6")
scene = scene_result["data"]

# Validate scene
validation = client.validate_scene(scene)
if validation["valid"]:
    print("✅ Scene is valid!")

# List themes
themes = client.list_themes()
```

## Output Format

All generated scenes follow the EduEngine Scene JSON Specification:

```json
{
  "scene_meta": {
    "id": "photosynthesis_grade5",
    "title": "Photosynthesis — Grade 5",
    "subject": "photosynthesis",
    "grade": "5",
    "ai_prompt": "teach photosynthesis for grade 5 students"
  },
  "world_settings": {
    "gravity": 980.0,
    "background_color": [12, 28, 12, 255]
  },
  "entities": [
    {
      "name": "Background",
      "tags": ["background"],
      "components": {
        "transform": { "x": 0, "y": 0, "scale_x": 1.0, "scale_y": 1.0 },
        "sprite": {
          "texture": "assets/images/photosynthesis/background.png",
          "color": [255, 255, 255, 255],
          "layer": 0
        }
      }
    },
    // ... more entities
  ]
}
```

## Theme Configuration Details

Each theme defines:

```python
{
  "slug": "theme_identifier",
  "background_color": [R, G, B, 255],
  "platform_color": [R, G, B, 255],
  "collectible_color": [R, G, B, 220],
  "keywords": ["keyword1", "keyword2", ...],
  "collectibles": ["Term1", "Term2", ...],
  "facts": [
    "Educational fact about the theme",
    ...
  ]
}
```

## Validation Rules

Generated scenes are validated against these rules:

✓ Exactly 1 Background entity  
✓ Exactly 1 Player at x=80, y=520  
✓ Exactly 1 Ground at x=0, y=620  
✓ Exactly 1 Goal Flag at x≈1160, y=558  
✓ 3-7 Floating Platforms  
✓ 4-8 Collectibles (with is_trigger=true)  
✓ 3 Info Signs  
✓ 0-3 Enemies (grade 5+)  
✓ All entities within world bounds (0-1280, 0-720)  
✓ All texture paths use correct theme slug  
✓ Grade field is a string, not integer  
✓ Proper component structure for all entities  

## API Reference

### Endpoints

| Method | Path | Purpose |
|--------|------|---------|
| GET | `/health` | Check API health |
| POST | `/generate-scene` | Generate scene from prompt |
| POST | `/generate-scene-file` | Generate and save scene |
| GET | `/themes` | List all themes |
| GET | `/theme/{name}` | Get theme details |
| POST | `/validate-scene` | Validate scene JSON |

### Response Format

All endpoints return JSON:

```json
{
  "success": true,
  "data": { ... },
  "message": "Human-readable status message"
}
```

Error responses:

```json
{
  "success": false,
  "error_code": 500,
  "detail": "Error description"
}
```

## Example Files

- `example_scene_generation.py` - Demonstrates scene generation with various prompts
- `client.py` - Python client for interacting with the API
- `main.py` - FastAPI server with all endpoints

## Integration with Game Engine

Generated scenes are ready to load in the C++ EduEngine:

1. Generate scene using AI Agent
2. Save JSON file to `data/scenes/` directory
3. Load in game engine via scene ID
4. Engine renders all entities, handles physics, and manages gameplay

## Troubleshooting

### Scene generation is slow
- Normal: 10-30 seconds for API calls to Claude
- Check internet connection
- Verify OPENAI_API_KEY is set

### Scene fails validation
- Check error messages for missing entities
- Ensure grade level is detected correctly
- Some themes may need fallback to minimal scene

### Theme not detected
- Add keywords to prompt
- Explicitly specify theme in request
- Falls back to "default" theme if no match

## Future Enhancements

- [ ] Custom entity positions via prompt hints
- [ ] Difficulty scaling per-entity
- [ ] Multiple scene styles per theme
- [ ] Procedural platform generation
- [ ] Enemy AI behavior configuration
- [ ] Achievement/badge systems
- [ ] Educational metadata per entity
- [ ] Sound/music configuration
