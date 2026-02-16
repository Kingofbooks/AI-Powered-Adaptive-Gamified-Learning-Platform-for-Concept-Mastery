# Educational Physics Sandbox - Graphical Desktop Version

A beautiful, interactive 2D physics simulation engine with a complete graphical interface. Designed for teachers with no programming background to create engaging physics demonstrations.

![Version](https://img.shields.io/badge/version-2.0-blue)
![Language](https://img.shields.io/badge/language-C++-orange)
![Graphics](https://img.shields.io/badge/graphics-SDL2-green)

---

## ✨ Features

### Two Ways to Create Simulations

1. **Natural Language Commands**
   - Type simple English: "create red ball at top"
   - Instant results, predictable behavior
   - No programming knowledge needed

2. **Drag-and-Drop Interface**
   - Click buttons to create objects
   - Drag objects with your mouse
   - Rearrange on the fly

### Beautiful Graphics

- Smooth animations at 60 FPS
- Realistic shadows and highlights
- Color-coded objects
- Grid background for reference
- Professional UI design

### Real Physics

- Accurate gravity simulation
- Realistic collisions with bounce
- Energy conservation
- Support for ramps and slopes
- Velocity vectors visualization

---

## 🚀 Quick Start

### Installation

#### Ubuntu/Debian
```bash
# Install SDL2
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-ttf-dev

# Build
make

# Run
./EduSandbox
```

#### macOS
```bash
# Install SDL2
brew install sdl2 sdl2_ttf

# Build
make

# Run
./EduSandbox
```

#### Fedora/RHEL
```bash
# Install SDL2
sudo dnf install SDL2-devel SDL2_ttf-devel

# Build
make

# Run
./EduSandbox
```

### First Steps

1. Launch the program: `./EduSandbox`  
   (the app opens fullscreen by default — press `F` to toggle windowed mode)
2. Click "Ball" button to create a ball
3. Watch it fall and bounce!
4. Drag it around with your mouse
5. Try typing: `create red ball at top`

---

## 🎮 User Interface

### Main Window Layout

```
┌─────────────────────────────────────────┐
│  MESSAGE BOX (appears when needed)      │
│                                         │
│                                         │
│                                         │
│         SIMULATION AREA                 │
│      (white with grid)                  │
│                                         │
│     Drag objects here                   │
│                                         │
│                                         │
├─────────────────────────────────────────┤
│ [Ball] [Block] [Ground] [Ramp]         │
│                                         │
│ [Type command here...    ] STATUS       │
└─────────────────────────────────────────┘
```

### Mouse Controls

- **Left Click**: Select object
- **Click + Drag**: Move object
- **Click Toolbar**: Create object
- **Click Prompt**: Type command

### Learning Mode (G)

- Press `G` to open the built-in Learning Mode overlay (levels, challenges, reflection questions).
- Press `Enter` to start typing your answer in the prompt box; submit with `Enter` again.
- The engine evaluates conceptual explanations (not only correctness), detects common misconceptions, awards points/badges, generates reinforcement exercises when needed, and unlocks the next level on mastery.
- Progress is persisted to `player_progress.txt` so student progress and badges are retained between runs.

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `ENTER` | Activate command prompt |
| `Space` or `P` | Pause/Resume simulation |
| `F` | Toggle fullscreen |
| `W/A/S/D` or Arrow keys | Apply directional force to selected object |
| `I / K` | Increase / decrease mass of selected object |
| `O` | Toggle gravity for selected object |
| `U` | Apply a small upward impulse to selected object |
| `1`–`5` | Jump to stage background (useful for Learning Mode testing) |
| `+` or `=` | Speed up time |
| `-` | Slow down time |
| `R` | Reset (clear all) |
| `H` | Show/Hide help |
| `G` | Toggle Learning Mode (open overlay with levels; press Enter to answer) |

---

## 📝 Commands

### Creating Objects

```
create ball                    # Red ball in center
create ball at top             # Ball at top
create blue block              # Blue square
create ground at bottom        # Floor platform
create ramp at left            # Sloped surface
create yellow ball at 400 200  # Specific position
```

### Available Objects

1. **Ball** - Bouncy sphere (default red)
2. **Block** - Rectangular solid (default blue)
3. **Ground** - Static platform (brown)
4. **Ramp** - Sloped surface (gray)

### Positions

- `at top` - Top of screen
- `at bottom` - Bottom of screen
- `at left` - Left side
- `at right` - Right side
- `at center` - Middle
- `at X Y` - Exact coordinates

### Colors

Available: `red`, `blue`, `green`, `yellow`, `orange`, `purple`, `brown`, `gray`, `white`, `black`

```
create red ball
create blue block
create yellow ball at top
```

### Explanations

```
explain gravity       # Learn about gravity
explain collision     # Understand bouncing
explain velocity      # See speed info (select object first)
explain energy        # Learn about kinetic energy
```

### Other Commands

```
clear                 # Remove all objects
reset                 # Same as clear
```

---

## 🎓 For Teachers

### Quick Demo (5 minutes)

1. **Open program**
2. **Click "Ball"** → Ball appears and falls
3. **Press P** → Pause
4. **Type**: `explain gravity`
5. **Press P** → Resume
6. **Click "Ground"** → Floor appears
7. **Drag ball to top** → Watch bounce!

### Lesson Plan: Gravity (20 min)

**Setup:**
- Projector connected to computer
- Students can see screen

**Procedure:**

1. **Introduction (3 min)**
   - Launch program
   - Ask: "What happens when I drop this?"
   - Click "Ball" button

2. **Observation (5 min)**
   - Students watch ball fall and bounce
   - Press P to pause mid-fall
   - Type: `explain gravity`
   - Discuss acceleration

3. **Experimentation (7 min)**
   - Type: `create red ball at left`
   - Type: `create blue ball at right`
   - Ask: "Which falls faster?"
   - Answer: Same! (Galileo's discovery)

4. **Exploration (5 min)**
   - Create multiple balls
   - Different colors, different positions
   - All fall at same rate

5. **Wrap-up (2 min)**
   - Key point: Gravity is constant
   - Preview next lesson: Collisions

### Lesson Plan: Energy Transfer (25 min)

**Learning Objective:** Understand energy transfer in collisions

**Procedure:**

1. **Setup (5 min)**
   ```
   create ground at bottom
   create red ball at 300 100
   create blue ball at 600 100
   ```

2. **Predict (3 min)**
   - Ask students: "What will happen?"
   - Write predictions

3. **Observe (5 min)**
   - Press Space to start
   - Watch balls bounce
   - Pause when balls collide: `P`
   - Type: `explain collision`

4. **Analyze (7 min)**
   - Why do bounces get lower?
   - Where does energy go?
   - Type: `explain energy` (with ball selected)
   - Discuss kinetic energy formula

5. **Extension (5 min)**
   - Create ramp: Click "Ramp"
   - Place ball at top
   - Watch it roll down
   - Discuss potential→kinetic energy

---

## 🔧 Technical Details

### System Requirements

**Minimum:**
- CPU: Any x86_64 processor
- RAM: 128MB
- Graphics: Basic OpenGL support
- OS: Linux, macOS, or Windows

**Recommended:**
- CPU: 1GHz dual-core
- RAM: 256MB
- Graphics: GPU with OpenGL 2.0+
- Display: 1024x768 or higher

### Architecture

```
main.cpp              - Application & GUI
├── EduSandbox.h      - Entity definitions
├── Entities.cpp      - Rendering logic
├── PhysicsEngine.h/cpp - Physics simulation
└── PromptParser.h/cpp  - Command parsing
```

### File Structure

- `EduSandbox.h` - Core data structures, entity classes
- `Entities.cpp` - SDL2 rendering for each entity type
- `PhysicsEngine.h/cpp` - Gravity, collisions, explanations
- `PromptParser.h/cpp` - Natural language interpretation
- `main.cpp` - GUI, event handling, main loop
- `Makefile` - Build system

---

## 🎨 Customization

### Changing Physics

Edit `PhysicsEngine.cpp`:

```cpp
// Make gravity stronger
PhysicsEngine::PhysicsEngine() 
    : gravity(0, 600.0f)  // Increased from 400
```

### Changing Colors

Edit `Entities.cpp`:

```cpp
Ball::Ball(const Vector2& pos, float radius)
    : Entity(EntityType::BALL, pos), radius(radius) {
    color = Color::Purple();  // Changed from Red
```

### Adding New Shapes

1. Define in `EduSandbox.h`
2. Implement rendering in `Entities.cpp`
3. Add keyword in `PromptParser.cpp`
4. Handle creation in `main.cpp`

---

## 📖 Examples

### Build a Marble Run

```
create ground at 300 400
create ramp at 100 200
create ramp at 400 300
create ball at 50 100
```

### Newton's Cradle (simplified)

```
create ball at 200 300
create ball at 300 300
create ball at 400 300
create ball at 500 300
create ball at 600 300
```

Drag first ball up and release!

### Bouncing Ball Demo

```
create ground at bottom
create ball at top
explain gravity
```

Watch, pause (P), explain, resume!

---

## 🐛 Troubleshooting

### "SDL2 not found"
```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev libsdl2-ttf-dev

# macOS
brew install sdl2 sdl2_ttf

# Fedora
sudo dnf install SDL2-devel SDL2_ttf-devel
```

### "TTF_OpenFont failed"
The program will work but without text. Font paths are:
- Linux: `/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf`
- macOS: `/System/Library/Fonts/Helvetica.ttc`

Install DejaVu fonts or edit `main.cpp` with your font path.

### Objects fall through ground
- Ground must be lower than objects
- Use: `create ground at bottom`
- Or drag ground to bottom of screen

### Simulation runs slow
- Close other programs
- Reduce window size (edit `SCREEN_WIDTH` in main.cpp)
- Build with: `make` (already optimized)

---

## 🔄 Comparison: Console vs Graphical Version

| Feature | Console (v1.0) | Graphical (v2.0) |
|---------|---------------|------------------|
| Graphics | ASCII art | Beautiful SDL2 |
| Mouse | No | Yes, drag & drop |
| Keyboard | Yes | Yes + shortcuts |
| Commands | Yes | Yes |
| Performance | Very fast | Fast (60 FPS) |
| Visual Appeal | Low | High |
| Ease of Use | Medium | Very Easy |

**Recommendation:** Use graphical version for classroom demonstrations!

---

## 📋 Version History

**v2.0** (Current - Graphical)
- Complete SDL2 graphical interface
- Mouse drag-and-drop support
- Beautiful rendering with shadows
- Toolbar with buttons
- Help overlay
- Message notifications
- Ramp support

**v1.0** (Console)
- ASCII terminal rendering
- Keyboard-only controls
- Basic entities
- Command-line interface

---

## 📄 License

Free for educational use. Teachers may use, modify, and share this software.

---

## 👥 Credits

Created for physics teachers who want engaging, visual demonstrations without learning to code.

Special thanks to:
- SDL2 development team
- Teachers who need better tools
- Students learning physics

---

## 📞 Support

For questions:
1. Press `H` in the program for help
2. Read this README
3. Check the examples section
4. Review the code comments

---

**Built for Education. Designed for Clarity. Made with ❤️**
