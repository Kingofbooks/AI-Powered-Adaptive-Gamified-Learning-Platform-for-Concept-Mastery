#include "EduSandbox.h"
#include "PhysicsEngine.h"
#include "PromptParser.h"
#include "GameEngine.h"
#ifdef USE_SDL_IMAGE
#include <SDL_image.h>
#endif
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <sstream>

int SCREEN_WIDTH = 1200;
int SCREEN_HEIGHT = 700;
// Left vertical toolbar width and bottom panel height
const int LEFT_TOOLBAR_WIDTH = 120;
const int BOTTOM_PANEL_HEIGHT = 80;
const int PROMPT_HEIGHT = 40;

class Application {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* smallFont;
    SDL_Texture* bgTexture;
    std::string bgPath;
    bool backgroundEnabled;
    
    std::vector<std::shared_ptr<Entity>> entities;
    PhysicsEngine physics;
    PromptParser parser;
    GameEngine gameEngine;                  // learning/game engine
    
    bool running;
    std::string promptText;
    std::string messageText;
    float messageTimer;
    
    Entity* selectedEntity;
    Entity* draggedEntity;
    Vector2 dragOffset;
    
    bool showHelp;
    std::string helpText;

    // Learning / game-mode UI state
    bool learningModeActive = false;        // toggle with 'g'
    bool gameAnswerMode = false;            // true when prompt is being used for an answer
    
    // UI state
    SDL_Rect promptBox;
    bool promptActive;
    bool isFullScreen = true;               // starts fullscreen; toggle with 'F' key
    
public:
        Application() : window(nullptr), renderer(nullptr), font(nullptr), 
                    smallFont(nullptr), bgTexture(nullptr), backgroundEnabled(false),
                    running(false), messageTimer(0), selectedEntity(nullptr), draggedEntity(nullptr),
                    showHelp(false), promptActive(false), isFullScreen(true) {

           promptBox = {LEFT_TOOLBAR_WIDTH + 20, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT + 20, 
               SCREEN_WIDTH - LEFT_TOOLBAR_WIDTH - 60, PROMPT_HEIGHT};
    }
    
    ~Application() {
        cleanup();
    }
    
    bool initialize() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        if (TTF_Init() < 0) {
            std::cerr << "TTF Init failed: " << TTF_GetError() << std::endl;
            return false;
        }

        // SDL_image is optional; if compiled with USE_SDL_IMAGE the app will try
        // to initialize support for PNG/JPEG. Otherwise BMP fallback will be used.
    #ifdef USE_SDL_IMAGE
        int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            std::cerr << "Warning: IMG_Init failed: " << IMG_GetError() << std::endl;
            // not fatal; we'll still allow BMP or no background
        }
    #endif
        
        // Start in fullscreen (desktop) so the sandbox fills the screen; user can toggle with F
        window = SDL_CreateWindow("Educational Physics Sandbox",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
        
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }

        // Update runtime width/height to match actual window size so UI scales correctly
        SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
        promptBox = {LEFT_TOOLBAR_WIDTH + 20, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT + 20, SCREEN_WIDTH - LEFT_TOOLBAR_WIDTH - 60, PROMPT_HEIGHT};
        
        renderer = SDL_CreateRenderer(window, -1, 
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        
        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20);
        smallFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
        
        if (!font || !smallFont) {
            // Try alternative font paths
            font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 20);
            smallFont = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 14);
        }
        
        if (!font || !smallFont) {
            std::cerr << "Font loading failed. Continuing without text." << std::endl;
        }
        
        // Setup initial scene (place ground above bottom panel)
        auto ground = std::make_shared<Ground>(Vector2(SCREEN_WIDTH/2, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT - 30));
        entities.push_back(ground);
        
        messageText = "Welcome! Type a command or drag objects from the toolbar.";
        messageTimer = 5.0f;
        
        return true;
    }
    
    void cleanup() {
        // persist learning progress
        gameEngine.saveProgress();

        if (bgTexture) {
            SDL_DestroyTexture(bgTexture);
            bgTexture = nullptr;
        }

        if (font) TTF_CloseFont(font);
        if (smallFont) TTF_CloseFont(smallFont);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
    #ifdef USE_SDL_IMAGE
        IMG_Quit();
    #endif
        TTF_Quit();
        SDL_Quit();
    }
    
    void handleEvent(const SDL_Event& event) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_DROPFILE) {
            // Accept dropped image file as background
            char* dropped_filedir = event.drop.file;
            if (dropped_filedir) {
                std::string path(dropped_filedir);
                loadBackgroundFromFile(path);
                SDL_free(dropped_filedir);
            }
        }
        else if (event.type == SDL_KEYDOWN) {
            handleKeyPress(event.key.keysym.sym);
        }
        else if (event.type == SDL_TEXTINPUT && promptActive) {
            promptText += event.text.text;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) {
            handleMouseDown(event.button.x, event.button.y, event.button.button);
        }
        else if (event.type == SDL_MOUSEBUTTONUP) {
            handleMouseUp(event.button.x, event.button.y);
        }
        else if (event.type == SDL_MOUSEMOTION) {
            handleMouseMove(event.motion.x, event.motion.y);
        }
    }
    
    void handleKeyPress(SDL_Keycode key) {
        if (promptActive) {
            if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                if (gameAnswerMode && learningModeActive) {
                    // submit learning/game answer
                    auto levelId = gameEngine.getCurrentLevel().id;
                    EvalResult res = gameEngine.submitAnswer(levelId, promptText, "");
                    showMessage(res.feedback + " (+" + std::to_string(res.pointsAwarded) + " pts)");
                    // end game-answer input
                    gameAnswerMode = false;
                    promptActive = false;
                    promptText.clear();
                    SDL_StopTextInput();
                } else {
                    executePrompt();
                    promptActive = false;
                    SDL_StopTextInput();
                }
            }
            else if (key == SDLK_BACKSPACE && !promptText.empty()) {
                promptText.pop_back();
            }
            else if (key == SDLK_ESCAPE) {
                promptActive = false;
                promptText.clear();
                gameAnswerMode = false;
                SDL_StopTextInput();
            }
        }
        else {
            switch (key) {
                case SDLK_SPACE:
                case SDLK_p:
                    physics.togglePause();
                    showMessage(physics.isPaused() ? "PAUSED" : "RESUMED");
                    break;

                case SDLK_f: // toggle fullscreen/windowed
                    isFullScreen = !isFullScreen;
                    if (isFullScreen) {
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    } else {
                        SDL_SetWindowFullscreen(window, 0);
                        SDL_SetWindowSize(window, 1200, 700);
                        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                    }
                    SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
                    promptBox = {LEFT_TOOLBAR_WIDTH + 20, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT + 20, 
                                 SCREEN_WIDTH - LEFT_TOOLBAR_WIDTH - 60, PROMPT_HEIGHT};
                    showMessage(isFullScreen ? "Fullscreen ON" : "Windowed mode");
                    break;

                case SDLK_g: // toggle learning / game mode
                    learningModeActive = !learningModeActive;
                    showMessage(learningModeActive ? "Learning Mode ON (press Enter to answer)" : "Learning Mode OFF");
                    break;
                    
                case SDLK_r: {
                    entities.clear();
                    auto ground = std::make_shared<Ground>(
                        Vector2(SCREEN_WIDTH/2, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT - 30));
                    entities.push_back(ground);
                    showMessage("Scene cleared");
                    break;
                }
                    
                case SDLK_EQUALS:
                case SDLK_PLUS:
                    physics.setTimeScale(physics.getTimeScale() + 0.1f);
                    showMessage("Speed: " + std::to_string((int)(physics.getTimeScale() * 100)) + "%");
                    break;
                    
                case SDLK_MINUS:
                    physics.setTimeScale(physics.getTimeScale() - 0.1f);
                    showMessage("Speed: " + std::to_string((int)(physics.getTimeScale() * 100)) + "%");
                    break;

                // Object operation controls (operate selected object)
                case SDLK_w:
                case SDLK_UP:
                    if (selectedEntity && selectedEntity->getPhysics().type == PhysicsType::DYNAMIC) {
                        selectedEntity->applyForce(Vector2(0, -400.0f * selectedEntity->getPhysics().mass));
                        showMessage("Applied upward force");
                    }
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    if (selectedEntity && selectedEntity->getPhysics().type == PhysicsType::DYNAMIC) {
                        selectedEntity->applyForce(Vector2(0, 400.0f * selectedEntity->getPhysics().mass));
                        showMessage("Applied downward force");
                    }
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    if (selectedEntity && selectedEntity->getPhysics().type == PhysicsType::DYNAMIC) {
                        selectedEntity->applyForce(Vector2(-400.0f * selectedEntity->getPhysics().mass, 0));
                        showMessage("Applied leftward force");
                    }
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    if (selectedEntity && selectedEntity->getPhysics().type == PhysicsType::DYNAMIC) {
                        selectedEntity->applyForce(Vector2(400.0f * selectedEntity->getPhysics().mass, 0));
                        showMessage("Applied rightward force");
                    }
                    break;

                case SDLK_i: // increase mass
                    if (selectedEntity) {
                        float m = selectedEntity->getPhysics().mass + 0.1f;
                        selectedEntity->setMass(m);
                        showMessage("Mass: " + std::to_string(m));
                    }
                    break;
                case SDLK_k: // decrease mass
                    if (selectedEntity) {
                        float m = std::max(0.1f, selectedEntity->getPhysics().mass - 0.1f);
                        selectedEntity->setMass(m);
                        showMessage("Mass: " + std::to_string(m));
                    }
                    break;

                case SDLK_o: // toggle gravity for selected entity
                    if (selectedEntity) {
                        bool newG = !selectedEntity->getPhysics().affectedByGravity;
                        selectedEntity->setAffectedByGravity(newG);
                        showMessage(std::string("Gravity ") + (newG ? "ON" : "OFF"));
                    }
                    break;

                case SDLK_u: // small upward impulse
                    if (selectedEntity && selectedEntity->getPhysics().type == PhysicsType::DYNAMIC) {
                        Vector2 vel = selectedEntity->getVelocity();
                        vel.y -= 300.0f;
                        selectedEntity->setVelocity(vel);
                        showMessage("Impulse applied");
                    }
                    break;

                case SDLK_h:
                    showHelp = !showHelp;
                    helpText = parser.getHelpText();
                    break;

                case SDLK_b: // toggle custom background visibility
                    backgroundEnabled = !backgroundEnabled;
                    showMessage(backgroundEnabled ? "Background ON" : "Background OFF");
                    break;

                // Quick stage selection for testing backgrounds
                case SDLK_1: case SDLK_2: case SDLK_3: case SDLK_4: case SDLK_5: {
                    int level = 0;
                    if (key == SDLK_1) level = 1;
                    else if (key == SDLK_2) level = 2;
                    else if (key == SDLK_3) level = 3;
                    else if (key == SDLK_4) level = 4;
                    else if (key == SDLK_5) level = 5;
                    gameEngine.setCurrentLevel(level);
                    showMessage(std::string("Stage ") + std::to_string(level) + " selected");
                    break;
                }
                    
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    // start typing either a normal command or a learning answer
                    promptActive = true;
                    promptText.clear();
                    if (learningModeActive) {
                        gameAnswerMode = true; // Entered to answer the current learning challenge
                    }
                    SDL_StartTextInput();
                    break;
            }
        }
    }

    void loadBackgroundFromFile(const std::string& path) {
        if (!renderer) return;

        // Free previous texture
        if (bgTexture) {
            SDL_DestroyTexture(bgTexture);
            bgTexture = nullptr;
            bgPath.clear();
        }

        SDL_Surface* surf = nullptr;
#ifdef USE_SDL_IMAGE
        surf = IMG_Load(path.c_str());
        if (!surf) {
            // Fall through to BMP attempt
            surf = nullptr;
        }
#endif
        if (!surf) {
            // Try BMP as fallback (built-in support)
            surf = SDL_LoadBMP(path.c_str());
        }

        if (!surf) {
            showMessage(std::string("Failed to load image: ") + path);
            return;
        }

        bgTexture = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);

        if (!bgTexture) {
            showMessage(std::string("Failed to create texture from: ") + path);
            return;
        }

        bgPath = path;
        showMessage(std::string("Background loaded: ") + path, 4.0f);
    }
    
    void handleMouseDown(int x, int y, Uint8 button) {
        if (button != SDL_BUTTON_LEFT) return;
        
        Vector2 mousePos(x, y);
        
        // Check left toolbar buttons (vertical dock)
        if (x <= LEFT_TOOLBAR_WIDTH) {
            handleLeftToolbarClick(x, y);
            return;
        }
        
        // Check prompt box
        if (x >= promptBox.x && x <= promptBox.x + promptBox.w &&
            y >= promptBox.y && y <= promptBox.y + promptBox.h) {
            promptActive = true;
            SDL_StartTextInput();
            return;
        }
        
        // Check entities
        for (auto it = entities.rbegin(); it != entities.rend(); ++it) {
            if ((*it)->getBoundingBox().contains(mousePos)) {
                if (selectedEntity) selectedEntity->setSelected(false);
                selectedEntity = it->get();
                selectedEntity->setSelected(true);
                
                if ((*it)->getPhysics().type == PhysicsType::DYNAMIC ||
                    (*it)->getPhysics().type == PhysicsType::KINEMATIC) {
                    draggedEntity = it->get();
                    draggedEntity->setDragging(true);
                    dragOffset = draggedEntity->getPosition() - mousePos;
                }
                return;
            }
        }
        
        // Deselect
        if (selectedEntity) {
            selectedEntity->setSelected(false);
            selectedEntity = nullptr;
        }
    }
    
    void handleMouseUp(int x, int y) {
        (void)x; (void)y;
        if (draggedEntity) {
            draggedEntity->setDragging(false);
            draggedEntity = nullptr;
        }
    }
    
    void handleMouseMove(int x, int y) {
        if (draggedEntity) {
            Vector2 newPos(x, y);
            draggedEntity->setPosition(newPos + dragOffset);
        }
    }
    
    void handleLeftToolbarClick(int x, int y) {
        int btnWidth = 80;
        int btnX = 20;
        int btnH = 36;
        int spacing = 10;

        struct Button {
            int idx;
            std::string type;
        };

        std::vector<Button> buttons = {
            {0, "min"},
            {1, "max"},
            {2, "ball"},
            {3, "block"},
            {4, "ground"},
            {5, "ramp"},
            {6, "bg"}
        };

        for (const auto& btn : buttons) {
            int btnY = 20 + btn.idx * (btnH + spacing);
            if (x >= btnX && x <= btnX + btnWidth &&
                y >= btnY && y <= btnY + btnH) {
                if (btn.type == "bg") {
                    if (bgTexture) {
                        backgroundEnabled = !backgroundEnabled;
                        showMessage(backgroundEnabled ? "Background ON" : "Background OFF");
                    } else {
                        showMessage("Drag an image (PNG/JPG/BMP) into the window to load a background");
                    }
                } else if (btn.type == "min") {
                    SDL_MinimizeWindow(window);
                    showMessage("Window minimized");
                } else if (btn.type == "max") {
                    // If fullscreen, exit fullscreen first so maximize/restore works predictably
                    if (isFullScreen) {
                        SDL_SetWindowFullscreen(window, 0);
                        isFullScreen = false;
                    }
                    Uint32 flags = SDL_GetWindowFlags(window);
                    if (flags & SDL_WINDOW_MAXIMIZED) {
                        SDL_RestoreWindow(window);
                        showMessage("Window restored");
                    } else {
                        SDL_MaximizeWindow(window);
                        showMessage("Window maximized");
                    }
                } else {
                    createEntity(btn.type);
                }
                return;
            }
        }
    }
    
    void createEntity(const std::string& type) {
        std::shared_ptr<Entity> entity;
        Vector2 pos(SCREEN_WIDTH / 2, 200);
        
        if (type == "ball") {
            entity = std::make_shared<Ball>(pos);
        } else if (type == "block") {
            entity = std::make_shared<Block>(pos);
        } else if (type == "ground") {
            entity = std::make_shared<Ground>(pos);
        } else if (type == "ramp") {
            entity = std::make_shared<Ramp>(pos);
        }
        
        if (entity) {
            entities.push_back(entity);
            showMessage("Created " + type);
        }
    }
    
    void executePrompt() {
        if (promptText.empty()) return;
        
        ParsedCommand cmd = parser.parse(promptText, 
            SCREEN_WIDTH, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT);
        
        if (cmd.type == ParsedCommand::CREATE_ENTITY) {
            auto entity = parser.createEntityFromCommand(cmd);
            if (entity) {
                entities.push_back(entity);
                showMessage("Created: " + entity->getDescription());
            }
        }
        else if (cmd.type == ParsedCommand::EXPLAIN) {
            if (cmd.explanationTopic == "gravity") {
                showMessage(physics.explainGravity(), 10.0f);
            }
            else if (cmd.explanationTopic == "collision") {
                auto& collisions = physics.getRecentCollisions();
                if (!collisions.empty()) {
                    showMessage(physics.explainCollision(collisions.back()), 10.0f);
                } else {
                    showMessage("No recent collisions");
                }
            }
            else if (cmd.explanationTopic == "velocity" && selectedEntity) {
                showMessage(physics.explainVelocity(selectedEntity), 10.0f);
            }
            else if (cmd.explanationTopic == "energy" && selectedEntity) {
                showMessage(physics.explainEnergy(selectedEntity), 10.0f);
            }
            else {
                showHelp = true;
                helpText = parser.getHelpText();
            }
        }
        else if (cmd.type == ParsedCommand::CLEAR) {
            entities.clear();
            auto ground = std::make_shared<Ground>(
                Vector2(SCREEN_WIDTH/2, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT - 30));
            entities.push_back(ground);
            showMessage("Cleared all entities");
        }
        else {
            showMessage("Unknown command. Press H for help.");
        }
        
        promptText.clear();
    }
    
    void showMessage(const std::string& msg, float duration = 3.0f) {
        messageText = msg;
        messageTimer = duration;
    }
    
    void update(float deltaTime) {
        physics.update(entities, deltaTime);
        
        if (messageTimer > 0) {
            messageTimer -= deltaTime;
        }
    }
    
    void render() {
        // Background
        Color bg = Color::Background();
        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
        SDL_RenderClear(renderer);
        
        // Simulation area background: if a custom background image is loaded and enabled,
        // render it stretched to the simulation area; otherwise fall back to GameEngine background.
        if (bgTexture && backgroundEnabled) {
            SDL_Rect dest = {LEFT_TOOLBAR_WIDTH, 0, SCREEN_WIDTH - LEFT_TOOLBAR_WIDTH, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT};
            SDL_RenderCopy(renderer, bgTexture, nullptr, &dest);
        } else {
            gameEngine.renderBackground(renderer, SCREEN_WIDTH, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT);

            // Grid (drawn on top of background)
            SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
            for (int x = LEFT_TOOLBAR_WIDTH; x < SCREEN_WIDTH; x += 50) {
                SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT);
            }
            for (int y = 0; y < SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT; y += 50) {
                SDL_RenderDrawLine(renderer, LEFT_TOOLBAR_WIDTH, y, SCREEN_WIDTH, y);
            }
        }
        
        // Entities
        for (auto& entity : entities) {
            entity->render(renderer);
            
            // Render text labels
            if (entity->getType() == EntityType::TEXT_LABEL && font) {
                TextLabel* label = static_cast<TextLabel*>(entity.get());
                renderText(label->getText(), entity->getPosition(), entity->getColor());
            }
        }
        
        // Velocity vectors for selected entity
        if (selectedEntity && selectedEntity->getPhysics().type == PhysicsType::DYNAMIC) {
            Vector2 pos = selectedEntity->getPosition();
            Vector2 vel = selectedEntity->getVelocity();
            float scale = 0.1f;
            Vector2 end = pos + (vel * scale);
            
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            for (int i = 0; i < 3; i++) {
                SDL_RenderDrawLine(renderer, 
                    (int)pos.x + i, (int)pos.y,
                    (int)end.x + i, (int)end.y);
            }
        }
        
        // Tool panels (left toolbar + bottom panel)
        renderToolbar();

        // Learning / Game Engine overlay
        if (learningModeActive && font && smallFont) {
            int panelW = 380;
            int panelH = 320;
            int panelX = SCREEN_WIDTH - panelW - 20;
            int panelY = 20;
            gameEngine.renderUI(renderer, font, smallFont, panelX, panelY, panelW, panelH);
        }
        
        // Message box
        if (messageTimer > 0 && font) {
            SDL_Rect msgBox = {20, 20, 400, 80};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 230);
            SDL_RenderFillRect(renderer, &msgBox);
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_RenderDrawRect(renderer, &msgBox);
            
            renderTextBox(messageText, Vector2(30, 30), Color::Black(), 380);
        }
        
        // Help overlay
        if (showHelp && font) {
            SDL_Rect helpBox = {SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2 - 200, 400, 400};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 250);
            SDL_RenderFillRect(renderer, &helpBox);
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            for (int i = 0; i < 3; i++) {
                SDL_Rect border = {helpBox.x - i, helpBox.y - i, 
                                  helpBox.w + i*2, helpBox.h + i*2};
                SDL_RenderDrawRect(renderer, &border);
            }
            
            renderTextBox(helpText, Vector2(helpBox.x + 20, helpBox.y + 20), 
                         Color::Black(), 360);
            
            renderText("Press H to close", 
                      Vector2(helpBox.x + helpBox.w/2 - 70, helpBox.y + helpBox.h - 30),
                      Color::Gray());
        }
        
        SDL_RenderPresent(renderer);
    }
    
    void renderToolbar() {
        // Left vertical toolbar
        SDL_Rect leftToolbar = {0, 0, LEFT_TOOLBAR_WIDTH, SCREEN_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 40, 44, 52, 255);
        SDL_RenderFillRect(renderer, &leftToolbar);

        // Buttons stacked vertically
        int btnW = 80;
        int btnH = 36;
        int btnX = 20;
        int spacing = 10;

        // Window control buttons
        renderButton(btnX, 20 + 0 * (btnH + spacing), btnW, btnH, "Min", Color::LightGray());
        renderButton(btnX, 20 + 1 * (btnH + spacing), btnW, btnH, "Max", Color::LightGray());
        // Entity buttons
        renderButton(btnX, 20 + 2 * (btnH + spacing), btnW, btnH, "Ball", Color::Red());
        renderButton(btnX, 20 + 3 * (btnH + spacing), btnW, btnH, "Block", Color::Blue());
        renderButton(btnX, 20 + 4 * (btnH + spacing), btnW, btnH, "Ground", Color::Brown());
        renderButton(btnX, 20 + 5 * (btnH + spacing), btnW, btnH, "Ramp", Color::Gray());
        renderButton(btnX, 20 + 6 * (btnH + spacing), btnW, btnH, "BG", Color::White());

        // Bottom panel (prompt + status)
        SDL_Rect bottomPanel = {LEFT_TOOLBAR_WIDTH, SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT, 
                                SCREEN_WIDTH - LEFT_TOOLBAR_WIDTH, BOTTOM_PANEL_HEIGHT};
        SDL_SetRenderDrawColor(renderer, 50, 50, 60, 230);
        SDL_RenderFillRect(renderer, &bottomPanel);

        // Prompt box inside bottom panel
        SDL_SetRenderDrawColor(renderer, promptActive ? 255 : 220, 
                              promptActive ? 255 : 220,
                              promptActive ? 255 : 220, 255);
        SDL_RenderFillRect(renderer, &promptBox);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderDrawRect(renderer, &promptBox);

        if (font) {
            std::string displayText = promptActive ? (promptText + "|") : "Type command (Enter)";
            renderText(displayText, Vector2(promptBox.x + 5, promptBox.y + 8), 
                      promptActive ? Color::Black() : Color::Gray());
        }

        // Status text to the right of prompt
        if (font) {
            std::string status = physics.isPaused() ? "PAUSED" : "RUNNING";
            status += " | Speed: " + std::to_string((int)(physics.getTimeScale() * 100)) + "%";
            status += " | Objects: " + std::to_string(entities.size());
            if (learningModeActive) status += " | LEARNING";

            int statusX = promptBox.x + promptBox.w + 20;
            int statusY = SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT + 12;
            renderText(status, Vector2(statusX, statusY), Color::White());

            renderText("H=Help  P=Pause  R=Reset  F=Fullscreen  WASD=force  I/K=mass  O=gravity  +/-=Speed",
                      Vector2(statusX, statusY + 22),
                      Color::LightGray());
        }
    }
    
    void renderButton(int x, int y, int w, int h, const std::string& text, const Color& color) {
        SDL_Rect btn = {x, y, w, h};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 200);
        SDL_RenderFillRect(renderer, &btn);
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &btn);
        
        if (font) {
            renderText(text, Vector2(x + w/2 - text.length()*4, y + h/2 - 7), Color::White());
        }
    }
    
    void renderText(const std::string& text, const Vector2& pos, const Color& color) {
        if (!font || text.empty()) return;
        
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color.toSDL());
        if (!surface) return;
        
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect dest = {(int)pos.x, (int)pos.y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &dest);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
    
    void renderTextBox(const std::string& text, const Vector2& pos, 
                       const Color& color, int maxWidth) {
        (void)maxWidth;
        if (!smallFont || text.empty()) return;
        
        std::istringstream iss(text);
        std::string line;
        int yOffset = 0;
        
        while (std::getline(iss, line)) {
            if (!line.empty()) {
                SDL_Surface* surface = TTF_RenderText_Blended(smallFont, line.c_str(), color.toSDL());
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                    if (texture) {
                        SDL_Rect dest = {(int)pos.x, (int)pos.y + yOffset, surface->w, surface->h};
                        SDL_RenderCopy(renderer, texture, nullptr, &dest);
                        SDL_DestroyTexture(texture);
                    }
                    yOffset += surface->h + 2;
                    SDL_FreeSurface(surface);
                }
            }
        }
    }
    
    void run() {
        running = true;
        Uint32 lastTime = SDL_GetTicks();
        
        while (running) {
            Uint32 currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;
            
            deltaTime = std::min(deltaTime, 0.05f);
            
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                handleEvent(event);
            }
            
            update(deltaTime);
            render();
        }
    }
};

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    Application app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize application" << std::endl;
        return 1;
    }
    
    std::cout << "Educational Physics Sandbox - Graphical Version" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Click and drag objects to move them" << std::endl;
    std::cout << "  Click buttons to create objects" << std::endl;
    std::cout << "  Type commands and press Enter" << std::endl;
    std::cout << "  Press H for help overlay" << std::endl;
    
    app.run();
    
    return 0;
}
