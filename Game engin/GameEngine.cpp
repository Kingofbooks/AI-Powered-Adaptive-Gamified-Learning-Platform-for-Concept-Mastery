#include "GameEngine.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// --- Utility helpers ----
static std::string toLower(const std::string& s) {
    std::string out; out.reserve(s.size());
    for (char c : s) out.push_back((char)std::tolower((unsigned char)c));
    return out;
}

static std::string stripPunct(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (std::isalnum((unsigned char)c) || std::isspace((unsigned char)c)) out.push_back(c);
        else out.push_back(' ');
    }
    return out;
}

// ----- GameEngine implementation -----
GameEngine::GameEngine()
    : currentTime(0.0f)
    , panelSlideProgress(0.0f)
    , showingFeedback(false)
    , feedbackAnimTime(0.0f)
    , progressBarAnimTarget(0.0f)
    , progressBarAnimCurrent(0.0f)
    , currentMode(GameMode::BLANK_SCREEN)  // Start with blank screen!
    , scenarioGenerated(false)
{
    initializeDefaultLevels();
    initializeAchievements();
    loadProgress();

    // Initialize player character (inactive until prompt submitted)
    player.active = false;
}

void GameEngine::initializeDefaultLevels() {
    levels.clear();

    Level L1;
    L1.id = 1;
    L1.title = "Gravity basics";
    L1.topic = "Kinematics";
    L1.difficulty = 1;
    L1.challenge = "A red ball is released from rest at the top of a ramp — describe what happens and why.";
    L1.reflectionQuestion = "In your own words, why does the ball accelerate when released?";
    L1.expectedKeywords = {"gravity","acceleration","force","down","weight"};
    L1.misconceptionHints = {{"heavier","Mass does not change the acceleration due to gravity (neglecting air resistance)."},
                              {"massive fall faster","All objects accelerate at the same rate in free-fall (neglecting drag)."}};
    L1.reinforcementExercises = {"Describe how gravity acts on a dropped ball.", "If you double the mass, what changes?"};
    L1.basePoints = 80;
    L1.unlocked = true;

    // NEW: Scenario Setup for Level 1
    L1.scenarioSetup.entities = {
        ScenarioSetup::EntitySetup("ball", Vector2(200, 100), Color::Red(), 1.0f, 25.0f, Vector2(0,0), "Ball"),
        ScenarioSetup::EntitySetup("ramp", Vector2(400, 350), Color::Gray(), 0.0f, 200.0f, Vector2(0,0), ""),
        ScenarioSetup::EntitySetup("ground", Vector2(600, 550), Color::Brown(), 0.0f, 1000.0f, Vector2(0,0), "")
    };
    L1.scenarioSetup.instruction = "Watch the ball roll down the ramp. Observe how gravity accelerates it.";
    L1.scenarioSetup.pauseOnStart = false;
    L1.requiresInteraction = false;
    L1.interactionHint = "Just observe the motion";
    L1.progressiveHints = {
        "Think about the forces acting on the ball",
        "Gravity pulls the ball downward at a constant rate",
        "The ball gains speed as it rolls because of gravitational acceleration"
    };

    Level L2 = L1;
    L2.id = 2;
    L2.title = "Collision and energy";
    L2.difficulty = 2;
    L2.challenge = "Two balls collide on a frictionless surface — explain how momentum and energy behave.";
    L2.reflectionQuestion = "How does total momentum compare before and after the collision?";
    L2.expectedKeywords = {"momentum","conservation","elastic","inelastic","velocity"};
    L2.misconceptionHints = {{"energy destroyed","Energy is not destroyed — it converts forms (kinetic → thermal etc)."}};
    L2.reinforcementExercises = {"Give an example of an elastic collision.", "What changes in an inelastic collision?"};
    L2.basePoints = 100;
    L2.unlocked = false;

    // NEW: Scenario Setup for Level 2
    L2.scenarioSetup.entities = {
        ScenarioSetup::EntitySetup("ball", Vector2(200, 300), Color::Red(), 1.0f, 25.0f, Vector2(100,0), "A"),
        ScenarioSetup::EntitySetup("ball", Vector2(700, 300), Color::Blue(), 1.2f, 30.0f, Vector2(-80,0), "B"),
        ScenarioSetup::EntitySetup("ground", Vector2(600, 550), Color::Brown(), 0.0f, 1000.0f, Vector2(0,0), "")
    };
    L2.scenarioSetup.instruction = "Watch the two balls collide. Observe momentum transfer.";
    L2.scenarioSetup.pauseOnStart = false;
    L2.requiresInteraction = false;
    L2.interactionHint = "Observe the collision";
    L2.progressiveHints = {
        "Think about conservation laws - what stays constant?",
        "Total momentum before = total momentum after",
        "In elastic collisions, kinetic energy is also conserved"
    };

    Level L3 = L1;
    L3.id = 3;
    L3.title = "Work and energy";
    L3.difficulty = 3;
    L3.challenge = "A block starts at ground level. When lifted to a higher position, it gains potential energy. Explain the work-energy relationship.";
    L3.reflectionQuestion = "What form of energy increases and why?";
    L3.expectedKeywords = {"work","potential","energy","force","gravity"};
    L3.misconceptionHints = {{"force times time","Work = force × distance, not time."}};
    L3.reinforcementExercises = {"Calculate work done by lifting a mass m by height h.", "How does power relate to work?"};
    L3.basePoints = 140;
    L3.unlocked = false;

    // NEW: Scenario Setup for Level 3
    L3.scenarioSetup.entities = {
        ScenarioSetup::EntitySetup("block", Vector2(300, 500), Color::Blue(), 2.0f, 40.0f, Vector2(0,0), "Block"),
        ScenarioSetup::EntitySetup("ground", Vector2(600, 550), Color::Brown(), 0.0f, 1000.0f, Vector2(0,0), ""),
        ScenarioSetup::EntitySetup("ground", Vector2(800, 300), Color(139,90,60), 0.0f, 150.0f, Vector2(0,0), "Shelf")
    };
    L3.scenarioSetup.instruction = "Use WASD keys to apply force and lift the block to the shelf.";
    L3.scenarioSetup.pauseOnStart = true;
    L3.requiresInteraction = true;
    L3.interactionHint = "Select the block and press W (up) multiple times to lift it";
    L3.progressiveHints = {
        "Work is the transfer of energy by a force acting over a distance",
        "As the block rises, it gains gravitational potential energy",
        "The work you do equals the change in potential energy (mgh)"
    };

    Level L4 = L1;
    L4.id = 4;
    L4.title = "Friction and motion";
    L4.difficulty = 4;
    L4.challenge = "A block slides down a ramp with friction. Describe the equilibrium of forces and how friction affects motion.";
    L4.reflectionQuestion = "When does an object reach terminal speed or constant velocity?";
    L4.expectedKeywords = {"friction","normal","force","equilibrium","terminal"};
    L4.misconceptionHints = {{"friction always stops","Friction may balance other forces leading to constant velocity."}};
    L4.reinforcementExercises = {"Explain static vs kinetic friction.", "What increases frictional force?"};
    L4.basePoints = 180;
    L4.unlocked = false;

    // NEW: Scenario Setup for Level 4
    L4.scenarioSetup.entities = {
        ScenarioSetup::EntitySetup("block", Vector2(250, 200), Color::Blue(), 1.5f, 35.0f, Vector2(0,0), "Block"),
        ScenarioSetup::EntitySetup("ramp", Vector2(400, 350), Color::Gray(), 0.0f, 250.0f, Vector2(0,0), ""),
        ScenarioSetup::EntitySetup("ground", Vector2(600, 550), Color::Brown(), 0.0f, 1000.0f, Vector2(0,0), "")
    };
    L4.scenarioSetup.instruction = "Watch the block slide down the ramp. Friction slows it down.";
    L4.scenarioSetup.pauseOnStart = false;
    L4.requiresInteraction = false;
    L4.interactionHint = "Observe how friction opposes motion";
    L4.progressiveHints = {
        "Friction acts opposite to the direction of motion",
        "The normal force depends on the surface and the object's weight",
        "Terminal velocity occurs when friction balances the driving force"
    };

    Level L5 = L1;
    L5.id = 5;
    L5.title = "Energy conservation (mastery)";
    L5.difficulty = 5;
    L5.challenge = "A ball rolls down a ramp, converting potential energy to kinetic energy. In an ideal system with no friction, mechanical energy is conserved. Explain this energy transformation.";
    L5.reflectionQuestion = "How would you test that energy is conserved in this setup?";
    L5.expectedKeywords = {"conservation","energy","kinetic","potential","negligible"};
    L5.misconceptionHints = {{"energy disappears","Look for energy transfer to other forms (heat, sound)."}};
    L5.reinforcementExercises = {"Suggest measurements to verify conservation.", "What sources could cause energy loss?"};
    L5.basePoints = 240;
    L5.unlocked = false;

    // NEW: Scenario Setup for Level 5
    L5.scenarioSetup.entities = {
        ScenarioSetup::EntitySetup("ball", Vector2(150, 100), Color::Red(), 1.0f, 25.0f, Vector2(0,0), "Ball"),
        ScenarioSetup::EntitySetup("ramp", Vector2(400, 300), Color::Gray(), 0.0f, 300.0f, Vector2(0,0), ""),
        ScenarioSetup::EntitySetup("ramp", Vector2(750, 300), Color::Gray(), 0.0f, 300.0f, Vector2(0,0), ""),
        ScenarioSetup::EntitySetup("ground", Vector2(600, 550), Color::Brown(), 0.0f, 1000.0f, Vector2(0,0), "")
    };
    L5.scenarioSetup.instruction = "Watch energy transform: potential → kinetic → potential as ball rolls.";
    L5.scenarioSetup.pauseOnStart = false;
    L5.requiresInteraction = false;
    L5.interactionHint = "Observe the energy transformation cycle";
    L5.progressiveHints = {
        "At the top of the ramp, the ball has maximum potential energy",
        "As it descends, potential energy converts to kinetic energy (speed)",
        "In an ideal system, total mechanical energy (PE + KE) remains constant"
    };

    levels = {L1,L2,L3,L4,L5};
}

const Level& GameEngine::getCurrentLevel() const {
    int id = progress.currentLevelId;
    for (const auto& l : levels) if (l.id == id) return l;
    return levels.front();
}

const Level* GameEngine::getLevelById(int id) const {
    for (const auto& l : levels) if (l.id == id) return &l;
    return nullptr;
}

void GameEngine::unlockLevel(int id) {
    for (auto& l : levels) if (l.id == id) l.unlocked = true;
}

// Enhanced rule-based evaluator with detailed feedback
EvalResult GameEngine::evaluateAnswer(int levelId, const std::string& studentAnswer, const std::string& reflectionAnswer) {
    EvalResult out;
    out.score = 0.0f;
    out.mastered = false;
    out.needsReinforcement = true;
    out.pointsAwarded = 0;

    const Level* level = getLevelById(levelId);
    if (!level) {
        out.feedback = "Level not found";
        return out;
    }

    std::string norm = stripPunct(toLower(studentAnswer));
    auto tokens = tokenize(norm);

    // Keyword matching with detailed tracking
    int needed = std::max(1, (int)level->expectedKeywords.size());
    int found = 0;
    for (auto& kw : level->expectedKeywords) {
        if (containsToken(tokens, toLower(kw))) {
            found++;
            out.strengths.push_back("Mentioned key concept: " + kw);
        } else {
            out.improvements.push_back("Consider discussing: " + kw);
        }
    }

    float keywordRatio = (float)found / (float)needed;

    // Detect misconceptions with hints
    for (auto& m : level->misconceptionHints) {
        if (containsToken(tokens, toLower(m.first))) {
            out.misconceptionsFound.push_back(m.second);
        }
    }

    // Scoring with bonuses
    float baseScore = keywordRatio;
    if (!out.misconceptionsFound.empty()) baseScore *= 0.5f;

    // Reflection bonus (encourages explanation)
    if (!reflectionAnswer.empty()) {
        auto reflTokens = tokenize(stripPunct(toLower(reflectionAnswer)));
        if (reflTokens.size() > 4) {
            baseScore = std::min(1.0f, baseScore + 0.15f);
            out.strengths.push_back("Provided thoughtful reflection");
        }
    }

    // Length bonus (encourages detailed answers)
    if (tokens.size() >= 15) {
        baseScore = std::min(1.0f, baseScore + 0.1f);
        out.strengths.push_back("Detailed explanation");
    }

    out.score = std::min(1.0f, baseScore);
    out.mastered = out.score >= 0.7f && out.misconceptionsFound.empty();
    out.needsReinforcement = !out.mastered;

    // Star rating (1-5 stars)
    out.starRating = std::min(5, std::max(1, (int)(out.score * 5.5f)));

    // Feedback generation with color coding
    std::ostringstream fb;
    if (out.mastered) {
        fb << "Excellent understanding! You've demonstrated mastery of the key concepts.";
        out.motivationalMessage = "Outstanding work!";
        out.feedbackColor = Color::Green();
    } else if (out.score >= 0.5f) {
        fb << "Good effort! You're showing partial understanding.";
        if (!out.misconceptionsFound.empty()) {
            fb << " Review the misconceptions noted below.";
        }
        fb << " Try incorporating the suggested improvements.";
        out.motivationalMessage = "You're on the right track!";
        out.feedbackColor = Color::Orange();
    } else {
        fb << "You need more practice with this concept.";
        fb << " Review the challenge and try the reinforcement exercises.";
        out.motivationalMessage = "Keep trying! Learning takes time.";
        out.feedbackColor = Color(200, 80, 80);
    }
    out.feedback = fb.str();

    // Points
    out.pointsAwarded = (int)std::round(level->basePoints * out.score);

    // Store for later rendering
    lastResult = out;

    return out;
}

EvalResult GameEngine::submitAnswer(int levelId, const std::string& studentAnswer, const std::string& reflectionAnswer) {
    EvalResult r = evaluateAnswer(levelId, studentAnswer, reflectionAnswer);

    // update attempts and mastery
    progress.attempts[levelId]++;
    float prevMastery = 0.0f;
    if (progress.mastery.count(levelId)) prevMastery = progress.mastery[levelId];
    // moving average for mastery
    float newMastery = std::max(prevMastery, r.score);
    progress.mastery[levelId] = newMastery;

    // award points & badges
    if (r.pointsAwarded > 0) awardPoints(r.pointsAwarded);
    if (r.mastered) {
        std::string badge = "Level " + std::to_string(levelId) + " Mastery";
        awardBadge(badge);
        // unlock next level if available
        int nextId = levelId + 1;
        if (getLevelById(nextId)) unlockLevel(nextId);
        progress.currentLevelId = std::max(progress.currentLevelId, nextId);

        // Check for achievements
        checkAndUnlockAchievements();
    }

    // adaptive difficulty hook
    adjustDifficultyAdaptive(levelId, r);

    // NEW: Trigger feedback animation
    showingFeedback = true;
    feedbackAnimTime = 0.0f;

    // NEW: Add animation for points
    addAnimation(Animation::FADE_IN, Vector2(0, 0), 1.0f);

    // persist progress automatically
    saveProgress();
    return r;
}

std::vector<std::string> GameEngine::generateReinforcement(int levelId, int count) const {
    std::vector<std::string> out;
    const Level* L = getLevelById(levelId);
    if (!L) return out;

    // return predefined practice items first
    for (auto& ex : L->reinforcementExercises) {
        out.push_back(ex);
        if ((int)out.size() >= count) return out;
    }

    // if not enough, synthesize simple variants
    for (int i = 0; (int)out.size() < count && i < 10; ++i) {
        std::ostringstream s;
        s << L->challenge << " (practice variant " << (i+1) << ")";
        out.push_back(s.str());
    }
    return out;
}

void GameEngine::adjustDifficultyAdaptive(int levelId, const EvalResult& result) {
    // simple rule: if repeatedly failing, lower 'effective' difficulty by unlocking extra practice
    int attempts = progress.attempts[levelId];
    if (result.needsReinforcement && attempts >= 3) {
        // make reinforcement appear (no change to level difficulty structure here)
        // in a fuller implementation we might insert easier sub-levels.
    }
    // if mastered quickly, consider awarding extra points (already handled via score)
}

void GameEngine::awardPoints(int pts) {
    progress.totalPoints += pts;
}

void GameEngine::awardBadge(const std::string& badge) {
    if (std::find(progress.badges.begin(), progress.badges.end(), badge) == progress.badges.end()) {
        progress.badges.push_back(badge);
    }
}

// --- persistence (very small, human-readable) ---
void GameEngine::saveProgress(const std::string& path) {
    std::ofstream ofs(path);
    if (!ofs) return;
    ofs << "currentLevelId=" << progress.currentLevelId << "\n";
    ofs << "totalPoints=" << progress.totalPoints << "\n";
    ofs << "badges=";
    for (size_t i = 0; i < progress.badges.size(); ++i) {
        if (i) ofs << ";";
        ofs << progress.badges[i];
    }
    ofs << "\n";
    ofs << "attempts=";
    bool first = true;
    for (auto &p : progress.attempts) {
        if (!first) ofs << ",";
        first = false;
        ofs << p.first << ":" << p.second;
    }
    ofs << "\n";
    ofs << "mastery=";
    first = true;
    for (auto &p : progress.mastery) {
        if (!first) ofs << ",";
        first = false;
        ofs << p.first << ":" << p.second;
    }
    ofs << "\n";
}

void GameEngine::loadProgress(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs) return; // no saved data — start fresh
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.rfind("currentLevelId=",0) == 0) progress.currentLevelId = std::stoi(line.substr(15));
        else if (line.rfind("totalPoints=",0) == 0) progress.totalPoints = std::stoi(line.substr(12));
        else if (line.rfind("badges=",0) == 0) {
            progress.badges.clear();
            std::string v = line.substr(7);
            std::istringstream ss(v);
            std::string token;
            while (std::getline(ss, token, ';')) if (!token.empty()) progress.badges.push_back(token);
        }
        else if (line.rfind("attempts=",0) == 0) {
            progress.attempts.clear();
            std::string v = line.substr(9);
            std::istringstream ss(v);
            std::string token;
            while (std::getline(ss, token, ',')) {
                if (token.empty()) continue;
                auto pos = token.find(':');
                if (pos!=std::string::npos) {
                    int id = std::stoi(token.substr(0,pos));
                    int val = std::stoi(token.substr(pos+1));
                    progress.attempts[id] = val;
                }
            }
        }
        else if (line.rfind("mastery=",0) == 0) {
            progress.mastery.clear();
            std::string v = line.substr(7);
            std::istringstream ss(v);
            std::string token;
            while (std::getline(ss, token, ',')) {
                if (token.empty()) continue;
                auto pos = token.find(':');
                if (pos!=std::string::npos) {
                    int id = std::stoi(token.substr(0,pos));
                    float val = std::stof(token.substr(pos+1));
                    progress.mastery[id] = val;
                }
            }
        }
    }
}

// --- small text helpers used by evaluator ---
std::string GameEngine::normalizeText(const std::string& s) {
    return stripPunct(toLower(s));
}

std::vector<std::string> GameEngine::tokenize(const std::string& s) {
    std::vector<std::string> out;
    std::istringstream iss(s);
    std::string w;
    while (iss >> w) out.push_back(w);
    return out;
}

bool GameEngine::containsToken(const std::vector<std::string>& tokens, const std::string& keyword) {
    std::string k = toLower(keyword);
    for (auto &t : tokens) if (t.find(k) != std::string::npos) return true;
    return false;
}

std::string GameEngine::join(const std::vector<std::string>& parts, const std::string& sep) {
    std::string out;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i) out += sep;
        out += parts[i];
    }
    return out;
}

// --- Rendering helpers (simple, uses SDL_ttf directly) ---
void GameEngine::drawText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, const Color& color) {
    if (!font || text.empty()) return;
    SDL_Surface* surf = TTF_RenderText_Blended(font, text.c_str(), color.toSDL());
    if (!surf) return;
    SDL_Texture* tx = SDL_CreateTextureFromSurface(renderer, surf);
    if (tx) {
        SDL_Rect r = {x, y, surf->w, surf->h};
        SDL_RenderCopy(renderer, tx, nullptr, &r);
        SDL_DestroyTexture(tx);
    }
    SDL_FreeSurface(surf);
}

void GameEngine::drawBox(SDL_Renderer* renderer, int x, int y, int w, int h, const Color& fill, const Color& border) {
    SDL_Rect r = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(renderer, &r);
    SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
    SDL_RenderDrawRect(renderer, &r);
}

// Small helper to draw a filled circle (local to this file)
static void DrawFilledCircleLocal(SDL_Renderer* renderer, int cx, int cy, int radius, const Color& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
            if (x*x + y*y <= radius*radius) SDL_RenderDrawPoint(renderer, cx + x, cy + y);
        }
    }
}

// Render a background for the current level (5 dramatic stage designs)
void GameEngine::renderBackground(SDL_Renderer* renderer, int width, int height) {
    const Level& L = getCurrentLevel();
    int id = L.id;

    // Clip to simulation area
    SDL_Rect area = {0, 0, width, height};
    SDL_RenderSetClipRect(renderer, &area);

    if (id == 1) {
        // ========== Stage 1: DETAILED CLASSROOM ==========
        // Gradient wall (warm beige to light yellow)
        for (int y = 0; y < height; ++y) {
            float t = (float)y / (float)height;
            Uint8 r = (Uint8)(245 - 15*t);
            Uint8 g = (Uint8)(240 - 10*t);
            Uint8 b = (Uint8)(220 - 5*t);
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawLine(renderer, 0, y, width, y);
        }

        // Floor tiles (wooden pattern)
        SDL_Rect floor = {0, height - 100, width, 100};
        SDL_SetRenderDrawColor(renderer, 160, 120, 80, 255);
        SDL_RenderFillRect(renderer, &floor);
        for (int x = 0; x < width; x += 60) {
            SDL_SetRenderDrawColor(renderer, 140, 100, 70, 255);
            SDL_RenderDrawLine(renderer, x, height - 100, x, height);
        }
        for (int y = height - 100; y < height; y += 20) {
            SDL_SetRenderDrawColor(renderer, 140, 100, 70, 255);
            SDL_RenderDrawLine(renderer, 0, y, width, y);
        }

        // Large detailed chalkboard with frame
        SDL_Rect boardFrame = {50, 40, width/2 - 60, height/3 + 20};
        SDL_SetRenderDrawColor(renderer, 80, 50, 30, 255); // wood frame
        SDL_RenderFillRect(renderer, &boardFrame);
        SDL_Rect board = {58, 48, boardFrame.w - 16, boardFrame.h - 16};
        SDL_SetRenderDrawColor(renderer, 25, 80, 35, 255); // dark green chalkboard
        SDL_RenderFillRect(renderer, &board);

        // Chalk equations and diagrams
        SDL_SetRenderDrawColor(renderer, 220, 240, 220, 180);
        // F=ma equation
        for (int i = 0; i < 5; ++i) {
            SDL_RenderDrawLine(renderer, board.x + 20 + i, board.y + 30,
                             board.x + 80 + i, board.y + 30);
        }
        // Diagram: circle (ball) and arrow (force)
        DrawFilledCircleLocal(renderer, board.x + 40, board.y + 80, 15, Color(220,240,220,150));
        for (int i = 0; i < 3; ++i) {
            SDL_RenderDrawLine(renderer, board.x + 55, board.y + 80,
                             board.x + 110, board.y + 80 + i);
        }
        // More chalk lines (notes)
        for (int i = 0; i < 6; ++i) {
            int lineY = board.y + 110 + i * 18;
            int lineW = (i % 2 == 0) ? board.w - 60 : board.w - 90;
            SDL_RenderDrawLine(renderer, board.x + 20, lineY, board.x + 20 + lineW, lineY);
        }

        // Window with outdoor view (right side)
        SDL_Rect windowFrame = {width - 250, 60, 180, 220};
        SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255); // window frame
        SDL_RenderFillRect(renderer, &windowFrame);
        SDL_Rect windowView = {windowFrame.x + 10, windowFrame.y + 10, 160, 200};
        // Sky view through window
        for (int y = 0; y < windowView.h; ++y) {
            float t = (float)y / (float)windowView.h;
            Uint8 b = (Uint8)(180 + 60*t);
            SDL_SetRenderDrawColor(renderer, 120, 160, b, 255);
            SDL_RenderDrawLine(renderer, windowView.x, windowView.y + y,
                             windowView.x + windowView.w, windowView.y + y);
        }
        // Cloud visible through window
        DrawFilledCircleLocal(renderer, windowView.x + 60, windowView.y + 40, 20, Color(255,255,255,200));
        DrawFilledCircleLocal(renderer, windowView.x + 80, windowView.y + 38, 24, Color(255,255,255,200));
        // Window cross frame
        SDL_SetRenderDrawColor(renderer, 180, 180, 190, 255);
        SDL_RenderDrawLine(renderer, windowFrame.x + 90, windowFrame.y + 10,
                          windowFrame.x + 90, windowFrame.y + windowFrame.h - 10);
        SDL_RenderDrawLine(renderer, windowFrame.x + 10, windowFrame.y + 110,
                          windowFrame.x + windowFrame.w - 10, windowFrame.y + 110);

        // Bookshelf (left side)
        SDL_Rect shelf1 = {20, height/2 - 20, 100, 15};
        SDL_SetRenderDrawColor(renderer, 120, 80, 50, 255);
        SDL_RenderFillRect(renderer, &shelf1);
        SDL_Rect shelf2 = {20, height/2 + 40, 100, 15};
        SDL_RenderFillRect(renderer, &shelf2);
        // Books on shelves (colorful rectangles)
        int bookColors[][3] = {{180,50,50}, {50,100,180}, {80,160,80}, {200,180,50}, {140,80,140}};
        for (int i = 0; i < 5; ++i) {
            SDL_Rect book = {28 + i*18, shelf1.y - 35, 15, 35};
            SDL_SetRenderDrawColor(renderer, bookColors[i][0], bookColors[i][1], bookColors[i][2], 255);
            SDL_RenderFillRect(renderer, &book);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
            SDL_RenderDrawRect(renderer, &book);
        }

        // Teacher's desk with items (bottom right)
        SDL_Rect desk = {width - 280, height - 120, 240, 25};
        SDL_SetRenderDrawColor(renderer, 120, 80, 50, 255);
        SDL_RenderFillRect(renderer, &desk);
        SDL_SetRenderDrawColor(renderer, 90, 60, 40, 255);
        SDL_RenderDrawRect(renderer, &desk);
        // Desk legs
        SDL_Rect leg1 = {desk.x + 20, desk.y + 25, 15, 75};
        SDL_Rect leg2 = {desk.x + desk.w - 35, desk.y + 25, 15, 75};
        SDL_SetRenderDrawColor(renderer, 100, 70, 45, 255);
        SDL_RenderFillRect(renderer, &leg1);
        SDL_RenderFillRect(renderer, &leg2);
        // Items on desk: books, apple, pen
        SDL_Rect book = {desk.x + 30, desk.y - 18, 50, 18};
        SDL_SetRenderDrawColor(renderer, 100, 60, 180, 255);
        SDL_RenderFillRect(renderer, &book);
        DrawFilledCircleLocal(renderer, desk.x + 120, desk.y - 8, 10, Color(200,50,50)); // apple
        SDL_Rect pen = {desk.x + 160, desk.y - 3, 40, 3};
        SDL_SetRenderDrawColor(renderer, 50, 50, 80, 255);
        SDL_RenderFillRect(renderer, &pen);

        // Educational poster on wall
        SDL_Rect poster = {width/2 + 40, height/2 - 50, 120, 160};
        SDL_SetRenderDrawColor(renderer, 255, 250, 240, 255);
        SDL_RenderFillRect(renderer, &poster);
        SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
        SDL_RenderDrawRect(renderer, &poster);
        // Poster content (physics symbols)
        DrawFilledCircleLocal(renderer, poster.x + 60, poster.y + 50, 20, Color(200,100,100,255));
        for (int i = 0; i < 3; ++i) {
            SDL_RenderDrawLine(renderer, poster.x + 80, poster.y + 50 + i,
                             poster.x + 100, poster.y + 50 + i);
        }
    }
    else if (id == 2) {
        // ========== Stage 2: BEAUTIFUL OUTDOOR SCENE ==========
        // Rich blue sky gradient (deeper at top)
        for (int y = 0; y < height; ++y) {
            float t = (float)y / (float)height;
            Uint8 r = (Uint8)(100 + 135*t);
            Uint8 g = (Uint8)(180 + 75*t);
            Uint8 b = (Uint8)(220 + 35*t);
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawLine(renderer, 0, y, width, y);
        }

        // Distant mountains (layered silhouettes)
        SDL_SetRenderDrawColor(renderer, 80, 120, 140, 180);
        for (int i = 0; i < 4; ++i) {
            int mx = i * 200;
            int my = height/2 + 80 + i*20;
            int mw = 250;
            int mh = 120 - i*15;
            SDL_Rect mountain = {mx, my - mh, mw, mh};
            SDL_RenderFillRect(renderer, &mountain);
            // Mountain peak (triangle approximation)
            for (int py = 0; py < mh/2; ++py) {
                int px1 = mx + mw/2 - py;
                int px2 = mx + mw/2 + py;
                SDL_RenderDrawLine(renderer, px1, my - mh + py, px2, my - mh + py);
            }
        }

        // Sun with glow effect (multiple layers)
        int sunX = width - 120;
        int sunY = 100;
        for (int r = 60; r > 0; r -= 10) {
            Uint8 alpha = (Uint8)(40 + (60-r)*2);
            DrawFilledCircleLocal(renderer, sunX, sunY, r, Color(255, 240, 100, alpha));
        }
        DrawFilledCircleLocal(renderer, sunX, sunY, 35, Color(255, 235, 80));
        // Sun rays
        SDL_SetRenderDrawColor(renderer, 255, 245, 150, 120);
        for (int i = 0; i < 8; ++i) {
            float angle = (i * 45.0f) * 3.14159f / 180.0f;
            int ex = sunX + (int)(80 * std::cos(angle));
            int ey = sunY + (int)(80 * std::sin(angle));
            for (int j = 0; j < 3; ++j) {
                SDL_RenderDrawLine(renderer, sunX, sunY, ex + j, ey);
            }
        }

        // Multiple cloud layers with depth
        // Layer 1: Far clouds (smaller, lighter)
        DrawFilledCircleLocal(renderer, 100, 100, 18, Color(255,255,255,160));
        DrawFilledCircleLocal(renderer, 125, 95, 22, Color(255,255,255,160));
        DrawFilledCircleLocal(renderer, 150, 98, 20, Color(255,255,255,160));

        DrawFilledCircleLocal(renderer, width - 300, 130, 20, Color(255,255,255,160));
        DrawFilledCircleLocal(renderer, width - 270, 125, 25, Color(255,255,255,160));

        // Layer 2: Mid clouds (larger, more opaque)
        DrawFilledCircleLocal(renderer, 220, 160, 28, Color(255,255,255,220));
        DrawFilledCircleLocal(renderer, 255, 152, 35, Color(255,255,255,220));
        DrawFilledCircleLocal(renderer, 290, 158, 30, Color(255,255,255,220));

        // Layer 3: Close clouds (largest, solid)
        DrawFilledCircleLocal(renderer, width/2 - 80, 200, 32, Color(250,250,255,240));
        DrawFilledCircleLocal(renderer, width/2 - 40, 195, 40, Color(250,250,255,240));
        DrawFilledCircleLocal(renderer, width/2 + 5, 200, 35, Color(250,250,255,240));

        // Flying birds (V formations)
        SDL_SetRenderDrawColor(renderer, 40, 40, 50, 200);
        int birdX = width/2 + 150;
        int birdY = 140;
        for (int i = 0; i < 5; ++i) {
            int by = birdY + std::abs(i - 2) * 8;
            // Simple V shape
            SDL_RenderDrawLine(renderer, birdX + i*15, by, birdX + i*15 + 5, by - 5);
            SDL_RenderDrawLine(renderer, birdX + i*15 + 5, by - 5, birdX + i*15 + 10, by);
        }

        // Rolling green hills (multiple layers with shading)
        // Back hills (darker)
        SDL_SetRenderDrawColor(renderer, 70, 140, 70, 255);
        for (int i = 0; i < 3; ++i) {
            int hx = 100 + i * 250;
            int hy = height - 180;
            int hr = 200;
            for (int dy = 0; dy < hr/2; ++dy) {
                int hwidth = (int)(hr * std::sqrt(1.0f - (float)(dy*dy)/((hr/2)*(hr/2))));
                SDL_RenderDrawLine(renderer, hx - hwidth, hy + dy, hx + hwidth, hy + dy);
            }
        }

        // Front hills (brighter green)
        SDL_SetRenderDrawColor(renderer, 90, 180, 90, 255);
        for (int i = 0; i < 4; ++i) {
            int hx = 50 + i * 220;
            int hy = height - 120;
            int hr = 180;
            for (int dy = 0; dy < hr/2; ++dy) {
                int hwidth = (int)(hr * std::sqrt(1.0f - (float)(dy*dy)/((hr/2)*(hr/2))));
                SDL_RenderDrawLine(renderer, hx - hwidth, hy + dy, hx + hwidth, hy + dy);
            }
        }

        // Lush grass with texture
        SDL_Rect grassBase = {0, height - 60, width, 60};
        for (int y = 0; y < grassBase.h; ++y) {
            float t = (float)y / (float)grassBase.h;
            Uint8 g = (Uint8)(140 - 30*t);
            SDL_SetRenderDrawColor(renderer, 40, g, 50, 255);
            SDL_RenderDrawLine(renderer, 0, grassBase.y + y, width, grassBase.y + y);
        }
        // Grass blades (textured effect)
        SDL_SetRenderDrawColor(renderer, 50, 160, 60, 255);
        for (int x = 0; x < width; x += 8) {
            int gh = 10 + (x % 15);
            SDL_RenderDrawLine(renderer, x, height - 60, x, height - 60 + gh);
        }

        // Flowers (colorful dots)
        int flowerColors[][3] = {{255,100,100}, {255,200,50}, {150,100,255}, {255,150,200}};
        for (int i = 0; i < 20; ++i) {
            int fx = (i * 63) % width;
            int fy = height - 55 + (i % 10);
            int fc = i % 4;
            DrawFilledCircleLocal(renderer, fx, fy, 4, Color(flowerColors[fc][0], flowerColors[fc][1], flowerColors[fc][2]));
            // Stem
            SDL_SetRenderDrawColor(renderer, 50, 140, 50, 255);
            SDL_RenderDrawLine(renderer, fx, fy, fx, fy + 12);
        }

        // Dirt path/trail (winding)
        SDL_SetRenderDrawColor(renderer, 160, 130, 90, 180);
        int pathY = height - 100;
        for (int x = 0; x < width; x += 5) {
            int py = pathY + (int)(15 * std::sin(x * 0.02f));
            SDL_Rect pathSegment = {x, py, 8, 35};
            SDL_RenderFillRect(renderer, &pathSegment);
        }
    }
    else if (id == 3) {
        // ========== Stage 3: STUNNING NIGHT SKY ==========
        // Deep night gradient (dark blue to purple)
        for (int y = 0; y < height; ++y) {
            float t = (float)y / (float)height;
            Uint8 r = (Uint8)(15 + 35*t);
            Uint8 g = (Uint8)(10 + 25*t);
            Uint8 b = (Uint8)(40 + 50*t);
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawLine(renderer, 0, y, width, y);
        }

        // Aurora Borealis effect (top portion)
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        for (int y = 40; y < 200; ++y) {
            float wave = std::sin((y + currentTime * 30.0f) * 0.05f) * 30.0f;
            for (int x = 0; x < width; x += 3) {
                float wx = x + wave;
                float intensity = std::sin(wx * 0.01f) * std::sin(y * 0.02f);
                if (intensity > 0.3f) {
                    Uint8 alpha = (Uint8)(intensity * 80);
                    // Green-blue aurora
                    SDL_SetRenderDrawColor(renderer, 50, 255, 180, alpha);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Glowing moon with craters
        int moonX = width - 140;
        int moonY = 110;
        int moonR = 45;
        // Moon glow
        for (int r = moonR + 30; r > moonR; r -= 5) {
            Uint8 alpha = (Uint8)(20 + (moonR + 30 - r) * 2);
            DrawFilledCircleLocal(renderer, moonX, moonY, r, Color(240, 240, 200, alpha));
        }
        // Moon body
        DrawFilledCircleLocal(renderer, moonX, moonY, moonR, Color(240, 240, 220));
        // Craters (darker circles)
        DrawFilledCircleLocal(renderer, moonX - 12, moonY - 8, 8, Color(200, 200, 180));
        DrawFilledCircleLocal(renderer, moonX + 15, moonY + 5, 6, Color(205, 205, 185));
        DrawFilledCircleLocal(renderer, moonX - 5, moonY + 18, 10, Color(195, 195, 175));
        DrawFilledCircleLocal(renderer, moonX + 8, moonY - 20, 5, Color(210, 210, 190));

        // Constellation patterns (connected stars)
        SDL_SetRenderDrawColor(renderer, 200, 200, 255, 150);
        int constellation[][2] = {{150,120}, {180,100}, {210,115}, {190,145}, {160,150}};
        for (int i = 0; i < 4; ++i) {
            SDL_RenderDrawLine(renderer, constellation[i][0], constellation[i][1],
                             constellation[i+1][0], constellation[i+1][1]);
        }
        SDL_RenderDrawLine(renderer, constellation[4][0], constellation[4][1],
                          constellation[0][0], constellation[0][1]);

        // Dense star field (multiple sizes and brightness)
        for (int i = 0; i < 150; ++i) {
            int sx = (i * 73) % width;
            int sy = 20 + (i * 41) % (height/2 + 100);
            int size = 1 + (i % 3);
            Uint8 brightness = (Uint8)(180 + (i % 75));

            SDL_SetRenderDrawColor(renderer, brightness, brightness, 255, 255);
            if (size == 1) {
                SDL_RenderDrawPoint(renderer, sx, sy);
            } else {
                DrawFilledCircleLocal(renderer, sx, sy, size, Color(brightness, brightness, 255));
            }

            // Twinkling effect (some stars brighter)
            if (i % 7 == 0) {
                int twinkle = (int)(std::sin(currentTime * 3.0f + i) * 30);
                if (twinkle > 15) {
                    DrawFilledCircleLocal(renderer, sx, sy, size + 1,
                        Color(255, 255, 255, 180));
                }
            }
        }

        // Shooting stars (streaks)
        float shootTime = std::fmod(currentTime, 3.0f);
        if (shootTime < 0.5f) {
            int sx = width/2 + (int)(shootTime * 200);
            int sy = 80 + (int)(shootTime * 100);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
            for (int i = 0; i < 30; ++i) {
                int tx = sx - i*2;
                int ty = sy - i;
                Uint8 alpha = (Uint8)(200 - i*6);
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
                SDL_RenderDrawPoint(renderer, tx, ty);
            }
        }

        // Mountain silhouette at bottom
        SDL_SetRenderDrawColor(renderer, 15, 15, 25, 255);
        for (int i = 0; i < 5; ++i) {
            int mx = i * 180 - 50;
            int my = height - 150 + (i % 2) * 30;
            int mw = 220;
            int mh = 150 - (i % 3) * 20;
            // Triangle mountain shape
            for (int py = 0; py < mh; ++py) {
                int width_at_y = (int)((float)mw * (1.0f - (float)py / mh));
                int x1 = mx + mw/2 - width_at_y/2;
                int x2 = mx + mw/2 + width_at_y/2;
                SDL_RenderDrawLine(renderer, x1, my + py, x2, my + py);
            }
        }

        // Tree silhouettes (foreground)
        SDL_SetRenderDrawColor(renderer, 10, 10, 20, 255);
        for (int i = 0; i < 3; ++i) {
            int tx = 100 + i * 300;
            int ty = height - 140;
            // Trunk
            SDL_Rect trunk = {tx, ty, 20, 80};
            SDL_RenderFillRect(renderer, &trunk);
            // Triangular foliage
            for (int py = 0; py < 60; ++py) {
                int tw = 80 - py;
                SDL_RenderDrawLine(renderer, tx + 10 - tw/2, ty - py,
                                 tx + 10 + tw/2, ty - py);
            }
        }
    }
    else if (id == 4) {
        // ========== Stage 4: PROFESSIONAL LABORATORY ==========
        // Clean laboratory wall gradient
        for (int y = 0; y < height; ++y) {
            float t = (float)y / (float)height;
            Uint8 val = (Uint8)(235 - 10*t);
            SDL_SetRenderDrawColor(renderer, val, val, val + 5, 255);
            SDL_RenderDrawLine(renderer, 0, y, width, y);
        }

        // Tiled floor (checkered pattern)
        int tileSize = 40;
        for (int y = height - 140; y < height; y += tileSize) {
            for (int x = 0; x < width; x += tileSize) {
                bool dark = ((x/tileSize) + (y/tileSize)) % 2 == 0;
                SDL_Rect tile = {x, y, tileSize, tileSize};
                SDL_SetRenderDrawColor(renderer, dark ? 200 : 220, dark ? 200 : 220, dark ? 205 : 225, 255);
                SDL_RenderFillRect(renderer, &tile);
                SDL_SetRenderDrawColor(renderer, 180, 180, 185, 255);
                SDL_RenderDrawRect(renderer, &tile);
            }
        }

        // Laboratory bench (long counter)
        SDL_Rect benchTop = {40, height - 260, width - 80, 30};
        SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
        SDL_RenderFillRect(renderer, &benchTop);
        SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
        SDL_RenderDrawRect(renderer, &benchTop);
        // Bench shadow
        SDL_SetRenderDrawColor(renderer, 60, 60, 70, 180);
        SDL_Rect shadow = {benchTop.x + 5, benchTop.y + benchTop.h, benchTop.w - 5, 8};
        SDL_RenderFillRect(renderer, &shadow);

        // Bench legs/cabinets
        for (int i = 0; i < 4; ++i) {
            SDL_Rect cabinet = {benchTop.x + 20 + i * 220, benchTop.y + 30, 100, 110};
            SDL_SetRenderDrawColor(renderer, 160, 160, 170, 255);
            SDL_RenderFillRect(renderer, &cabinet);
            SDL_SetRenderDrawColor(renderer, 120, 120, 130, 255);
            SDL_RenderDrawRect(renderer, &cabinet);
            // Cabinet handles
            SDL_Rect handle = {cabinet.x + 40, cabinet.y + 50, 20, 8};
            SDL_SetRenderDrawColor(renderer, 100, 100, 110, 255);
            SDL_RenderFillRect(renderer, &handle);
        }

        // Laboratory equipment on bench
        // Beakers with colored liquids
        int beakerX = benchTop.x + 80;
        // Beaker 1 (blue liquid)
        SDL_SetRenderDrawColor(renderer, 200, 200, 210, 200);
        SDL_Rect beaker1 = {beakerX, benchTop.y - 80, 40, 80};
        SDL_RenderFillRect(renderer, &beaker1);
        SDL_Rect liquid1 = {beakerX + 2, beaker1.y + 30, 36, 48};
        SDL_SetRenderDrawColor(renderer, 100, 180, 230, 200);
        SDL_RenderFillRect(renderer, &liquid1);
        SDL_SetRenderDrawColor(renderer, 150, 150, 160, 255);
        SDL_RenderDrawRect(renderer, &beaker1);

        // Beaker 2 (green liquid) - larger
        beakerX += 80;
        SDL_SetRenderDrawColor(renderer, 200, 200, 210, 200);
        SDL_Rect beaker2 = {beakerX, benchTop.y - 100, 50, 100};
        SDL_RenderFillRect(renderer, &beaker2);
        SDL_Rect liquid2 = {beakerX + 2, beaker2.y + 35, 46, 63};
        SDL_SetRenderDrawColor(renderer, 120, 220, 140, 200);
        SDL_RenderFillRect(renderer, &liquid2);
        SDL_SetRenderDrawColor(renderer, 150, 150, 160, 255);
        SDL_RenderDrawRect(renderer, &beaker2);

        // Test tubes in rack
        beakerX += 100;
        SDL_Rect rack = {beakerX, benchTop.y - 50, 80, 10};
        SDL_SetRenderDrawColor(renderer, 140, 100, 60, 255);
        SDL_RenderFillRect(renderer, &rack);
        int tubeColors[][3] = {{255,100,100}, {255,200,100}, {200,100,255}, {100,255,200}};
        for (int i = 0; i < 4; ++i) {
            SDL_Rect tube = {beakerX + 8 + i*18, rack.y - 40, 12, 40};
            SDL_SetRenderDrawColor(renderer, 220, 220, 230, 200);
            SDL_RenderFillRect(renderer, &tube);
            SDL_Rect tubeLiquid = {tube.x + 1, tube.y + 15, 10, 24};
            SDL_SetRenderDrawColor(renderer, tubeColors[i][0], tubeColors[i][1], tubeColors[i][2], 200);
            SDL_RenderFillRect(renderer, &tubeLiquid);
            SDL_SetRenderDrawColor(renderer, 150, 150, 160, 255);
            SDL_RenderDrawRect(renderer, &tube);
        }

        // Microscope
        int microscopeX = benchTop.x + benchTop.w - 200;
        // Base
        SDL_Rect microBase = {microscopeX, benchTop.y - 25, 60, 25};
        SDL_SetRenderDrawColor(renderer, 80, 80, 90, 255);
        SDL_RenderFillRect(renderer, &microBase);
        // Arm
        SDL_Rect microArm = {microscopeX + 25, benchTop.y - 80, 10, 55};
        SDL_RenderFillRect(renderer, &microArm);
        // Eyepiece
        DrawFilledCircleLocal(renderer, microscopeX + 30, benchTop.y - 85, 12, Color(90,90,100));
        // Objective
        DrawFilledCircleLocal(renderer, microscopeX + 30, benchTop.y - 35, 8, Color(60,60,70));

        // Wall cabinets (top)
        for (int i = 0; i < 3; ++i) {
            SDL_Rect wallCab = {50 + i * 280, 60, 220, 140};
            SDL_SetRenderDrawColor(renderer, 180, 180, 190, 255);
            SDL_RenderFillRect(renderer, &wallCab);
            SDL_SetRenderDrawColor(renderer, 140, 140, 150, 255);
            SDL_RenderDrawRect(renderer, &wallCab);
            // Cabinet door frames
            SDL_RenderDrawLine(renderer, wallCab.x + 110, wallCab.y,
                             wallCab.x + 110, wallCab.y + wallCab.h);
            SDL_RenderDrawLine(renderer, wallCab.x, wallCab.y + 70,
                             wallCab.x + wallCab.w, wallCab.y + 70);
        }

        // Periodic table poster
        SDL_Rect poster = {width - 260, 60, 200, 130};
        SDL_SetRenderDrawColor(renderer, 255, 255, 250, 255);
        SDL_RenderFillRect(renderer, &poster);
        SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
        SDL_RenderDrawRect(renderer, &poster);
        // Element boxes (simplified)
        int elemColors[][3] = {{255,180,180}, {180,255,180}, {180,180,255}, {255,255,180}};
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 6; ++col) {
                SDL_Rect elem = {poster.x + 10 + col*30, poster.y + 15 + row*26, 26, 22};
                int colorIdx = (row + col) % 4;
                SDL_SetRenderDrawColor(renderer, elemColors[colorIdx][0],
                                      elemColors[colorIdx][1], elemColors[colorIdx][2], 255);
                SDL_RenderFillRect(renderer, &elem);
                SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                SDL_RenderDrawRect(renderer, &elem);
            }
        }

        // Safety equipment on wall
        // Fire extinguisher
        SDL_Rect extinguisher = {20, height/2, 30, 80};
        SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        SDL_RenderFillRect(renderer, &extinguisher);
        SDL_Rect extTop = {extinguisher.x + 8, extinguisher.y - 15, 14, 15};
        SDL_SetRenderDrawColor(renderer, 180, 180, 190, 255);
        SDL_RenderFillRect(renderer, &extTop);
    }
    else { // id == 5 or default
        // ========== Stage 5: MAGICAL SPACE SCENE ==========
        // Deep space gradient (black to deep purple/blue)
        for (int y = 0; y < height; ++y) {
            float t = (float)y / (float)height;
            Uint8 r = (Uint8)(5 + 35*t);
            Uint8 g = (Uint8)(5 + 20*t);
            Uint8 b = (Uint8)(15 + 60*t);
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderDrawLine(renderer, 0, y, width, y);
        }

        // Nebula effect (colorful gas clouds)
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        // Purple nebula (left)
        for (int y = 100; y < 300; ++y) {
            for (int x = 50; x < 300; ++x) {
                float dist = std::sqrt((x-175)*(x-175) + (y-200)*(y-200));
                if (dist < 120) {
                    float intensity = (120 - dist) / 120.0f;
                    Uint8 alpha = (Uint8)(intensity * 60);
                    SDL_SetRenderDrawColor(renderer, 180, 50, 255, alpha);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
        // Blue-green nebula (right)
        for (int y = 150; y < 350; ++y) {
            for (int x = width - 280; x < width - 50; ++x) {
                float dist = std::sqrt((x-(width-165))*(x-(width-165)) + (y-250)*(y-250));
                if (dist < 100) {
                    float intensity = (100 - dist) / 100.0f;
                    Uint8 alpha = (Uint8)(intensity * 50);
                    SDL_SetRenderDrawColor(renderer, 50, 200, 255, alpha);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Dense starfield (varying sizes and colors)
        for (int i = 0; i < 200; ++i) {
            int sx = (i * 67) % width;
            int sy = (i * 53) % height;
            int size = 1 + (i % 4);
            int colorVar = i % 5;
            Color starColor;

            switch(colorVar) {
                case 0: starColor = Color(255, 255, 255); break; // white
                case 1: starColor = Color(200, 220, 255); break; // blue-white
                case 2: starColor = Color(255, 250, 200); break; // yellow-white
                case 3: starColor = Color(255, 200, 200); break; // red-white
                default: starColor = Color(230, 230, 255); break; // bright white
            }

            // Twinkling
            if (i % 5 == 0) {
                float twinkle = std::sin(currentTime * 2.5f + i * 0.5f);
                if (twinkle > 0.5f) {
                    size += 1;
                    starColor.a = (Uint8)(200 + twinkle * 55);
                }
            }

            if (size == 1) {
                SDL_SetRenderDrawColor(renderer, starColor.r, starColor.g, starColor.b, starColor.a);
                SDL_RenderDrawPoint(renderer, sx, sy);
            } else {
                DrawFilledCircleLocal(renderer, sx, sy, size, starColor);
            }
        }

        // Large planet (bottom right)
        int planetX = width - 180;
        int planetY = height - 150;
        int planetR = 100;
        // Planet atmosphere glow
        for (int r = planetR + 25; r > planetR; r -= 5) {
            Uint8 alpha = (Uint8)(15 + (planetR + 25 - r));
            DrawFilledCircleLocal(renderer, planetX, planetY, r, Color(100, 150, 255, alpha));
        }
        // Planet body (layered for depth)
        DrawFilledCircleLocal(renderer, planetX, planetY, planetR, Color(80, 120, 200));
        // Planet bands/stripes
        SDL_SetRenderDrawColor(renderer, 60, 100, 180, 180);
        for (int i = 0; i < 5; ++i) {
            int by = planetY - 60 + i * 30;
            for (int bx = planetX - planetR; bx < planetX + planetR; ++bx) {
                int dy = by - planetY;
                int dx = bx - planetX;
                if (dx*dx + dy*dy < planetR * planetR) {
                    SDL_RenderDrawPoint(renderer, bx, by);
                    SDL_RenderDrawPoint(renderer, bx, by + 1);
                }
            }
        }
        // Planet highlight (light side)
        DrawFilledCircleLocal(renderer, planetX - 30, planetY - 30, 25, Color(120, 160, 255, 100));

        // Smaller moon orbiting
        int moonX = planetX - 140;
        int moonY = planetY + 20;
        DrawFilledCircleLocal(renderer, moonX, moonY, 20, Color(180, 180, 190));
        DrawFilledCircleLocal(renderer, moonX - 5, moonY - 3, 5, Color(150, 150, 160)); // crater

        // Saturn-like rings for planet
        SDL_SetRenderDrawColor(renderer, 150, 120, 80, 150);
        for (int ry = -10; ry < 10; ++ry) {
            int ringY = planetY + ry;
            for (int rx = -140; rx < 140; ++rx) {
                int ringX = planetX + rx;
                int dy = ringY - planetY;
                int dx = ringX - planetX;
                float dist = std::sqrt(dx*dx + dy*dy);
                if (dist > planetR + 10 && dist < planetR + 40 && std::abs(ry) < 8) {
                    // Only draw if in front of planet
                    if (dy < 0 || dx*dx + dy*dy > planetR*planetR) {
                        SDL_RenderDrawPoint(renderer, ringX, ringY);
                    }
                }
            }
        }

        // Asteroids (small rocky objects)
        int asteroidPos[][2] = {{200,350}, {450,250}, {120,450}, {width-300,180}};
        for (int i = 0; i < 4; ++i) {
            int ax = asteroidPos[i][0];
            int ay = asteroidPos[i][1];
            int ar = 15 + i * 3;
            // Irregular shape (approximated with overlapping circles)
            DrawFilledCircleLocal(renderer, ax, ay, ar, Color(120, 110, 100));
            DrawFilledCircleLocal(renderer, ax + ar/2, ay - ar/3, ar/2, Color(100, 90, 85));
            DrawFilledCircleLocal(renderer, ax - ar/3, ay + ar/3, ar/2, Color(110, 100, 90));
            // Craters
            DrawFilledCircleLocal(renderer, ax + 3, ay - 2, 3, Color(80, 75, 70));
        }

        // Distant galaxy (spiral effect)
        int galaxyX = width/2 - 100;
        int galaxyY = 120;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
        for (int i = 0; i < 100; ++i) {
            float angle = i * 0.3f;
            float radius = i * 0.8f;
            int gx = galaxyX + (int)(radius * std::cos(angle));
            int gy = galaxyY + (int)(radius * std::sin(angle) * 0.6f);
            Uint8 alpha = (Uint8)(80 - i * 0.6f);
            SDL_SetRenderDrawColor(renderer, 200, 180, 255, alpha);
            SDL_RenderDrawPoint(renderer, gx, gy);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Shooting comet
        float cometTime = std::fmod(currentTime * 0.5f, 4.0f);
        if (cometTime < 1.5f) {
            int cx = 100 + (int)(cometTime * 400);
            int cy = 200 + (int)(cometTime * 150);
            // Comet head
            DrawFilledCircleLocal(renderer, cx, cy, 6, Color(255, 255, 240));
            DrawFilledCircleLocal(renderer, cx, cy, 10, Color(255, 255, 200, 100));
            // Comet tail
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
            for (int i = 0; i < 50; ++i) {
                int tx = cx - i * 3;
                int ty = cy - i * 2;
                Uint8 alpha = (Uint8)(150 - i * 3);
                int size = 3 - i/20;
                if (size < 1) size = 1;
                DrawFilledCircleLocal(renderer, tx, ty, size, Color(255, 240, 180, alpha));
            }
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        }

        // Space station silhouette (small, in distance)
        SDL_SetRenderDrawColor(renderer, 160, 160, 170, 255);
        SDL_Rect station = {width/2 + 200, height/2 - 80, 80, 20};
        SDL_RenderFillRect(renderer, &station);
        SDL_Rect solarPanel1 = {station.x - 30, station.y - 5, 25, 30};
        SDL_RenderFillRect(renderer, &solarPanel1);
        SDL_Rect solarPanel2 = {station.x + station.w + 5, station.y - 5, 25, 30};
        SDL_RenderFillRect(renderer, &solarPanel2);
        DrawFilledCircleLocal(renderer, station.x + 40, station.y + 10, 12, Color(180, 180, 190));
    }

    // restore clip
    SDL_RenderSetClipRect(renderer, nullptr);
}

void GameEngine::setCurrentLevel(int id) {
    const Level* L = getLevelById(id);
    if (!L) return;
    unlockLevel(id);
    progress.currentLevelId = id;
    saveProgress();
}

// --- NEW METHODS: Text Wrapping ---

std::vector<std::string> GameEngine::wrapText(const std::string& text, int maxWidth, TTF_Font* font) {
    std::vector<std::string> lines;
    if (!font) {
        lines.push_back(text);
        return lines;
    }

    std::istringstream words(text);
    std::string word, currentLine;

    while (words >> word) {
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;

        int w, h;
        if (TTF_SizeText(font, testLine.c_str(), &w, &h) == 0) {
            if (w > maxWidth && !currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine = word;
            } else {
                currentLine = testLine;
            }
        } else {
            currentLine = testLine;
        }
    }

    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}

void GameEngine::drawTextWrapped(SDL_Renderer* r, TTF_Font* font, const std::string& text, int x, int y, int maxW, const Color& color) {
    auto lines = wrapText(text, maxW, font);
    int lineY = y;
    for (const auto& line : lines) {
        drawText(r, font, line, x, lineY, color);
        lineY += 18;  // Line height
    }
}

// --- NEW METHODS: Visual Components ---

void GameEngine::renderProgressBar(SDL_Renderer* r, int x, int y, int w, int h, float progress, const std::string& label, TTF_Font* font) {
    // Background
    SDL_Rect bg = {x, y, w, h};
    SDL_SetRenderDrawColor(r, 220, 220, 220, 255);
    SDL_RenderFillRect(r, &bg);
    SDL_SetRenderDrawColor(r, 150, 150, 150, 255);
    SDL_RenderDrawRect(r, &bg);

    // Fill
    int fillW = (int)(w * progress);
    if (fillW > 0) {
        SDL_Rect fill = {x, y, fillW, h};
        SDL_SetRenderDrawColor(r, 50, 180, 50, 255);
        SDL_RenderFillRect(r, &fill);
    }

    // Label overlay
    if (!label.empty() && font) {
        std::string labelText = label + ": " + std::to_string((int)(progress * 100)) + "%";
        drawText(r, font, labelText, x + 8, y + 4, Color::White());
    }
}

void GameEngine::renderStarRating(SDL_Renderer* r, int stars, int maxStars, int x, int y, int size) {
    for (int i = 0; i < maxStars; ++i) {
        Color starColor = (i < stars) ? Color::Yellow() : Color::Gray();

        // Draw simple star (approximated with filled circle for simplicity)
        for (int dy = 0; dy < size; ++dy) {
            for (int dx = 0; dx < size; ++dx) {
                int cx = size / 2;
                int cy = size / 2;
                float dist = std::sqrt((float)((dx - cx)*(dx - cx) + (dy - cy)*(dy - cy)));
                if (dist <= size / 2.0f) {
                    SDL_SetRenderDrawColor(r, starColor.r, starColor.g, starColor.b, starColor.a);
                    SDL_RenderDrawPoint(r, x + i * (size + 4) + dx, y + dy);
                }
            }
        }
    }
}

void GameEngine::renderBadge(SDL_Renderer* r, const Achievement& badge, int x, int y, int size) {
    // Draw badge shape based on iconType
    if (badge.iconType == "star") {
        // Yellow star (circular approximation)
        for (int dy = 0; dy < size; ++dy) {
            for (int dx = 0; dx < size; ++dx) {
                int cx = size / 2;
                int cy = size / 2;
                float dist = std::sqrt((float)((dx - cx)*(dx - cx) + (dy - cy)*(dy - cy)));
                if (dist <= size / 2.0f) {
                    SDL_SetRenderDrawColor(r, badge.iconColor.r, badge.iconColor.g, badge.iconColor.b, badge.iconColor.a);
                    SDL_RenderDrawPoint(r, x + dx, y + dy);
                }
            }
        }
    } else if (badge.iconType == "trophy") {
        // Orange trophy (simplified rectangle + circle)
        SDL_Rect base = {x + size/4, y + size/2, size/2, size/2};
        SDL_SetRenderDrawColor(r, badge.iconColor.r, badge.iconColor.g, badge.iconColor.b, badge.iconColor.a);
        SDL_RenderFillRect(r, &base);

        // Trophy cup top
        for (int dy = 0; dy < size/3; ++dy) {
            for (int dx = size/4; dx < 3*size/4; ++dx) {
                SDL_SetRenderDrawColor(r, badge.iconColor.r, badge.iconColor.g, badge.iconColor.b, badge.iconColor.a);
                SDL_RenderDrawPoint(r, x + dx, y + size/3 + dy);
            }
        }
    } else if (badge.iconType == "medal") {
        // Medal (circle with ribbon)
        for (int dy = size/4; dy < 3*size/4; ++dy) {
            for (int dx = size/4; dx < 3*size/4; ++dx) {
                int cx = size / 2;
                int cy = size / 2;
                float dist = std::sqrt((float)((dx - cx)*(dx - cx) + (dy - cy)*(dy - cy)));
                if (dist <= size / 4.0f) {
                    SDL_SetRenderDrawColor(r, badge.iconColor.r, badge.iconColor.g, badge.iconColor.b, badge.iconColor.a);
                    SDL_RenderDrawPoint(r, x + dx, y + dy);
                }
            }
        }
    } else {
        // Default: simple circle
        for (int dy = 0; dy < size; ++dy) {
            for (int dx = 0; dx < size; ++dx) {
                int cx = size / 2;
                int cy = size / 2;
                float dist = std::sqrt((float)((dx - cx)*(dx - cx) + (dy - cy)*(dy - cy)));
                if (dist <= size / 2.0f) {
                    SDL_SetRenderDrawColor(r, badge.iconColor.r, badge.iconColor.g, badge.iconColor.b, badge.iconColor.a);
                    SDL_RenderDrawPoint(r, x + dx, y + dy);
                }
            }
        }
    }
}

// NEW: Animated Progress Bar with smooth filling
void GameEngine::renderAnimatedProgressBar(SDL_Renderer* r, int x, int y, int w, int h, float targetProgress, const std::string& label, TTF_Font* font) {
    // Update target
    if (std::abs(progressBarAnimTarget - targetProgress) > 0.01f) {
        progressBarAnimTarget = targetProgress;
    }

    // Background with subtle gradient
    for (int i = 0; i < h; ++i) {
        float t = (float)i / h;
        Uint8 gray = (Uint8)(220 - t * 20);
        SDL_SetRenderDrawColor(r, gray, gray, gray, 255);
        SDL_Rect line = {x, y + i, w, 1};
        SDL_RenderFillRect(r, &line);
    }

    // Border
    SDL_SetRenderDrawColor(r, 150, 150, 150, 255);
    SDL_Rect border = {x, y, w, h};
    SDL_RenderDrawRect(r, &border);

    // Animated fill with gradient (green to light green)
    int fillW = (int)(w * progressBarAnimCurrent);
    if (fillW > 0) {
        for (int i = 0; i < h; ++i) {
            float t = (float)i / h;
            Uint8 r_val = (Uint8)(50 + t * 100);
            Uint8 g_val = (Uint8)(180 + t * 20);
            Uint8 b_val = 50;
            SDL_SetRenderDrawColor(r, r_val, g_val, b_val, 255);
            SDL_Rect line = {x, y + i, fillW, 1};
            SDL_RenderFillRect(r, &line);
        }

        // Shine effect on top
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 255, 255, 255, 80);
        SDL_Rect shine = {x, y, fillW, h / 3};
        SDL_RenderFillRect(r, &shine);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    }

    // Label overlay with shadow
    if (!label.empty() && font) {
        std::string labelText = label + ": " + std::to_string((int)(progressBarAnimCurrent * 100)) + "%";
        // Shadow
        drawText(r, font, labelText, x + 9, y + 5, Color(0, 0, 0, 128));
        // Text
        drawText(r, font, labelText, x + 8, y + 4, Color::White());
    }
}

// NEW: Professional Button with hover effect
void GameEngine::renderButton(SDL_Renderer* r, const std::string& text, int x, int y, int w, int h, const Color& color, TTF_Font* font, bool hovered) {
    // Button background with gradient
    for (int i = 0; i < h; ++i) {
        float t = (float)i / h;
        Uint8 r_val = hovered ? (Uint8)(color.r + 30) : color.r;
        Uint8 g_val = hovered ? (Uint8)(color.g + 30) : color.g;
        Uint8 b_val = hovered ? (Uint8)(color.b + 30) : color.b;

        r_val = (Uint8)(r_val - t * 40);
        g_val = (Uint8)(g_val - t * 40);
        b_val = (Uint8)(b_val - t * 40);

        SDL_SetRenderDrawColor(r, r_val, g_val, b_val, 255);
        SDL_Rect line = {x, y + i, w, 1};
        SDL_RenderFillRect(r, &line);
    }

    // Highlight on top edge
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 255, 255, 255, hovered ? 120 : 80);
    SDL_Rect highlight = {x, y, w, h / 4};
    SDL_RenderFillRect(r, &highlight);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    // Border (lighter when hovered)
    Color borderColor = hovered ? Color::White() : Color(200, 200, 200);
    SDL_SetRenderDrawColor(r, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_Rect btnRect = {x, y, w, h};
    SDL_RenderDrawRect(r, &btnRect);

    // Shadow on bottom edge when not hovered
    if (!hovered) {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 60);
        SDL_RenderDrawLine(r, x, y + h + 1, x + w, y + h + 1);
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    }

    // Text centered with shadow
    if (font) {
        int textX = x + (w - (int)text.length() * 7) / 2;
        int textY = y + (h - 14) / 2;
        // Shadow
        drawText(r, font, text, textX + 1, textY + 1, Color(0, 0, 0, 180));
        // Text
        drawText(r, font, text, textX, textY, Color::White());
    }
}

// NEW: Animated Feedback Modal
void GameEngine::renderFeedbackModal(SDL_Renderer* r, TTF_Font* font, TTF_Font* small, int screenW, int screenH) {
    if (!showingFeedback || feedbackAnimTime < 0.1f) return;

    // Fade in animation (first 0.3 seconds)
    float alpha = std::min(1.0f, feedbackAnimTime / 0.3f);

    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 0, 0, 0, (Uint8)(120 * alpha));
    SDL_Rect overlay = {0, 0, screenW, screenH};
    SDL_RenderFillRect(r, &overlay);

    // Modal window with bounce animation
    int modalW = 500;
    int modalH = 400;
    int modalX = (screenW - modalW) / 2;
    int modalY = (screenH - modalH) / 2;

    // Bounce effect (first 0.5 seconds)
    if (feedbackAnimTime < 0.5f) {
        float t = feedbackAnimTime / 0.5f;
        float bounce = 1.0f + 0.2f * std::sin(t * 3.14159f * 2.0f) * (1.0f - t);
        int scaleW = (int)(modalW * bounce);
        int scaleH = (int)(modalH * bounce);
        modalX = (screenW - scaleW) / 2;
        modalY = (screenH - scaleH) / 2;
        modalW = scaleW;
        modalH = scaleH;
    }

    // Shadow
    SDL_SetRenderDrawColor(r, 0, 0, 0, (Uint8)(100 * alpha));
    SDL_Rect shadow = {modalX + 8, modalY + 8, modalW, modalH};
    SDL_RenderFillRect(r, &shadow);

    // Modal background with gradient
    for (int i = 0; i < modalH; ++i) {
        float t = (float)i / modalH;
        Uint8 val = (Uint8)(255 - t * 20);
        SDL_SetRenderDrawColor(r, val, val, val, (Uint8)(255 * alpha));
        SDL_Rect line = {modalX, modalY + i, modalW, 1};
        SDL_RenderFillRect(r, &line);
    }

    // Border with result color
    SDL_SetRenderDrawColor(r, lastResult.feedbackColor.r, lastResult.feedbackColor.g,
                           lastResult.feedbackColor.b, (Uint8)(255 * alpha));
    for (int i = 0; i < 4; ++i) {
        SDL_Rect borderRect = {modalX - i, modalY - i, modalW + i*2, modalH + i*2};
        SDL_RenderDrawRect(r, &borderRect);
    }

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    // Content (only render if fully visible)
    if (alpha >= 0.9f) {
        int cx = modalX + 30;
        int cy = modalY + 30;

        // Result header
        if (lastResult.mastered) {
            drawText(r, font, "EXCELLENT WORK!", cx + 80, cy, Color::Green());
        } else if (lastResult.score >= 0.5f) {
            drawText(r, font, "GOOD EFFORT!", cx + 100, cy, Color::Orange());
        } else {
            drawText(r, font, "KEEP PRACTICING!", cx + 70, cy, Color::Red());
        }
        cy += 45;

        // Star rating with animation
        int starSize = 40;
        int starStartX = cx + 100;
        for (int i = 0; i < 5; ++i) {
            // Stagger star appearance
            float starAlpha = std::max(0.0f, std::min(1.0f, (feedbackAnimTime - 0.3f - i * 0.1f) / 0.2f));
            if (starAlpha > 0.0f) {
                int starY = cy - (int)(5 * std::sin(starAlpha * 3.14159f)); // Bounce in
                Color starColor = (i < lastResult.starRating) ? Color::Yellow() : Color::Gray();

                // Draw star (filled circle for simplicity)
                for (int dy = 0; dy < starSize; ++dy) {
                    for (int dx = 0; dx < starSize; ++dx) {
                        int scx = starSize / 2;
                        int scy = starSize / 2;
                        float dist = std::sqrt((float)((dx - scx)*(dx - scx) + (dy - scy)*(dy - scy)));
                        if (dist <= starSize / 2.0f) {
                            SDL_SetRenderDrawColor(r, starColor.r, starColor.g, starColor.b, (Uint8)(starColor.a * starAlpha));
                            SDL_RenderDrawPoint(r, starStartX + i * (starSize + 8) + dx, starY + dy);
                        }
                    }
                }
            }
        }
        cy += starSize + 25;

        // Points awarded with animation
        std::string pointsText = "+" + std::to_string(lastResult.pointsAwarded) + " Points!";
        // Scale effect on points
        float pointsScale = 1.0f + 0.3f * std::max(0.0f, 1.0f - (feedbackAnimTime - 0.8f) / 0.3f);
        drawText(r, font, pointsText, cx + 140, cy, Color::Blue());
        cy += 40;

        // Strengths
        if (!lastResult.strengths.empty() && feedbackAnimTime > 1.2f) {
            drawText(r, small, "Strengths:", cx, cy, Color::Green());
            cy += 22;
            for (size_t i = 0; i < lastResult.strengths.size() && i < 3; ++i) {
                drawText(r, small, "• " + lastResult.strengths[i], cx + 15, cy, Color(60,120,60));
                cy += 20;
            }
            cy += 10;
        }

        // Improvements
        if (!lastResult.improvements.empty() && feedbackAnimTime > 1.5f) {
            drawText(r, small, "Areas to Improve:", cx, cy, Color::Orange());
            cy += 22;
            for (size_t i = 0; i < lastResult.improvements.size() && i < 3; ++i) {
                drawText(r, small, "• " + lastResult.improvements[i], cx + 15, cy, Color(120,80,40));
                cy += 20;
            }
        }

        // Motivational message at bottom
        int msgY = modalY + modalH - 60;
        drawText(r, font, lastResult.motivationalMessage, cx + 60, msgY, lastResult.feedbackColor);

        // Close hint
        drawText(r, small, "Click anywhere to continue...", cx + 110, msgY + 30, Color::Gray());
    }
}

// --- NEW METHODS: Animation System ---

void GameEngine::update(float deltaTime) {
    currentTime += deltaTime;
    updateAnimations(deltaTime);
    updateParticles(deltaTime);

    // Spawn ambient particles periodically
    static float particleSpawnTimer = 0.0f;
    particleSpawnTimer += deltaTime;
    if (particleSpawnTimer > 0.3f) {
        particleSpawnTimer = 0.0f;
        // Spawn particles in random locations
        int x = rand() % 1200;
        int y = rand() % 700;
        Color particleColor = Color(150 + rand()%105, 150 + rand()%105, 200 + rand()%55, 180);
        spawnParticles(Vector2((float)x, (float)y), 1, particleColor);
    }

    // Panel slide-in animation (smooth ease-out)
    if (panelSlideProgress < 1.0f) {
        panelSlideProgress += deltaTime * 3.0f; // 3x speed for quick appearance
        if (panelSlideProgress > 1.0f) panelSlideProgress = 1.0f;
        // Ease out cubic
        float t = panelSlideProgress;
        panelSlideProgress = 1.0f - std::pow(1.0f - t, 3.0f);
    }

    // Animated progress bar (smooth fill)
    if (std::abs(progressBarAnimCurrent - progressBarAnimTarget) > 0.01f) {
        float diff = progressBarAnimTarget - progressBarAnimCurrent;
        progressBarAnimCurrent += diff * deltaTime * 5.0f; // Smooth interpolation
    }

    // Feedback animation timer
    if (showingFeedback) {
        feedbackAnimTime += deltaTime;
        if (feedbackAnimTime > 5.0f) { // Show feedback for 5 seconds
            showingFeedback = false;
            feedbackAnimTime = 0.0f;
        }
    }
}

void GameEngine::updateAnimations(float deltaTime) {
    for (auto& anim : activeAnimations) {
        if (!anim.active) continue;

        float elapsed = currentTime - anim.startTime;
        float t = std::min(1.0f, elapsed / anim.duration);

        // Apply easing based on animation type
        switch(anim.type) {
            case Animation::BOUNCE:
                t = 1.0f - std::abs(std::cos(t * 3.14159f * 2)) * (1.0f - t);
                break;
            case Animation::FADE_IN:
                anim.currentAlpha = (float)(255 * t);
                break;
            case Animation::SCALE_UP:
                anim.currentScale = t;
                break;
            default:
                break;
        }

        if (t >= 1.0f) {
            anim.active = false;
        }
    }

    // Remove completed animations
    activeAnimations.erase(
        std::remove_if(activeAnimations.begin(), activeAnimations.end(),
                      [](const Animation& a) { return !a.active; }),
        activeAnimations.end()
    );
}

void GameEngine::addAnimation(Animation::Type type, Vector2 pos, float duration) {
    Animation anim;
    anim.type = type;
    anim.startTime = currentTime;
    anim.duration = duration;
    anim.startPos = pos;
    anim.endPos = pos;
    anim.currentAlpha = 0;
    anim.currentScale = 0.0f;
    anim.active = true;
    activeAnimations.push_back(anim);
}

void GameEngine::renderAnimations(SDL_Renderer* r) {
    // Placeholder for future animation rendering
    (void)r;  // Suppress unused parameter warning
}

// --- NEW METHODS: Particle System ---

void GameEngine::spawnParticles(Vector2 position, int count, Color color) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.position = position;
        // Random velocity in all directions
        float angle = ((float)(rand() % 360)) * 3.14159f / 180.0f;
        float speed = 20.0f + (rand() % 60);
        p.velocity = Vector2(std::cos(angle) * speed, std::sin(angle) * speed);
        p.color = color;
        p.maxLifetime = 1.0f + (float)(rand() % 100) / 100.0f;
        p.lifetime = p.maxLifetime;
        p.size = 2.0f + (float)(rand() % 6);
        p.active = true;
        particles.push_back(p);
    }
}

void GameEngine::updateParticles(float deltaTime) {
    for (auto& p : particles) {
        if (!p.active) continue;

        p.lifetime -= deltaTime;
        if (p.lifetime <= 0.0f) {
            p.active = false;
            continue;
        }

        // Update position
        p.position.x += p.velocity.x * deltaTime;
        p.position.y += p.velocity.y * deltaTime;

        // Apply gravity
        p.velocity.y += 200.0f * deltaTime;

        // Fade out
        float lifeRatio = p.lifetime / p.maxLifetime;
        p.color.a = (Uint8)(255 * lifeRatio);
    }

    // Remove inactive particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                      [](const Particle& p) { return !p.active; }),
        particles.end()
    );
}

void GameEngine::renderParticles(SDL_Renderer* r) {
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    for (const auto& p : particles) {
        if (!p.active) continue;

        int size = (int)p.size;
        for (int dy = -size; dy <= size; ++dy) {
            for (int dx = -size; dx <= size; ++dx) {
                if (dx*dx + dy*dy <= size*size) {
                    SDL_SetRenderDrawColor(r, p.color.r, p.color.g, p.color.b, p.color.a);
                    SDL_RenderDrawPoint(r, (int)p.position.x + dx, (int)p.position.y + dy);
                }
            }
        }
    }
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

// --- NEW METHODS: Achievement System ---

void GameEngine::initializeAchievements() {
    achievements = {
        {"first_steps", "First Steps", "Complete your first level", "star", Color::Yellow(), 50, false, 0.0f},
        {"rising_star", "Rising Star", "Earn 500 points", "trophy", Color::Orange(), 100, false, 0.0f},
        {"perfect", "Perfect Score", "Master a level on first try", "medal", Color::Blue(), 200, false, 0.0f},
        {"persistent", "Persistent Learner", "Retry a level 3 times and succeed", "lightning", Color::Purple(), 150, false, 0.0f},
        {"scholar", "Scholar", "Complete all 5 levels", "trophy", Color(255, 215, 0), 500, false, 0.0f}
    };
}

void GameEngine::checkAndUnlockAchievements() {
    // Check "First Steps" - complete any level
    if (!achievements[0].unlocked) {
        for (const auto& badge : progress.badges) {
            if (badge.find("Level") != std::string::npos && badge.find("Mastery") != std::string::npos) {
                achievements[0].unlocked = true;
                achievements[0].unlockTime = currentTime;
                awardPoints(achievements[0].pointValue);
                break;
            }
        }
    }

    // Check "Rising Star" - 500+ points
    if (!achievements[1].unlocked && progress.totalPoints >= 500) {
        achievements[1].unlocked = true;
        achievements[1].unlockTime = currentTime;
        awardPoints(achievements[1].pointValue);
    }

    // Check "Perfect Score" - master level on first attempt
    if (!achievements[2].unlocked) {
        for (const auto& att : progress.attempts) {
            if (att.second == 1 && progress.mastery.count(att.first) && progress.mastery.at(att.first) >= 0.7f) {
                achievements[2].unlocked = true;
                achievements[2].unlockTime = currentTime;
                awardPoints(achievements[2].pointValue);
                break;
            }
        }
    }

    // Check "Persistent" - 3+ attempts then succeed
    if (!achievements[3].unlocked) {
        for (const auto& att : progress.attempts) {
            if (att.second >= 3 && progress.mastery.count(att.first) && progress.mastery.at(att.first) >= 0.7f) {
                achievements[3].unlocked = true;
                achievements[3].unlockTime = currentTime;
                awardPoints(achievements[3].pointValue);
                break;
            }
        }
    }

    // Check "Scholar" - complete all 5 levels
    if (!achievements[4].unlocked) {
        int mastered = 0;
        for (const auto& m : progress.mastery) {
            if (m.second >= 0.7f) mastered++;
        }
        if (mastered >= 5) {
            achievements[4].unlocked = true;
            achievements[4].unlockTime = currentTime;
            awardPoints(achievements[4].pointValue);
        }
    }
}

// --- NEW METHODS: Scenario System ---

std::vector<std::shared_ptr<Entity>> GameEngine::getScenarioEntities(int levelId) {
    std::vector<std::shared_ptr<Entity>> entities;
    const Level* L = getLevelById(levelId);
    if (!L) return entities;

    for (const auto& setup : L->scenarioSetup.entities) {
        std::shared_ptr<Entity> e;

        if (setup.type == "ball") {
            e = std::make_shared<Ball>(setup.position, setup.size);
        } else if (setup.type == "block") {
            e = std::make_shared<Block>(setup.position, setup.size, setup.size);
        } else if (setup.type == "ramp") {
            e = std::make_shared<Ramp>(setup.position, setup.size, setup.size * 0.5f);
        } else if (setup.type == "ground") {
            e = std::make_shared<Ground>(setup.position, setup.size, 30.0f);
        }

        if (e) {
            e->setColor(setup.color);
            if (setup.mass > 0) e->setMass(setup.mass);
            e->setVelocity(setup.velocity);
            if (!setup.label.empty()) {
                e->setName(setup.label);
            }
            entities.push_back(e);
        }
    }

    return entities;
}

ScenarioSetup GameEngine::getCurrentScenarioSetup() const {
    return getCurrentLevel().scenarioSetup;
}

// --- NEW METHODS: Hint System ---

std::string GameEngine::getNextHint(int levelId) {
    const Level* L = getLevelById(levelId);
    if (!L) return "";

    int hintIndex = hintsUsed[levelId];
    if (hintIndex >= (int)L->progressiveHints.size()) {
        return "No more hints available for this level.";
    }

    std::string hint = L->progressiveHints[hintIndex];
    hintsUsed[levelId]++;
    return hint;
}

bool GameEngine::hasAvailableHint(int levelId) const {
    const Level* L = getLevelById(levelId);
    if (!L) return false;

    auto it = hintsUsed.find(levelId);
    int used = (it != hintsUsed.end()) ? it->second : 0;
    return used < (int)L->progressiveHints.size();
}

void GameEngine::renderUI(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* smallFont, int x, int y, int w, int h) {
    // Render particles FIRST (behind UI)
    renderParticles(renderer);

    // Apply slide-in animation
    int slideOffset = (int)((1.0f - panelSlideProgress) * w);
    x += slideOffset;

    // Draw animated shadow with slide
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < 8; ++i) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, (Uint8)(15 - i * 2));
        SDL_Rect shadow = {x + i, y + i, w, h};
        SDL_RenderDrawRect(renderer, &shadow);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Panel background with vertical gradient
    for (int i = 0; i < h; ++i) {
        float t = (float)i / h;
        Uint8 val = (Uint8)(250 - t * 15);
        SDL_SetRenderDrawColor(renderer, val, val, val + 5, 240);
        SDL_Rect line = {x, y + i, w, 1};
        SDL_RenderFillRect(renderer, &line);
    }

    // Get level info first
    const Level& L = getCurrentLevel();
    int pad = 20;
    int cx = x + pad;
    int cy = y + pad;
    int contentW = w - pad * 2;

    // LEVEL-THEMED COLORS (Each level gets unique color scheme)
    int levelThemes[][6] = {
        {80, 180, 120, 120, 220, 180},   // Level 1: Green/Teal (Education/Growth)
        {70, 140, 220, 120, 200, 255},   // Level 2: Blue/Cyan (Sky/Nature)
        {140, 80, 200, 180, 120, 240},   // Level 3: Purple/Magenta (Night/Mystery)
        {220, 160, 80, 255, 220, 120},   // Level 4: Orange/Yellow (Lab/Science)
        {100, 80, 180, 150, 130, 255}    // Level 5: Deep Blue/Violet (Space/Advanced)
    };
    int themeIdx = (L.id - 1) % 5;
    int *theme = levelThemes[themeIdx];

    // Panel border with PULSING glow effect (level-themed colors)
    float borderPulse = std::sin(currentTime * 3.0f) * 0.3f + 0.7f; // 0.4 to 1.0
    Color borderColor((Uint8)(theme[0] * borderPulse),
                     (Uint8)(theme[1] * borderPulse),
                     (Uint8)(theme[2] * borderPulse));

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < 5; ++i) {
        Uint8 alpha = (Uint8)((220 - i * 40) * borderPulse);
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, alpha);
        SDL_Rect border = {x - i, y - i, w + i*2, h + i*2};
        SDL_RenderDrawRect(renderer, &border);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Animated header with level-themed gradient + pulsing glow
    float pulsePhase = std::sin(currentTime * 2.0f) * 0.2f + 0.8f; // 0.6 to 1.0
    for (int i = 0; i < 60; ++i) {
        float t = (float)i / 60.0f;
        Uint8 r = (Uint8)(theme[0] + (theme[3] - theme[0]) * t);
        Uint8 g = (Uint8)(theme[1] + (theme[4] - theme[1]) * t);
        Uint8 b = (Uint8)(theme[2] + (theme[5] - theme[2]) * t);
        // Apply pulse to brightness
        r = (Uint8)(r * pulsePhase);
        g = (Uint8)(g * pulsePhase);
        b = (Uint8)(b * pulsePhase);
        SDL_SetRenderDrawColor(renderer, r, g, b, 200);
        SDL_Rect headerLine = {x, y + i, w, 1};
        SDL_RenderFillRect(renderer, &headerLine);
    }

    // Header text with glow
    drawText(renderer, font, "LEARNING MODE", cx + 82, cy + 10, Color(40, 40, 60, 100));
    drawText(renderer, font, "LEARNING MODE", cx + 80, cy + 8, Color::White());
    cy += 70;

    // Level badge (modern card style)
    SDL_Rect levelCard = {cx, cy, contentW, 45};
    // Card background
    SDL_SetRenderDrawColor(renderer, 240, 245, 255, 255);
    SDL_RenderFillRect(renderer, &levelCard);
    // Card border
    SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
    SDL_RenderDrawRect(renderer, &levelCard);
    // Level number circle
    for (int dy = -18; dy <= 18; ++dy) {
        for (int dx = -18; dx <= 18; ++dx) {
            if (dx*dx + dy*dy <= 18*18) {
                SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
                SDL_RenderDrawPoint(renderer, cx + 22 + dx, cy + 22 + dy);
            }
        }
    }
    drawText(renderer, font, std::to_string(L.id), cx + 16, cy + 14, Color::White());
    // Level title
    drawText(renderer, smallFont, L.title, cx + 50, cy + 8, Color::Black());
    drawText(renderer, smallFont, L.topic, cx + 50, cy + 25, Color(100, 100, 150));
    // Difficulty stars
    renderStarRating(renderer, L.difficulty, 5, cx + contentW - 110, cy + 12, 18);
    cy += 60;

    // Challenge section with modern card
    SDL_Rect challengeCard = {cx, cy, contentW, 120};
    // Gradient background
    for (int i = 0; i < 120; ++i) {
        float t = (float)i / 120.0f;
        Uint8 val = (Uint8)(255 - t * 10);
        SDL_SetRenderDrawColor(renderer, val, 255 - (Uint8)(t*10), 240 - (Uint8)(t*5), 255);
        SDL_Rect line = {cx, cy + i, contentW, 1};
        SDL_RenderFillRect(renderer, &line);
    }
    SDL_SetRenderDrawColor(renderer, 80, 200, 120, 255);
    SDL_RenderDrawRect(renderer, &challengeCard);

    drawText(renderer, smallFont, "CHALLENGE", cx + 8, cy + 8, Color(50, 150, 80));
    cy += 28;
    auto wrappedChallenge = wrapText(L.challenge, contentW - 20, smallFont);
    for (const auto& line : wrappedChallenge) {
        drawText(renderer, smallFont, line, cx + 10, cy, Color(40,40,40));
        cy += 18;
    }
    cy += 138 - (int)wrappedChallenge.size() * 18 + 15;

    // Reflection section
    drawText(renderer, smallFont, "REFLECT:", cx, cy, Color::Purple());
    cy += 20;
    auto wrappedReflection = wrapText(L.reflectionQuestion, contentW, smallFont);
    for (const auto& line : wrappedReflection) {
        drawText(renderer, smallFont, line, cx + 5, cy, Color(100,60,140));
        cy += 18;
    }
    cy += 20;

    // ========== PHYSICS SCENARIO DISPLAY (NEW!) ==========
    const ScenarioSetup& scenario = L.scenarioSetup;
    if (!scenario.entities.empty()) {
        SDL_Rect scenarioCard = {cx, cy, contentW, 90};
        // Gradient background (purple/blue theme for physics)
        for (int i = 0; i < 90; ++i) {
            float t = (float)i / 90.0f;
            Uint8 r = (Uint8)(180 - t * 40);
            Uint8 g = (Uint8)(140 + t * 40);
            Uint8 b = (Uint8)(255 - t * 20);
            SDL_SetRenderDrawColor(renderer, r, g, b, 240);
            SDL_Rect line = {cx, cy + i, contentW, 1};
            SDL_RenderFillRect(renderer, &line);
        }
        // Glowing border
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int i = 0; i < 3; ++i) {
            SDL_SetRenderDrawColor(renderer, 140, 100, 255, (Uint8)(180 - i * 50));
            SDL_Rect border = {scenarioCard.x - i, scenarioCard.y - i,
                             scenarioCard.w + i*2, scenarioCard.h + i*2};
            SDL_RenderDrawRect(renderer, &border);
        }
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        int scy = cy + 8;
        drawText(renderer, smallFont, "PHYSICS SCENARIO CONFIGURED", cx + 8, scy, Color::White());
        scy += 22;

        // Display instruction
        if (!scenario.instruction.empty()) {
            drawText(renderer, smallFont, scenario.instruction, cx + 10, scy, Color(240,240,255));
            scy += 18;
        }

        // Display entity list with icons
        drawText(renderer, smallFont, "Entities:", cx + 10, scy, Color(255,220,180));
        scy += 18;

        int entityX = cx + 20;
        for (size_t i = 0; i < std::min((size_t)4, scenario.entities.size()); ++i) {
            const auto& e = scenario.entities[i];

            // Draw small icon representing entity type
            if (e.type == "ball") {
                DrawFilledCircleLocal(renderer, entityX, scy + 6, 5, e.color);
            } else if (e.type == "block") {
                SDL_Rect block = {entityX - 5, scy + 1, 10, 10};
                SDL_SetRenderDrawColor(renderer, e.color.r, e.color.g, e.color.b, e.color.a);
                SDL_RenderFillRect(renderer, &block);
            } else if (e.type == "ramp") {
                SDL_SetRenderDrawColor(renderer, e.color.r, e.color.g, e.color.b, e.color.a);
                for (int j = 0; j < 10; ++j) {
                    SDL_RenderDrawLine(renderer, entityX - 5, scy + 11 - j,
                                     entityX + 5 - j, scy + 11 - j);
                }
            } else if (e.type == "ground") {
                SDL_Rect ground = {entityX - 8, scy + 8, 16, 4};
                SDL_SetRenderDrawColor(renderer, e.color.r, e.color.g, e.color.b, e.color.a);
                SDL_RenderFillRect(renderer, &ground);
            }

            // Entity label
            std::string labelText = e.type;
            if (!e.label.empty()) labelText = e.label;
            drawText(renderer, smallFont, labelText, entityX + 15, scy, Color::White());

            entityX += 70;
            if (entityX > cx + contentW - 50) {
                entityX = cx + 20;
                scy += 16;
            }
        }

        if (scenario.entities.size() > 4) {
            drawText(renderer, smallFont, "+ " + std::to_string(scenario.entities.size() - 4) + " more",
                    entityX, scy, Color(200,200,255));
        }

        cy += 95;
    }

    // Progress section with card
    SDL_Rect progressCard = {cx, cy, contentW, 120};
    SDL_SetRenderDrawColor(renderer, 248, 250, 255, 255);
    SDL_RenderFillRect(renderer, &progressCard);
    SDL_SetRenderDrawColor(renderer, 150, 180, 255, 255);
    SDL_RenderDrawRect(renderer, &progressCard);

    int pcy = cy + 10;
    drawText(renderer, smallFont, "YOUR PROGRESS", cx + 8, pcy, Color(80, 100, 200));
    pcy += 22;

    // Points display with icon
    SDL_Rect pointsBox = {cx + 8, pcy, contentW - 16, 24};
    SDL_SetRenderDrawColor(renderer, 255, 245, 200, 255);
    SDL_RenderFillRect(renderer, &pointsBox);
    SDL_SetRenderDrawColor(renderer, 220, 180, 100, 255);
    SDL_RenderDrawRect(renderer, &pointsBox);
    std::string pointsText = "Total Points: " + std::to_string(progress.totalPoints);
    drawText(renderer, smallFont, pointsText, cx + 15, pcy + 5, Color(180, 120, 0));
    pcy += 32;

    // ANIMATED Mastery bar
    float m = 0.0f;
    if (progress.mastery.count(L.id)) m = progress.mastery.at(L.id);
    renderAnimatedProgressBar(renderer, cx + 8, pcy, contentW - 16, 28, m, "Mastery", smallFont);
    pcy += 35;

    // Badges with animation
    if (!progress.badges.empty()) {
        drawText(renderer, smallFont, "Badges:", cx + 10, pcy, Color::Orange());
        int badgeX = cx + 80;
        for (size_t i = 0; i < std::min((size_t)4, progress.badges.size()); ++i) {
            // Animated bounce for each badge
            float bouncePhase = currentTime + i * 0.3f;
            int bounceY = (int)(3.0f * std::sin(bouncePhase * 2.0f));
            Achievement fakeBadge{"", progress.badges[i], "", "star", Color::Yellow(), 0, true, 0.0f};
            renderBadge(renderer, fakeBadge, badgeX, pcy + bounceY, 28);
            badgeX += 35;
        }
        if (progress.badges.size() > 4) {
            drawText(renderer, smallFont, "+" + std::to_string(progress.badges.size() - 4), badgeX, pcy + 6, Color::Gray());
        }
    }
    cy += 135;

    // Interactive buttons at bottom
    int buttonY = y + h - 90;
    int buttonW = (contentW - 10) / 2;

    // Hint button (if hints available)
    if (hasAvailableHint(L.id)) {
        renderButton(renderer, "HINT", cx, buttonY, buttonW, 35, Color(255, 180, 60), smallFont, false);
    }

    // Next Level button (if current level mastered)
    if (progress.mastery.count(L.id) && progress.mastery.at(L.id) >= 0.7f && L.id < 5) {
        renderButton(renderer, "NEXT", cx + buttonW + 10, buttonY, buttonW, 35, Color(80, 200, 120), smallFont, false);
    }

    // Controls hint box (more compact)
    SDL_Rect hintBox = {x + pad, y + h - 45, w - pad*2, 38};
    // Gradient background
    for (int i = 0; i < 38; ++i) {
        float t = (float)i / 38.0f;
        Uint8 val = (Uint8)(255 - t * 30);
        SDL_SetRenderDrawColor(renderer, val, val - 10, 200 + (Uint8)(t*20), 220);
        SDL_Rect line = {hintBox.x, hintBox.y + i, hintBox.w, 1};
        SDL_RenderFillRect(renderer, &line);
    }
    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    SDL_RenderDrawRect(renderer, &hintBox);

    drawText(renderer, smallFont, "Press ENTER to answer, then type & submit", hintBox.x + 10, hintBox.y + 6, Color::White());
    drawText(renderer, smallFont, "Press G to exit Learning Mode", hintBox.x + 40, hintBox.y + 22, Color(220,220,255));

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Render feedback modal if showing
    // renderFeedbackModal(renderer, font, smallFont, SCREEN_WIDTH, SCREEN_HEIGHT);
    // Note: We can't get screen dimensions here, so feedback modal would need to be called from main
}
// ============================================================================
// NEW: PROMPT-DRIVEN SYSTEM IMPLEMENTATION
// ============================================================================

// --- Game Mode Management ---

void GameEngine::setGameMode(GameMode mode) {
    currentMode = mode;

    if (mode == GameMode::PROMPT_INPUT) {
        currentPromptInput.clear();
        panelSlideProgress = 0.0f; // Reset animations
    } else if (mode == GameMode::PLAYING) {
        player.active = true;
        player.position = Vector2(100, 400);
        player.velocity = Vector2(0, 0);
        player.onGround = false;
    }
}

void GameEngine::clearScene() {
    scenarioGenerated = false;
    player.active = false;
    particles.clear();
}

// --- Prompt Input Handling ---

void GameEngine::addCharToPrompt(char c) {
    if (currentPromptInput.size() < 200) { // Max 200 characters
        currentPromptInput += c;
    }
}

void GameEngine::removeCharFromPrompt() {
    if (!currentPromptInput.empty()) {
        currentPromptInput.pop_back();
    }
}

void GameEngine::submitPrompt() {
    if (currentPromptInput.empty()) return;

    // Store prompt in context
    promptContext.userPrompt = currentPromptInput;

    // Process the prompt
    processPrompt(currentPromptInput);

    // Generate scenario
    generateScenarioFromPrompt();

    // Switch to playing mode
    setGameMode(GameMode::PLAYING);

    // Spawn celebration particles
    spawnParticles(Vector2(600, 300), 50, Color(100, 255, 200));
}

// --- Prompt Processing (Rule-based for now, AI-ready) ---

void GameEngine::processPrompt(const std::string& prompt) {
    std::string lower = toLower(prompt);

    // Extract physics concepts
    if (lower.find("gravity") != std::string::npos || lower.find("fall") != std::string::npos) {
        promptContext.parsedConcept = "gravity";
        promptContext.keywords.push_back("gravity");
        promptContext.keywords.push_back("acceleration");
    }
    else if (lower.find("collision") != std::string::npos || lower.find("bounce") != std::string::npos) {
        promptContext.parsedConcept = "collision";
        promptContext.keywords.push_back("collision");
        promptContext.keywords.push_back("momentum");
    }
    else if (lower.find("ramp") != std::string::npos || lower.find("slope") != std::string::npos) {
        promptContext.parsedConcept = "inclined_plane";
        promptContext.keywords.push_back("ramp");
        promptContext.keywords.push_back("gravity");
    }
    else if (lower.find("force") != std::string::npos || lower.find("push") != std::string::npos) {
        promptContext.parsedConcept = "forces";
        promptContext.keywords.push_back("force");
        promptContext.keywords.push_back("motion");
    }
    else {
        // Default to basic gravity demo
        promptContext.parsedConcept = "gravity";
        promptContext.keywords.push_back("gravity");
    }

    // Check if user wants to interact
    if (lower.find("control") != std::string::npos ||
        lower.find("move") != std::string::npos ||
        lower.find("player") != std::string::npos) {
        promptContext.requiresInteraction = true;
    }

    // Determine difficulty
    if (lower.find("simple") != std::string::npos || lower.find("basic") != std::string::npos) {
        promptContext.difficulty = "beginner";
    } else if (lower.find("advanced") != std::string::npos || lower.find("complex") != std::string::npos) {
        promptContext.difficulty = "advanced";
    } else {
        promptContext.difficulty = "intermediate";
    }
}

// --- Scenario Generation from Prompt ---

void GameEngine::generateScenarioFromPrompt() {
    scenarioGenerated = true;

    // This is where AI could be integrated!
    // For now, use rule-based generation

    if (promptContext.parsedConcept == "gravity") {
        // Generate gravity demonstration
        // (In real implementation, this would create entities in physics engine)
        // For now, we just mark as generated
    }
    else if (promptContext.parsedConcept == "collision") {
        // Generate collision scenario
    }
    else if (promptContext.parsedConcept == "inclined_plane") {
        // Generate ramp scenario
    }
    else if (promptContext.parsedConcept == "forces") {
        // Generate forces demonstration
    }

    // Example: Could call AI here
    // AIRequest request;
    // request.prompt = promptContext.userPrompt;
    // request.concept = promptContext.parsedConcept;
    // request.difficulty = promptContext.difficulty;
    // AIResponse response = processWithAI(request);
    // Then use response.entities to create physics objects
}

// --- AI Integration Placeholder ---

GameEngine::AIResponse GameEngine::processWithAI(const AIRequest& request) {
    // PLACEHOLDER: This would connect to external AI service
    // For now, return empty response

    // In future, this could:
    // 1. Send request to Flask/FastAPI backend
    // 2. Backend calls OpenAI/Claude API
    // 3. AI generates scenario description
    // 4. Backend converts to entity list
    // 5. Returns AIResponse

    AIResponse response;
    response.instruction = "AI-generated scenario for: " + request.concept;
    response.explanation = "This scenario demonstrates " + request.concept;

    // Example entities (AI would generate these)
    if (request.concept == "gravity") {
        ScenarioSetup::EntitySetup ball;
        ball.type = "ball";
        ball.position = Vector2(300, 100);
        ball.color = Color::Red();
        ball.mass = 1.0f;
        ball.size = 25.0f;
        ball.velocity = Vector2(0, 0);
        ball.label = "Ball";
        response.entities.push_back(ball);

        ScenarioSetup::EntitySetup ground;
        ground.type = "ground";
        ground.position = Vector2(600, 550);
        ground.color = Color::Brown();
        ground.mass = 0.0f;
        ground.size = 1000.0f;
        ground.velocity = Vector2(0, 0);
        ground.label = "";
        response.entities.push_back(ground);
    }

    return response;
}

// ============================================================================
// NEW: PLAYER CHARACTER CONTROL
// ============================================================================

void GameEngine::updatePlayerCharacter(float deltaTime) {
    if (!player.active) return;

    // Apply gravity
    player.velocity.y += 600.0f * deltaTime;

    // Update position
    player.position.x += player.velocity.x * deltaTime;
    player.position.y += player.velocity.y * deltaTime;

    // Simple ground collision (y > 500)
    if (player.position.y + player.size > 500) {
        player.position.y = 500 - player.size;
        player.velocity.y = 0;
        player.onGround = true;
    } else {
        player.onGround = false;
    }

    // Screen boundaries
    if (player.position.x < player.size) {
        player.position.x = player.size;
        player.velocity.x = 0;
    }
    if (player.position.x > 1200 - player.size) {
        player.position.x = 1200 - player.size;
        player.velocity.x = 0;
    }

    // Friction
    player.velocity.x *= 0.9f;
}

void GameEngine::handlePlayerInput(bool left, bool right, bool up, bool down, bool jump) {
    if (!player.active) return;

    (void)up;   // Unused for now
    (void)down; // Unused for now

    // Left/Right movement
    if (left) {
        player.velocity.x = -player.speed;
    }
    if (right) {
        player.velocity.x = player.speed;
    }

    // Jump (only when on ground)
    if (jump && player.onGround) {
        player.velocity.y = -player.jumpForce;
        player.onGround = false;

        // Spawn jump particles
        spawnParticles(player.position, 10, Color(150, 200, 255));
    }
}

void GameEngine::renderPlayerCharacter(SDL_Renderer* r) {
    if (!player.active) return;

    // Draw player as a character (simple for now)
    int px = (int)player.position.x;
    int py = (int)player.position.y;
    int size = (int)player.size;

    // Body (circle)
    for (int dy = -size; dy <= size; ++dy) {
        for (int dx = -size; dx <= size; ++dx) {
            if (dx*dx + dy*dy <= size*size) {
                SDL_SetRenderDrawColor(r, player.color.r, player.color.g, player.color.b, player.color.a);
                SDL_RenderDrawPoint(r, px + dx, py + dy);
            }
        }
    }

    // Eyes
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    SDL_Rect eye1 = {px - size/3, py - size/3, size/4, size/4};
    SDL_Rect eye2 = {px + size/10, py - size/3, size/4, size/4};
    SDL_RenderFillRect(r, &eye1);
    SDL_RenderFillRect(r, &eye2);

    // Pupils
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_Rect pupil1 = {px - size/3 + size/8, py - size/3 + size/12, size/8, size/8};
    SDL_Rect pupil2 = {px + size/10 + size/8, py - size/3 + size/12, size/8, size/8};
    SDL_RenderFillRect(r, &pupil1);
    SDL_RenderFillRect(r, &pupil2);

    // Smile
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    for (int i = -size/2; i < size/2; ++i) {
        int sy = py + size/4 + (int)(3 * std::sin(i * 3.14159f / size));
        SDL_RenderDrawPoint(r, px + i, sy);
    }
}

// ============================================================================
// NEW: BLANK SCREEN AND PROMPT INPUT RENDERING
// ============================================================================

void GameEngine::renderBlankScreen(SDL_Renderer* r, TTF_Font* font, TTF_Font* small, int screenW, int screenH) {
    // Dark gradient background
    for (int y = 0; y < screenH; ++y) {
        float t = (float)y / (float)screenH;
        Uint8 v = (Uint8)(20 + 40*t);
        SDL_SetRenderDrawColor(r, v/2, v/2, v, 255);
        SDL_RenderDrawLine(r, 0, y, screenW, y);
    }

    // Floating particles in background
    renderParticles(r);

    // Title
    int centerX = screenW / 2;
    int centerY = screenH / 2;

    drawText(r, font, "EduSandbox - Interactive Physics Learning", centerX - 350, centerY - 150, Color(200, 200, 255));

    // Pulsing "Press ENTER" prompt
    float pulse = std::sin(currentTime * 3.0f) * 0.3f + 0.7f;
    Color promptColor((Uint8)(255 * pulse), (Uint8)(255 * pulse), (Uint8)(255 * pulse));

    drawText(r, font, "Press ENTER to start...", centerX - 180, centerY, promptColor);

    // Instructions at bottom
    drawText(r, small, "Type what you want to learn about physics", centerX - 200, centerY + 100, Color::LightGray());
    drawText(r, small, "Example: \"Show me gravity\", \"Ball rolling down ramp\", \"Collision between objects\"",
             centerX - 400, centerY + 130, Color::Gray());

    // Controls hint
    drawText(r, small, "Controls: Arrow Keys or WASD to move character, SPACE to jump", centerX - 300, screenH - 50, Color(180, 180, 200));
}

void GameEngine::renderPromptInputScreen(SDL_Renderer* r, TTF_Font* font, TTF_Font* small, int screenW, int screenH) {
    // Dark background
    SDL_SetRenderDrawColor(r, 20, 20, 30, 255);
    SDL_Rect fullScreen = {0, 0, screenW, screenH};
    SDL_RenderFillRect(r, &fullScreen);

    // Particles
    renderParticles(r);

    int centerX = screenW / 2;
    int centerY = screenH / 2;

    // Prompt box
    SDL_Rect promptBox = {centerX - 400, centerY - 100, 800, 200};

    // Glow effect
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    for (int i = 0; i < 10; ++i) {
        Uint8 alpha = (Uint8)(50 - i * 5);
        SDL_SetRenderDrawColor(r, 100, 150, 255, alpha);
        SDL_Rect glow = {promptBox.x - i, promptBox.y - i, promptBox.w + i*2, promptBox.h + i*2};
        SDL_RenderDrawRect(r, &glow);
    }
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);

    // Box background
    SDL_SetRenderDrawColor(r, 40, 40, 60, 255);
    SDL_RenderFillRect(r, &promptBox);

    // Box border
    SDL_SetRenderDrawColor(r, 100, 150, 255, 255);
    SDL_RenderDrawRect(r, &promptBox);

    // Title
    drawText(r, font, "What do you want to learn?", promptBox.x + 20, promptBox.y - 50, Color(200, 220, 255));

    // User input text
    std::string displayText = currentPromptInput;
    if ((int)(currentTime * 2) % 2 == 0) { // Blinking cursor
        displayText += "|";
    }

    drawText(r, font, displayText, promptBox.x + 20, promptBox.y + 30, Color::White());

    // Character count
    std::string countText = std::to_string(currentPromptInput.size()) + " / 200";
    drawText(r, small, countText, promptBox.x + promptBox.w - 80, promptBox.y + promptBox.h - 30, Color::Gray());

    // Instructions
    drawText(r, small, "Press ENTER to generate scenario", centerX - 150, promptBox.y + promptBox.h + 30, Color::LightGray());
    drawText(r, small, "Press ESC to cancel", centerX - 80, promptBox.y + promptBox.h + 55, Color::Gray());

    // Example prompts
    drawText(r, small, "Try: \"gravity\", \"ball on ramp\", \"collision\", \"forces\"", centerX - 200, promptBox.y + promptBox.h + 100, Color(150, 150, 180));
}
