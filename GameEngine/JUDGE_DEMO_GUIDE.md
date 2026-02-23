# 🎮 EduEngine Game Demo - Complete Walkthrough for Judges

## Overview
This is a fully automated AI-powered educational game creation platform. The system generates complete game scenes with learning objectives.

---

## 🚀 Quick Start (3 Steps to Play)

### Step 1: Generate a Scene (with AI or use pre-made)
```bash
# Option A: Generate new scene with AI (requires API keys)
cd /home/aryan/Desktop/AI_PROJECT
python3 orchestrator.py workflow "teach Newton's 3 laws of motion for grade 7"

# Option B: Use pre-generated scene (NO API KEYS NEEDED)
# Scene already exists at: GameEngine/data/scenes/newtons_laws_grade7.json
```

### Step 2: Prepare Scene (Analyze & Generate Assets)
```bash
cd GameEngine
python3 prepare_scene.py data/scenes/newtons_laws_grade7.json
```

### Step 3: Launch Game
```bash
./EduEngine
# Then load the scene from the UI
# Or use: python3 game_launcher.py newtons_laws_grade7.json
```

---

## 📊 What Happens in Each Step

### PIPELINE ARCHITECTURE

```
┌─────────────────────────────────────────────────────────┐
│ 1. AI SCENE GENERATION (Optional - requires API keys)   │
│    - Input: Natural language prompt                     │
│    - Output: Complete scene JSON                        │
│    - Example: "teach photosynthesis for grade 5"        │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│ 2. ASSET ANALYSIS                                       │
│    - Analyzes which game assets are needed              │
│    - Checks which assets already exist                  │
│    - Identifies missing assets                          │
│    - Run: python3 analyze_scene_assets.py <scene.json>  │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│ 3. AUTO-GENERATE MISSING ASSETS                         │
│    - Creates placeholder sprites automatically          │
│    - Uses PIL (Pillow) library                          │
│    - Generates PNG files in correct directories         │
│    - Run: python3 generate_assets.py --scene <scene>    │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│ 4. VALIDATE & PREPARE                                   │
│    - Verifies JSON structure                            │
│    - Ensures all references are valid                   │
│    - Reports ready status                               │
│    - Run: python3 prepare_scene.py <scene>              │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│ 5. LAUNCH GAME ENGINE                                   │
│    - Loads EduEngine C++ binary                         │
│    - Initializes scene                                  │
│    - Player can learn and play                          │
└─────────────────────────────────────────────────────────┘
```

---

## 🎯 Available Pre-Made Demo Scenes

These scenes are ready to play WITHOUT any API keys or generation:

### 1. **Newton's 3 Laws of Motion** (Grade 7)
```
📁 data/scenes/newtons_laws_grade7.json
├─ Learn about: Inertia, F=ma, Action-Reaction
├─ 3 Collectible tokens to gather
├─ 2 Enemy obstacles
├─ 3 Info zones with facts
└─ Ready to play: YES ✅
```

**Launch:**
```bash
cd GameEngine
python3 prepare_scene.py data/scenes/newtons_laws_grade7.json
./EduEngine
```

### 2. **Solar System** (Grade 8)
```
📁 data/scenes/space___solar_system_grade6.json
├─ Learn about: Planets, Mercury, Venus, Earth, Mars, Jupiter, Saturn
├─ 6 Collectible planets
├─ Multiple platforms
├─ Info zones with facts
└─ Ready to play: YES ✅
```

### 3. **Photosynthesis** (Grade 5)
```
📁 data/scenes/photosynthesis_grade5.json
├─ Learn about: CO₂, H₂O, Glucose, Oxygen, Chlorophyll
├─ 5 Collectibles
├─ Info zones
└─ Ready to play: YES ✅
```

---

## 📝 Complete Scene JSON Format

All AI-generated scenes follow this format:

```json
{
  "scene_meta": {
    "id": "unique_scene_id",
    "title": "Learning Topic — Grade X",
    "subject": "physics|biology|chemistry|...",
    "grade": "5",
    "ai_prompt": "teach concept for grade 5"
  },
  "world_settings": {
    "gravity": 980.0,
    "background_color": [R, G, B, A]
  },
  "entities": [
    {
      "name": "Entity_Name",
      "tags": ["player|platform|collectible|enemy|info|goal|background"],
      "components": {
        "transform": { "x": 0, "y": 0, "scale_x": 1.0, "scale_y": 1.0 },
        "sprite": { "texture": "assets/images/theme/file.png", "color": [R,G,B,A], "layer": 0 },
        "rigid_body": { "mass": 1.0, "gravity_scale": 1.0 },
        "collider": { "width": 32, "height": 32, "is_trigger": false },
        "player_tag": { "move_speed": 220, "jump_force": -580, "max_jumps": 2 },
        "collectible": { "label": "Item", "point_value": 10 },
        "enemy": { "patrol_min_x": 0, "patrol_max_x": 100, "speed": 50 },
        "info_zone": { "text": "Learning fact here" },
        "goal": null,
        "text_label": { "text": "LABEL", "font_size": 14, "color": [R,G,B,A] }
      }
    }
  ]
}
```

### Physics Components Included

| Component | Purpose | Required Fields |
|-----------|---------|-----------------|
| `transform` | Position & scale | x, y |
| `sprite` | Visual appearance | texture, color, layer |
| `rigid_body` | Physics simulation | mass, gravity_scale |
| `collider` | Collision detection | width, height, is_trigger |
| `player_tag` | Player controls | move_speed, jump_force, max_jumps |
| `collectible` | Pickup items | label, point_value |
| `enemy` | AI opponents | patrol_min_x, patrol_max_x, speed |
| `info_zone` | Learning content | text |
| `goal` | Level completion | (null means goal reached) |

---

## 🎨 Asset Generation System

### Why Assets Matter
The game visual appearance depends on assets (PNG images). Without them, the game runs but looks plain. Our system automatically generates them!

### Available Asset Themes
- **physics** - Forces, motion, energy
- **biology** - Photosynthesis, cells, organisms
- **chemistry** - Molecules, reactions, atoms
- **space** - Planets, stars, solar system
- **history** - Ancient civilizations, events
- **mathematics** - Numbers, equations, shapes

### Asset File Structure
```
assets/images/
├── physics/
│   ├── background.png      (1280×720)
│   ├── player.png          (24×32)
│   ├── platform.png        (160×20)
│   ├── enemy.png           (32×48)
│   ├── collectible_*.png   (32×32)
│   ├── goal.png            (40×60)
│   └── info_sign.png       (48×48)
├── space/
│   ├── background.png
│   ├── player.png
│   └── ...
└── ... (other themes)
```

### How To Generate Assets

**Option 1: Automatic (Recommended)**
```bash
python3 prepare_scene.py data/scenes/my_scene.json
# Automatically analyzes and generates any missing assets
```

**Option 2: For Specific Theme**
```bash
python3 generate_assets.py --theme physics
# Generates all physics-related assets
```

**Option 3: For Specific Scene**
```bash
python3 generate_assets.py --scene data/scenes/newtons_laws_grade7.json
# Generates only what this scene needs
```

### Asset Analysis Without Generation
```bash
python3 analyze_scene_assets.py data/scenes/newtons_laws_grade7.json
# Shows:
# - Total assets needed
# - Which ones exist
# - Which ones are missing
# - How to generate them
```

---

## ⚙️ System Requirements

### For Scene Generation (Optional - Requires API Keys)
- OpenAI API key (gpt-4o model)
- *OR* Google Gemini API key (gemini-2.0-flash)
- *OR* Groq API key (llama-3.1-70b-versatile)

### For Asset Generation (Always Available)
- Python 3.8+
- PIL/Pillow (`pip install Pillow`)
- No internet required!

### For Running Game
- EduEngine binary compiled (already included)
- Graphics support (OpenGL 3.3+)
- No API keys needed

---

## 🎮 Controls In-Game

| Key | Action |
|-----|--------|
| A / D | Move left/right |
| SPACE | Jump |
| E | Interact with info signs |
| Collectibles | Automatic pickup on contact |
| Goal | Reach to complete level |

---

## 📚 Educational Content Example

### Newton's 3 Laws Scene

**Law 1 - Inertia**
- Statement: "An object at rest stays at rest, and an object in motion stays in motion unless acted upon by a force"
- In-game: Collectible to gather on first platform
- Info zone: Available for learning

**Law 2 - Force = mass × acceleration (F=ma)**
- Statement: "Force equals mass times acceleration. More force creates more acceleration!"
- In-game: Second platform with enemies representing resistance forces
- Info zone: Interactive learning point

**Law 3 - Action-Reaction**
- Statement: "For every action, there is an equal and opposite reaction. Forces always come in pairs!"
- In-game: Third platform, goal zone
- Learning: Apply concepts to reach goal

---

## 🔄 Workflow: From Prompt to Game

```
User Input:
"Teach Newton's 3 laws of motion for grade 7"
         ↓
[AI Agent - Uses Groq if OpenAI/Gemini fails]
         ↓
Generated JSON Scene:
{
  "entities": [...],
  "world_settings": {...},
  "scene_meta": {...}
}
         ↓
[Asset Analyzer]
Needs: physics/background.png, physics/platform.png, ...
         ↓
[Asset Generator - Auto-creates missing assets]
Creates: PNG images for all missing assets
         ↓
[Scene Validation]
Verifies all references and physics parameters
         ↓
[Game Engine Launch]
EduEngine loads scene and initializes gameplay
         ↓
Player learns while playing!
```

---

## 🎯 Demo Command Sequence (For Judges)

### Scenario: Show complete workflow

```bash
# 1. Show existing scene
cat GameEngine/data/scenes/newtons_laws_grade7.json | head -20

# 2. Analyze what assets are needed
cd GameEngine
python3 analyze_scene_assets.py data/scenes/newtons_laws_grade7.json

# 3. Prepare the scene (generate missing assets)
python3 prepare_scene.py data/scenes/newtons_laws_grade7.json

# 4. View the demo report
python3 demo_newtons_laws.py

# 5. Launch the game
./EduEngine
# (Then load: newtons_laws_grade7.json from UI)
```

---

## ✅ Verification Checklist

- [ ] Scene JSON exists and is valid
- [ ] All required assets are generated
- [ ] prepare_scene.py runs successfully
- [ ] Game engine binary exists
- [ ] EduEngine launches and loads scene
- [ ] Player can move and interact
- [ ] Collectibles can be collected
- [ ] Info zones display facts
- [ ] Goal can be reached

---

## 🐛 Troubleshooting

### Issue: Missing assets
```bash
# Solution: Regenerate
python3 generate_assets.py --scene data/scenes/your_scene.json
```

### Issue: Scene not loading in game
```bash
# Solution: Validate first
python3 prepare_scene.py data/scenes/your_scene.json
```

### Issue: Game engine won't launch
```bash
# Check if compiled
ls -la EduEngine
# Make executable
chmod +x EduEngine
# Try running
./EduEngine
```

---

## 📊 Statistics

- **Scenes Available**: 5+ pre-made scenes
- **Auto-Generatable Themes**: 10+ educational subjects
- **Fallback AI Models**: 3 (OpenAI → Gemini → Groq)
- **Asset Types**: 9 (background, player, platforms, enemies, collectibles, etc.)
- **Physics Simulation**: Full 2D rigid body dynamics
- **Playable Without API Keys**: YES ✅

---

## 🎓 For Judges: Key Innovation Points

1. **No Manual Asset Creation** - Everything generated automatically
2. **AI-Powered Learning Design** - AI creates educational content
3. **Zero API Key Requirement** - Works offline after scene generation
4. **Complete Fallback Chain** - Multiple AI providers + local generation
5. **Physics-Based Learning** - Real 2D physics simulation
6. **Instant Customization** - Any educational concept → Playable game in minutes

---

## 📞 Questions?

- **Scene generation fails?** Check API keys in `.env`
- **Assets missing?** Run `py prepare_scene.py <scene>`
- **Game won't start?** Verify EduEngine binary exists
- **Need new theme?** Add to `generate_assets.py` and regenerate

Enjoy the demo!
