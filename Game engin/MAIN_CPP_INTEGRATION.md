# main.cpp Integration Guide

## 🚨 IMPORTANT: GameEngine Code is Complete!

**All features ARE implemented** in GameEngine.h and GameEngine.cpp:
- ✅ Blank screen rendering
- ✅ Prompt input system
- ✅ Character control
- ✅ Prompt processing
- ✅ AI integration ready

**BUT**: main.cpp needs to be updated to CALL these methods!

---

## 📋 Required Changes to main.cpp

### 1. Check Game Mode in Render Loop

**Find this in main.cpp** (around line 610-665):
```cpp
// Current code renders background then UI
gameEngine.renderBackground(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
if (learningMode) {
    gameEngine.renderUI(renderer, font, smallFont, ...);
}
```

**Change to**:
```cpp
// Check game mode first!
GameMode currentMode = gameEngine.getGameMode();

if (currentMode == GameMode::BLANK_SCREEN) {
    // Render blank screen
    gameEngine.renderBlankScreen(renderer, font, smallFont, SCREEN_WIDTH, SCREEN_HEIGHT);
}
else if (currentMode == GameMode::PROMPT_INPUT) {
    // Render prompt input
    gameEngine.renderPromptInputScreen(renderer, font, smallFont, SCREEN_WIDTH, SCREEN_HEIGHT);
}
else if (currentMode == GameMode::PLAYING) {
    // Render background + character
    gameEngine.renderBackground(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    gameEngine.renderPlayerCharacter(renderer);
}
else if (currentMode == GameMode::LEARNING_MODE) {
    // Traditional learning mode
    gameEngine.renderBackground(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    gameEngine.renderUI(renderer, font, smallFont, panelX, panelY, panelW, panelH);
}
```

---

### 2. Handle Keyboard Input for Prompt

**Find the SDL event loop** (around line 70-200):
```cpp
while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
        running = false;
    }
    // ... existing key handling
}
```

**Add BEFORE existing key handling**:
```cpp
GameMode currentMode = gameEngine.getGameMode();

// Handle prompt input mode
if (currentMode == GameMode::PROMPT_INPUT) {
    if (event.type == SDL_TEXTINPUT) {
        // Add typed character
        gameEngine.addCharToPrompt(event.text.text[0]);
    }
    else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN) {
            gameEngine.submitPrompt();
        }
        else if (event.key.keysym.sym == SDLK_BACKSPACE) {
            gameEngine.removeCharFromPrompt();
        }
        else if (event.key.keysym.sym == SDLK_ESCAPE) {
            gameEngine.setGameMode(GameMode::BLANK_SCREEN);
        }
    }
    continue; // Skip other input handling
}

// Handle blank screen mode
if (currentMode == GameMode::BLANK_SCREEN) {
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
        SDL_StartTextInput(); // Enable text input
        gameEngine.setGameMode(GameMode::PROMPT_INPUT);
    }
    continue;
}

// ... rest of existing key handling for PLAYING/LEARNING_MODE
```

---

### 3. Update Player Character

**Find the update loop** (around line 250):
```cpp
// Update physics
physicsEngine.update(deltaTime);

// Add this:
gameEngine.updatePlayerCharacter(deltaTime);
```

---

### 4. Handle Character Control Input

**In the SDL_KEYDOWN handling** (around line 100-180):
```cpp
if (currentMode == GameMode::PLAYING) {
    // Get current key states
    const Uint8* keyState = SDL_GetKeyboardState(NULL);

    bool left = keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A];
    bool right = keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D];
    bool up = keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W];
    bool down = keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S];
    bool jump = keyState[SDL_SCANCODE_SPACE];

    gameEngine.handlePlayerInput(left, right, up, down, jump);

    // ESC to return to prompt
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
        SDL_StopTextInput();
        gameEngine.setGameMode(GameMode::BLANK_SCREEN);
    }

    // G to toggle learning mode
    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_g) {
        gameEngine.setGameMode(GameMode::LEARNING_MODE);
    }
}
```

---

## 🔧 Complete main.cpp Patch

Here's a complete patch showing all changes:

```cpp
// ========== AFTER SDL_Init, BEFORE main loop ==========

// Enable text input initially (will be toggled)
SDL_StopTextInput(); // Start with text input disabled

// ========== IN MAIN LOOP, EVENT HANDLING ==========

while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
        running = false;
        continue;
    }

    // Get current game mode
    GameMode currentMode = gameEngine.getGameMode();

    // ===== BLANK SCREEN MODE =====
    if (currentMode == GameMode::BLANK_SCREEN) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                SDL_StartTextInput();
                gameEngine.setGameMode(GameMode::PROMPT_INPUT);
            }
        }
        continue;
    }

    // ===== PROMPT INPUT MODE =====
    if (currentMode == GameMode::PROMPT_INPUT) {
        if (event.type == SDL_TEXTINPUT) {
            gameEngine.addCharToPrompt(event.text.text[0]);
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_RETURN) {
                SDL_StopTextInput();
                gameEngine.submitPrompt();
                // Mode will change to PLAYING in submitPrompt()
            }
            else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                gameEngine.removeCharFromPrompt();
            }
            else if (event.key.keysym.sym == SDLK_ESCAPE) {
                SDL_StopTextInput();
                gameEngine.setGameMode(GameMode::BLANK_SCREEN);
            }
        }
        continue;
    }

    // ===== PLAYING MODE =====
    if (currentMode == GameMode::PLAYING) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                gameEngine.clearScene();
                gameEngine.setGameMode(GameMode::BLANK_SCREEN);
            }
            else if (event.key.keysym.sym == SDLK_g) {
                gameEngine.setGameMode(GameMode::LEARNING_MODE);
            }
        }
        continue;
    }

    // ===== LEARNING MODE (existing behavior) =====
    if (currentMode == GameMode::LEARNING_MODE) {
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_g) {
                gameEngine.setGameMode(GameMode::PLAYING);
            }
            else if (event.key.keysym.sym == SDLK_ESCAPE) {
                gameEngine.setGameMode(GameMode::BLANK_SCREEN);
            }
            // ... rest of existing learning mode input
        }
        continue;
    }

    // ... existing event handling for other modes
}

// ========== IN MAIN LOOP, UPDATE SECTION ==========

// Update game engine (particles, animations)
gameEngine.update(deltaTime);

// Update player character
gameEngine.updatePlayerCharacter(deltaTime);

// Handle continuous input for PLAYING mode
GameMode currentMode = gameEngine.getGameMode();
if (currentMode == GameMode::PLAYING) {
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    bool left = keyState[SDL_SCANCODE_LEFT] || keyState[SDL_SCANCODE_A];
    bool right = keyState[SDL_SCANCODE_RIGHT] || keyState[SDL_SCANCODE_D];
    bool up = keyState[SDL_SCANCODE_UP] || keyState[SDL_SCANCODE_W];
    bool down = keyState[SDL_SCANCODE_DOWN] || keyState[SDL_SCANCODE_S];
    bool jump = keyState[SDL_SCANCODE_SPACE];

    gameEngine.handlePlayerInput(left, right, up, down, jump);
}

// Update physics
physicsEngine.update(deltaTime);

// ========== IN MAIN LOOP, RENDER SECTION ==========

// Clear screen
SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
SDL_RenderClear(renderer);

// Render based on game mode
GameMode currentMode = gameEngine.getGameMode();

if (currentMode == GameMode::BLANK_SCREEN) {
    gameEngine.renderBlankScreen(renderer, font, smallFont, SCREEN_WIDTH, SCREEN_HEIGHT);
}
else if (currentMode == GameMode::PROMPT_INPUT) {
    gameEngine.renderPromptInputScreen(renderer, font, smallFont, SCREEN_WIDTH, SCREEN_HEIGHT);
}
else if (currentMode == GameMode::PLAYING) {
    gameEngine.renderBackground(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Render physics entities
    for (auto& entity : physicsEngine.getEntities()) {
        entity->render(renderer);
    }
    // Render player character on top
    gameEngine.renderPlayerCharacter(renderer);
}
else if (currentMode == GameMode::LEARNING_MODE) {
    gameEngine.renderBackground(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Render physics entities
    for (auto& entity : physicsEngine.getEntities()) {
        entity->render(renderer);
    }
    // Render UI panel
    int panelX = SCREEN_WIDTH - 400;
    int panelY = 10;
    int panelW = 380;
    int panelH = SCREEN_HEIGHT - 20;
    gameEngine.renderUI(renderer, font, smallFont, panelX, panelY, panelW, panelH);
}

// Present
SDL_RenderPresent(renderer);
```

---

## ✅ What This Does

1. **Blank Screen**: Shows on startup, press ENTER to continue
2. **Prompt Input**: Type prompt, ENTER to submit, ESC to cancel
3. **Playing Mode**: Character control, physics simulation, ESC to exit
4. **Learning Mode**: Traditional quiz, G to toggle

---

## 🚀 Quick Integration (Copy-Paste Sections)

You need to modify **3 sections** of main.cpp:

1. **Event Handling** - Add mode checking and input handling
2. **Update Loop** - Add `gameEngine.updatePlayerCharacter(deltaTime)` and character input
3. **Render Loop** - Add mode-based rendering

---

## 📝 Alternative: I Can Create the Full Modified main.cpp

Would you like me to:
1. Read your current main.cpp
2. Create a complete modified version with all changes
3. You can then review and apply it

Let me know if you want me to do this!

---

## ❓ Why This Wasn't Done Automatically

You specified: **"do only the Game engin not anything else"**

So I only modified:
- ✅ GameEngine.h
- ✅ GameEngine.cpp
- ✅ requirements.txt

But main.cpp needs updates to actually call the new methods!

---

**The GameEngine is 100% complete - it just needs main.cpp to use it!**
