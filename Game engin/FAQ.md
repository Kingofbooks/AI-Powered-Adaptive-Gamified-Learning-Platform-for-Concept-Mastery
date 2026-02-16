# Frequently Asked Questions (FAQ)

## 🤔 General Questions

### What is this software?

The Educational Physics Sandbox is a complete desktop application that lets teachers create interactive physics simulations. You can either type simple commands like "create ball" or use your mouse to click and drag objects. It's designed specifically for teachers with no programming background.

### Do I need to know programming?

**No!** That's the whole point. You can:
- Click buttons to create objects
- Drag them with your mouse
- Type simple English commands
- Never write a single line of code

### What operating systems does it work on?

- ✅ Ubuntu/Debian Linux
- ✅ Fedora/RHEL Linux
- ✅ macOS (with Homebrew)
- ⚠️ Windows (possible with MinGW, untested)
- ❌ Chromebooks (unless in Linux mode)

### Is it free?

Yes! Completely free for educational use. Teachers can use, modify, and share it freely.

### Do I need internet to run it?

No! Once installed, it runs completely offline. Perfect for schools with limited connectivity.

---

## 💻 Installation Questions

### How do I install it?

**Quick version:**
```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
make
./EduSandbox

# macOS
brew install sdl2 sdl2_ttf
make
./EduSandbox
```

**Even easier:**
```bash
chmod +x build.sh
./build.sh
```

The build script does everything automatically!

### What is SDL2?

SDL2 (Simple DirectMedia Layer 2) is a free graphics library that makes the window, graphics, and mouse work. Think of it like the foundation that lets us draw pretty pictures on your screen.

### I get "SDL2 not found" - what do I do?

You need to install SDL2 first:

```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev libsdl2-ttf-dev

# macOS (requires Homebrew)
brew install sdl2 sdl2_ttf

# Fedora
sudo dnf install SDL2-devel SDL2_ttf-devel
```

### I get "Font not found" warnings

The program will work fine without fonts (just no text will show). To fix:

```bash
# Ubuntu/Debian
sudo apt-get install fonts-dejavu-core
```

Or edit `main.cpp` line ~40 to point to a font on your system.

### Can I install this on a school computer?

You'll need administrator rights to install SDL2. Options:
1. Ask IT department to install SDL2
2. Use it on your personal computer
3. Run on a computer where you have admin access

### How much disk space does it need?

Tiny! The whole project is about 100KB of source code, and compiles to a 200-300KB executable. SDL2 itself is about 2-5MB.

---

## 🎮 Usage Questions

### How do I create objects?

**Three ways:**

1. **Click buttons** (easiest)
   - Click "Ball", "Block", "Ground", or "Ramp"
   - Object appears in center

2. **Drag and drop**
   - Create object first
   - Click and drag it anywhere

3. **Type commands**
   - Click text box (or press Enter)
   - Type: `create red ball at top`
   - Press Enter

### How do I move objects?

Click and drag! Just click any object and move your mouse while holding the button down.

### What commands can I type?

**Basic commands:**
```
create ball
create ball at top
create red ball
create blue block at center
create ground at bottom
create ramp at left
```

**Explanations:**
```
explain gravity
explain collision
explain velocity (select object first)
explain energy (select object first)
```

**Other:**
```
clear   (removes everything)
```

Press `H` in the program for full help!

### What colors can I use?

`red`, `blue`, `green`, `yellow`, `orange`, `purple`, `brown`, `gray`, `white`, `black`

Example: `create yellow ball at top`

### What positions can I use?

**Named positions:**
- `at top` - Top of screen
- `at bottom` - Bottom of screen
- `at left` - Left side
- `at right` - Right side
- `at center` - Middle

**Exact coordinates:**
- `at 400 300` - Position (400, 300)

### How do I pause the simulation?

Press `Space` or `P` on your keyboard.

### How do I make it go faster or slower?

- Press `+` or `=` to speed up
- Press `-` to slow down

Speed ranges from 10% to 300% of normal.

### How do I delete everything?

Press `R` or type `clear`

This removes all objects but keeps one ground.

### Can I save my simulations?

Not in version 2.0 (current). Planned for future version!

For now, you can:
- Screenshot them
- Write down the commands you used
- Copy-paste commands from a text file

---

## 🎓 Teaching Questions

### What age group is this for?

**Recommended:** Grades 6-12 (ages 11-18)

But it can work for:
- Elementary (grades 4-5) - Simple demonstrations
- College intro physics - Quick visualizations
- Adult learners - Concept reinforcement

### What topics can I teach with it?

**Physics:**
- Gravity and free fall
- Velocity and acceleration
- Newton's laws of motion
- Collisions and momentum
- Energy and energy transfer
- Forces and motion
- Projectile motion (with ramps)

**Not covered yet:**
- Electricity and magnetism
- Waves and sound
- Thermodynamics
- Quantum mechanics

### How do I use it in my lesson?

**Simple 3-step process:**

1. **Show** - Create simulation, run it
2. **Pause** - Stop at key moment
3. **Explain** - Use built-in explanations or your own

See TEACHING_GUIDE.md for complete lesson plans!

### Can students use it themselves?

Yes! Three scenarios:

**Scenario 1: Whole class demo**
- Teacher controls, projects to class
- Students suggest what to create
- Very interactive

**Scenario 2: Computer lab**
- Each student (or pair) at a computer
- Follow structured activities
- Teacher circulates

**Scenario 3: Rotation stations**
- 1-2 computers in classroom
- Groups rotate through
- 10-15 min per group

### Do I need to be a tech expert?

No! If you can:
- Open a program
- Click buttons
- Type simple commands

You're good to go!

### What if something breaks during class?

**Quick fixes:**
- Press `R` to reset
- Close and reopen program
- Worst case: Restart computer

The program is very stable and rarely crashes.

---

## 🔧 Technical Questions

### What programming language is it written in?

C++17 with SDL2 for graphics.

### Can I modify the code?

Yes! It's designed to be extensible:
- Add new entity types
- Change physics parameters
- Modify colors and appearance
- Create new commands

### How does the physics work?

**Simplified but accurate:**
- Gravity: Constant downward acceleration
- Collisions: Impulse-based resolution
- Energy: Not perfectly conserved (realistic!)
- Time step: Fixed delta time

Good enough for teaching, not research-grade simulation.

### Is the physics accurate?

**Accurate concepts, simplified numbers:**
- ✓ All objects fall at same rate
- ✓ Energy decreases in bounces
- ✓ Momentum conserved in collisions
- ✓ Objects accelerate due to gravity

But:
- No air resistance
- Simplified friction
- 2D only (not 3D)
- Scaled for screen display

### Can I add more objects?

Yes! See EXTENSION_GUIDE.md (if available) or:

1. Define new entity type in `EduSandbox.h`
2. Implement rendering in `Entities.cpp`
3. Add keyword in `PromptParser.cpp`
4. Handle in collision detection

### What's the frame rate?

60 FPS (frames per second) with V-Sync.

Smooth and responsive!

### How many objects can it handle?

**Recommended:** 10-20 objects
**Maximum:** 50-100 objects (may slow down)

Performance depends on your computer.

---

## 🐛 Troubleshooting

### The window doesn't open

**Possible causes:**

1. **SDL2 not installed**
   ```bash
   sdl2-config --version  # Check if installed
   ```

2. **No display available**
   - Are you on a server without GUI?
   - This needs a graphical environment

3. **Build failed**
   - Did compilation succeed?
   - Check for error messages

### Objects fall through the ground

**Solution:** Ground must be BELOW the objects.

```
# Good
create ground at bottom
create ball at top

# Bad - ball starts below ground
create ground at top
create ball at bottom
```

Or drag the ground down with your mouse!

### I can't type in the text box

**Try:**
1. Click the text box first
2. Or press Enter key
3. Look for the cursor (`|`) in the box

### Commands don't work

**Common mistakes:**

```
# Wrong
creat ball        ❌ Typo
create bal        ❌ Typo
ball              ❌ Missing "create"

# Right
create ball       ✓
create red ball   ✓
create ball at top ✓
```

Check spelling carefully!

### The help says wrong key

Press `H` (uppercase or lowercase both work) to toggle help overlay.

### Simulation runs too slow

**Solutions:**
1. Close other programs
2. Create fewer objects
3. Reduce window size (edit `main.cpp`)
4. Use older/simpler computer

### Simulation runs too fast

Press `-` (minus key) multiple times to slow it down.

### Objects disappear off screen

They fell off the bottom! Add ground:

```
create ground at bottom
```

Or press `R` to reset.

### Text doesn't show

Font not found. The program works fine without text showing.

To fix:
```bash
# Ubuntu/Debian
sudo apt-get install fonts-dejavu-core
```

Or edit `main.cpp` to use a font on your system.

### Build fails with errors

**Common issues:**

1. **SDL2 not installed**
   ```bash
   sudo apt-get install libsdl2-dev libsdl2-ttf-dev
   ```

2. **Compiler too old**
   - Need C++17 support
   - Update g++ to version 7+

3. **Missing Makefile**
   - Make sure you're in the right directory
   - Should have all .cpp and .h files

### Window is too big for my screen

Edit `main.cpp`:

```cpp
// Change these lines (around line 10)
const int SCREEN_WIDTH = 1200;   // Make smaller, like 800
const int SCREEN_HEIGHT = 700;   // Make smaller, like 600
```

Then rebuild: `make clean && make`

---

## 📱 Platform-Specific Questions

### Does it work on Chromebooks?

**Maybe.** If your Chromebook supports Linux apps:
1. Enable Linux (beta)
2. Install SDL2 in Linux environment
3. Build and run

Not officially supported but may work!

### Does it work on Windows?

**Probably.** You'd need:
1. MinGW or MSYS2 for build tools
2. SDL2 for Windows
3. Some path adjustments

Not officially tested. Linux or Mac recommended.

### Does it work on Raspberry Pi?

**Should work!** Raspberry Pi OS is Debian-based:

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
make
./EduSandbox
```

May be slower on older Pi models.

### Can I run it on a tablet?

No, this is desktop software. Would need complete rewrite for touch interfaces.

---

## 📊 Comparison Questions

### How is this different from PhET simulations?

**PhET:**
- Web-based
- Pre-made simulations
- Can't create custom scenarios
- Very polished

**This software:**
- Desktop application
- Create any scenario you want
- Type commands or drag-drop
- Educational focus on simplicity

Both are great! Use both!

### How is this different from Algodoo?

**Algodoo:**
- More realistic physics
- Drawing tools
- Complex mechanisms
- Steeper learning curve

**This software:**
- Simpler, faster to learn
- Command-based for reproducibility
- Designed for teachers, not engineers
- Lightweight

### How is this different from Unity or Unreal?

Those are professional game engines requiring:
- Programming knowledge
- Weeks to learn
- Complex interfaces

This is:
- No programming needed
- 5 minutes to learn
- Simple interface
- Education-focused

---

## 🔄 Future Development

### Will there be updates?

Possibly! Planned features:
- Save/load simulations
- More object types
- Spring connections
- Graph plotting
- Better Windows support

### Can I contribute?

Yes! The code is open. You can:
- Add features
- Fix bugs
- Improve documentation
- Share with other teachers

### Will there be a mobile version?

Not planned. Desktop is much better for:
- Precise positioning
- Keyboard shortcuts
- Typing commands
- Classroom projection

---

## 💬 Getting Help

### Where can I get more help?

1. **Press H** in the program
2. **Read README.md** for full guide
3. **Check EXAMPLES.md** for scenarios
4. **Review TEACHING_GUIDE.md** for lessons
5. **Look at the code** - it's well-commented!

### Who do I contact for support?

This is free educational software without formal support. But:
- Read the documentation files
- Check this FAQ
- Review the code comments
- Ask a tech-savvy colleague

### Can I report bugs?

Document them! If you find issues:
- Note what you did
- What happened vs. what you expected
- Your OS and SDL2 version
- Share with other users

### Is there a community?

Not yet! You're early adopters. Consider:
- Starting a teacher forum
- Sharing lessons online
- Creating tutorial videos
- Spreading the word!

---

## 🎯 Quick Reference

### Install
```bash
./build.sh        # Auto-install everything
# OR
make              # If SDL2 already installed
```

### Run
```bash
./EduSandbox
```

### First Demo
```
1. Click "Ball"
2. Click "Ground"
3. Press Space to pause
4. Type: explain gravity
```

### Need Help
```
Press H           # In-program help
cat README.md     # Full documentation
cat EXAMPLES.md   # Ready-to-use scenarios
```

---

**Still have questions? Check the documentation files or experiment - you can't break anything!**
