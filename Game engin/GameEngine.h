#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "EduSandbox.h"
#include <string>
#include <vector>
#include <map>

// Lightweight learning-game engine (levels, evaluation, rewards, progress)
// - Rule-based evaluation (pluggable for AI later)
// - Progress persistence (simple file)
// - Rendering helper to show level UI in the sandbox

// Physics Scenario Configuration
struct ScenarioSetup {
    struct EntitySetup {
        std::string type;      // "ball", "block", "ramp", "ground"
        Vector2 position;
        Color color;
        float mass;
        float size;
        Vector2 velocity;
        std::string label;     // Optional text label

        EntitySetup() : position(0,0), color(Color::White()), mass(1.0f), size(25.0f), velocity(0,0) {}
        EntitySetup(const std::string& t, Vector2 pos, Color c, float m, float s, Vector2 v, const std::string& l)
            : type(t), position(pos), color(c), mass(m), size(s), velocity(v), label(l) {}
    };

    std::vector<EntitySetup> entities;
    std::string instruction;   // "Watch the ball roll down"
    bool pauseOnStart;

    ScenarioSetup() : pauseOnStart(false) {}
};

// Visual Achievements
struct Achievement {
    std::string id;
    std::string title;
    std::string description;
    std::string iconType;      // "star", "trophy", "medal", "lightning"
    Color iconColor;
    int pointValue;
    bool unlocked;
    float unlockTime;          // For animation timing

    Achievement() : iconColor(Color::Yellow()), pointValue(0), unlocked(false), unlockTime(0.0f) {}
    Achievement(const std::string& i, const std::string& t, const std::string& d,
                const std::string& icon, const Color& c, int pts, bool u, float time)
        : id(i), title(t), description(d), iconType(icon), iconColor(c),
          pointValue(pts), unlocked(u), unlockTime(time) {}
};

// Animation System
struct Animation {
    enum Type { SLIDE_IN, FADE_IN, SCALE_UP, BOUNCE, PARTICLE_BURST };
    Type type;
    float startTime;
    float duration;
    Vector2 startPos;
    Vector2 endPos;
    float currentAlpha;
    float currentScale;
    bool active;

    Animation() : type(FADE_IN), startTime(0.0f), duration(1.0f),
                  startPos(0,0), endPos(0,0), currentAlpha(255),
                  currentScale(1.0f), active(false) {}
};

// Particle System (for visual effects)
struct Particle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float lifetime;
    float maxLifetime;
    float size;
    bool active;

    Particle() : position(0,0), velocity(0,0), color(Color::White()),
                lifetime(0.0f), maxLifetime(1.0f), size(3.0f), active(false) {}
};

// Player Character (for interactive learning)
struct PlayerCharacter {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float size;
    bool active;
    float speed;
    float jumpForce;
    bool onGround;

    PlayerCharacter() : position(400, 300), velocity(0,0), color(Color(100, 200, 255)),
                       size(20.0f), active(false), speed(200.0f), jumpForce(400.0f), onGround(true) {}
};

// Prompt-Based Generation Mode
enum class GameMode {
    BLANK_SCREEN,      // Initial state - waiting for prompt
    PROMPT_INPUT,      // User typing prompt
    GENERATING,        // Processing prompt (can show AI later)
    PLAYING,           // Active game with physics
    LEARNING_MODE      // Traditional learning mode (original)
};

// Prompt context for AI integration
struct PromptContext {
    std::string userPrompt;           // What user wants to learn
    std::string parsedConcept;        // Extracted physics concept
    std::vector<std::string> keywords; // Key terms found
    std::string difficulty;           // "beginner", "intermediate", "advanced"
    bool requiresInteraction;         // Does user want to control things?

    PromptContext() : difficulty("beginner"), requiresInteraction(false) {}
};

struct Level {
    int id;
    std::string title;
    std::string topic;
    int difficulty; // 1..5
    std::string challenge;           // challenge prompt / scenario
    std::string reflectionQuestion;  // free-text question for student
    std::vector<std::string> expectedKeywords; // used by rule-based evaluator
    std::vector<std::pair<std::string,std::string>> misconceptionHints; // keyword -> hint
    std::vector<std::string> reinforcementExercises; // extra practice items
    int basePoints; // reward when mastered
    bool unlocked;

    // NEW: Physics integration
    ScenarioSetup scenarioSetup;       // Auto-setup physics for this level
    bool requiresInteraction;           // Must student interact?
    std::string interactionHint;        // "Apply upward force to ball"
    std::vector<std::string> progressiveHints;  // Unlock after attempts

    Level() : id(0), difficulty(1), basePoints(100), unlocked(false),
              requiresInteraction(false) {}
};

struct EvalResult {
    float score;               // 0.0 - 1.0
    bool mastered;            // did student reach mastery for this level
    bool needsReinforcement;  // should the engine produce extra practice
    std::vector<std::string> misconceptionsFound;
    std::string feedback;     // short human message
    int pointsAwarded;

    // NEW: Enhanced feedback components
    std::vector<std::string> strengths;        // "Good use of terminology"
    std::vector<std::string> improvements;     // "Consider explaining why..."
    int starRating;                             // 1-5 stars
    Color feedbackColor;                        // Green/yellow/red
    std::string motivationalMessage;            // "Great job!"

    EvalResult() : score(0.0f), mastered(false), needsReinforcement(true),
                   pointsAwarded(0), starRating(0), feedbackColor(Color::Gray()) {}
};

struct PlayerProgress {
    int currentLevelId;
    int totalPoints;
    std::vector<std::string> badges;
    std::map<int,int> attempts;      // levelId -> attempts count
    std::map<int,float> mastery;     // levelId -> mastery score (0..1)

    PlayerProgress() : currentLevelId(1), totalPoints(0) {}
};

class GameEngine {
public:
    GameEngine();

    // load / save progress (simple persistence)
    void saveProgress(const std::string& path = "player_progress.txt");
    void loadProgress(const std::string& path = "player_progress.txt");

    // levels management
    void initializeDefaultLevels();
    const Level& getCurrentLevel() const;
    const Level* getLevelById(int id) const;
    void unlockLevel(int id);

    // evaluation (rule-based for now) — will be replaceable by AI later
    EvalResult evaluateAnswer(int levelId, const std::string& studentAnswer, const std::string& reflectionAnswer = "");
    EvalResult submitAnswer(int levelId, const std::string& studentAnswer, const std::string& reflectionAnswer = "");

    // reinforcement and adaptive flow
    std::vector<std::string> generateReinforcement(int levelId, int count = 3) const;
    void adjustDifficultyAdaptive(int levelId, const EvalResult& result);

    // reward logic
    void awardPoints(int pts);
    void awardBadge(const std::string& badge);

    // progress accessors
    const PlayerProgress& getProgress() const { return progress; }

    // rendering UI for learning overlay
    void renderUI(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont, int x, int y, int w, int h);

    // render a stage background for the current level (simulation area)
    void renderBackground(SDL_Renderer* renderer, int width, int height);

    // developer/test helper: jump to a level (unlocks if necessary)
    void setCurrentLevel(int id);

    // NEW: Update (for animations, timers)
    void update(float deltaTime);

    // NEW: Scenario System
    std::vector<std::shared_ptr<Entity>> getScenarioEntities(int levelId);
    ScenarioSetup getCurrentScenarioSetup() const;

    // NEW: Achievements
    void initializeAchievements();
    void checkAndUnlockAchievements();
    const std::vector<Achievement>& getAchievements() const { return achievements; }

    // NEW: Hints
    std::string getNextHint(int levelId);
    bool hasAvailableHint(int levelId) const;

    // NEW: Animation
    void addAnimation(Animation::Type type, Vector2 pos, float duration);

private:
    std::vector<Level> levels;
    PlayerProgress progress;

    // NEW: Achievements
    std::vector<Achievement> achievements;

    // NEW: Animation
    std::vector<Animation> activeAnimations;
    float currentTime;

    // NEW: Particle effects
    std::vector<Particle> particles;

    // NEW: Player character
    PlayerCharacter player;

    // NEW: Game mode and prompt system
    GameMode currentMode;
    PromptContext promptContext;
    std::string currentPromptInput;
    bool scenarioGenerated;

    // NEW: Hints tracking
    std::map<int, int> hintsUsed;

    // NEW: Last evaluation result (for feedback display)
    EvalResult lastResult;

    // NEW: UI Animation state
    float panelSlideProgress;      // 0.0 to 1.0 for slide-in animation
    bool showingFeedback;          // True when showing feedback animation
    float feedbackAnimTime;        // Time since feedback started showing
    float progressBarAnimTarget;   // Target value for animated progress bar
    float progressBarAnimCurrent;  // Current animated value

    // helpers
    static std::string normalizeText(const std::string& s);
    static std::vector<std::string> tokenize(const std::string& s);
    static bool containsToken(const std::vector<std::string>& tokens, const std::string& keyword);
    static std::string join(const std::vector<std::string>& parts, const std::string& sep);

    // rendering helpers
    void drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, const Color& color);
    void drawBox(SDL_Renderer* renderer, int x, int y, int w, int h, const Color& fill, const Color& border);

    // NEW: Enhanced rendering helpers
    void renderProgressBar(SDL_Renderer* r, int x, int y, int w, int h, float progress, const std::string& label, TTF_Font* font);
    void renderAnimatedProgressBar(SDL_Renderer* r, int x, int y, int w, int h, float targetProgress, const std::string& label, TTF_Font* font);
    void renderStarRating(SDL_Renderer* r, int stars, int maxStars, int x, int y, int size);
    void renderBadge(SDL_Renderer* r, const Achievement& badge, int x, int y, int size);
    void renderButton(SDL_Renderer* r, const std::string& text, int x, int y, int w, int h, const Color& color, TTF_Font* font, bool hovered = false);
    void renderFeedbackModal(SDL_Renderer* r, TTF_Font* font, TTF_Font* small, int screenW, int screenH);

    // NEW: Text helpers
    std::vector<std::string> wrapText(const std::string& text, int maxWidth, TTF_Font* font);
    void drawTextWrapped(SDL_Renderer* r, TTF_Font* font, const std::string& text, int x, int y, int maxW, const Color& color);

    // NEW: Animation system
    void updateAnimations(float deltaTime);
    void renderAnimations(SDL_Renderer* r);

    // NEW: Particle system
    void spawnParticles(Vector2 position, int count, Color color);
    void updateParticles(float deltaTime);
    void renderParticles(SDL_Renderer* r);

    // NEW: Prompt-driven system
    void setGameMode(GameMode mode);
    GameMode getGameMode() const { return currentMode; }
    void processPrompt(const std::string& prompt);
    void generateScenarioFromPrompt();
    void clearScene();

    // NEW: Player character control
    void updatePlayerCharacter(float deltaTime);
    void handlePlayerInput(bool left, bool right, bool up, bool down, bool jump);
    void renderPlayerCharacter(SDL_Renderer* r);
    const PlayerCharacter& getPlayerCharacter() const { return player; }

    // NEW: Prompt input handling
    void addCharToPrompt(char c);
    void removeCharFromPrompt();
    const std::string& getCurrentPrompt() const { return currentPromptInput; }
    void submitPrompt();

    // NEW: Blank screen rendering (initial state)
    void renderBlankScreen(SDL_Renderer* r, TTF_Font* font, TTF_Font* small, int screenW, int screenH);
    void renderPromptInputScreen(SDL_Renderer* r, TTF_Font* font, TTF_Font* small, int screenW, int screenH);

    // NEW: AI Integration architecture (for future connection)
    struct AIRequest {
        std::string prompt;
        std::string concept;
        std::string difficulty;

        AIRequest() = default;
    };

    struct AIResponse {
        std::vector<ScenarioSetup::EntitySetup> entities;
        std::string instruction;
        std::string explanation;

        AIResponse() = default;
    };

    // Placeholder for AI integration (can be connected to external AI service)
    AIResponse processWithAI(const AIRequest& request);
};

#endif // GAME_ENGINE_H
