# Quick Installation Guide

## 1-Minute Setup

### Ubuntu/Debian
```bash
# Install dependencies
sudo apt-get update && sudo apt-get install -y libsdl2-dev libsdl2-ttf-dev g++ make

# Build
make

# Run
./EduSandbox
```

### macOS
```bash
# Install dependencies (requires Homebrew)
brew install sdl2 sdl2_ttf

# Build
make

# Run
./EduSandbox
```

### Fedora/RHEL
```bash
# Install dependencies
sudo dnf install -y SDL2-devel SDL2_ttf-devel gcc-c++ make

# Build
make

# Run
./EduSandbox
```

---

## What You Need

1. **SDL2 library** - Graphics
2. **SDL2_ttf library** - Text rendering
3. **C++ compiler** - g++ or clang
4. **Make** - Build tool

Most Linux systems have g++ and make already installed.

---

## First Run

After running `./EduSandbox`, you'll see a window with:
- White grid area (simulation space)
- Dark toolbar at bottom
- Buttons: Ball, Block, Ground, Ramp

**Try This:**
1. Click "Ball" button
2. Watch ball fall
3. Click "Ground" button  
4. Watch ball bounce!

**Or Type:**
1. Click in the text box at bottom
2. Type: `create red ball at top`
3. Press Enter

---

## Troubleshooting

### "SDL2 not found" during build
You forgot to install SDL2. See installation commands above.

### "Font not found" warning
Program works but text won't show. To fix:
```bash
# Ubuntu/Debian
sudo apt-get install fonts-dejavu-core

# macOS - fonts already installed
# Program should work fine
```

### Window doesn't open
Check if you have a display:
```bash
echo $DISPLAY    # Linux - should show :0 or similar
# macOS - always works
```

For remote servers without display, this won't work.
Use on local machine or VM with GUI.

---

## Files Included

- `EduSandbox.h` - Core structures
- `Entities.cpp` - Object rendering  
- `PhysicsEngine.h` - Physics header
- `PhysicsEngine.cpp` - Physics code
- `PromptParser.h` - Parser header
- `PromptParser.cpp` - Command parsing
- `main.cpp` - Main program + GUI
- `Makefile` - Build instructions
- `README.md` - Full documentation
- `INSTALL.md` - This file

---

## Quick Test

After installation, test everything works:

```bash
# Build
make

# Should show: "Build complete! Run with ./EduSandbox"

# Run
./EduSandbox

# Should open a window

# Try these in order:
# 1. Click "Ball" button
# 2. Click "Ground" button  
# 3. Press P (pause)
# 4. Press H (help)
# 5. Press Space (resume)
```

If all that works, you're ready to teach!

---

## Next Steps

1. Read README.md for full documentation
2. Try the example lesson plans
3. Experiment with commands
4. Create your own simulations

---

## Common Issues

**Q: Screen is blank after starting**
A: Ground is created by default. Click "Ball" to see action.

**Q: Can't type in text box**
A: Click the text box first, or press Enter.

**Q: Objects fly off screen**
A: Add ground first: Click "Ground" button or type `create ground at bottom`

**Q: Program crashes on start**
A: Check SDL2 is installed correctly:
```bash
sdl2-config --version    # Should show version number
```

---

**You're ready to make physics come alive!**

For detailed teaching guides and examples, see README.md
