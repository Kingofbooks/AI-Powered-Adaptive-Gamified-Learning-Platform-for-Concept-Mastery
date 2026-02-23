// =============================================================================
// Engine.h  —  Main Engine Class
// =============================================================================
#pragma once

#include "ecs/Registry.h"
#include "ecs/System.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <memory>
#include <string>
#include <vector>

// Forward declarations — full types included in Engine.cpp
namespace EduEngine {
    class SceneLoader;
    class EditorTools;
    class PromptSystem;
}

namespace EduEngine {

// ---------------------------------------------------------------------------
// EngineMode  —  toggled via the top menu bar in the editor overlay.
// ---------------------------------------------------------------------------
enum class EngineMode {
    Prompt,  ///< Full-screen AI generation UI
    Build,   ///< Manual entity / component editor + live game view
    Play     ///< Game running, editor hidden
};

// ---------------------------------------------------------------------------
// EngineConfig
// ---------------------------------------------------------------------------
struct EngineConfig {
    std::string window_title  = "EduEngine";
    int         window_width  = 1280;
    int         window_height = 720;
    bool        vsync         = true;
    std::string default_scene = "data/scenes/last_scene.json";
    bool        start_in_prompt_mode = true;
    bool        start_in_play_mode   = false;  ///< Skip editor, go straight to Play
};

// ---------------------------------------------------------------------------
// Engine
// ---------------------------------------------------------------------------
class Engine {
public:
    Engine();
    ~Engine();   // defined in Engine.cpp so unique_ptr<ForwardDecl> compiles

    Engine(const Engine&)            = delete;
    Engine& operator=(const Engine&) = delete;

    // ── Lifecycle ──────────────────────────────────────────────────────────
    bool Init(const EngineConfig& config = {});
    void Run();
    void Shutdown();

    // ── System registration ────────────────────────────────────────────────
    void RegisterSystem(std::unique_ptr<System> system);

    // ── Scene management ───────────────────────────────────────────────────
    void LoadScene(const std::string& json_path);

    // ── Mode switching ─────────────────────────────────────────────────────
    void SetMode(EngineMode mode) noexcept { m_mode = mode; }
    [[nodiscard]] EngineMode GetMode() const noexcept { return m_mode; }

    // ── Info-zone popup (set by TriggerSystem, rendered over the game) ─────
    void ShowInfoPopup(const std::string& text, float duration = 4.f) {
        m_popup_text  = text;
        m_popup_timer = duration;
    }

    // ── Accessors ──────────────────────────────────────────────────────────
    [[nodiscard]] Registry&           GetRegistry()     noexcept { return m_registry; }
    [[nodiscard]] SceneLoader&        GetSceneLoader()  noexcept { return *m_scene_loader; }
    [[nodiscard]] PromptSystem&       GetPromptSystem() noexcept { return *m_prompt; }
    [[nodiscard]] SDL_Renderer*       GetRenderer()     noexcept { return m_renderer; }
    [[nodiscard]] SDL_Window*         GetWindow()       noexcept { return m_window; }
    [[nodiscard]] const EngineConfig& GetConfig()       const noexcept { return m_config; }
    [[nodiscard]] const std::string&  GetScenePath()    const noexcept { return m_current_scene_path; }

    void RequestQuit() noexcept { m_running = false; }

private:
    void ProcessEvents();
    void Update(float dt);
    void Render(float dt);
    void RenderInfoPopup(SDL_Renderer* r);

    SDL_Window*   m_window   = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    TTF_Font*     m_hud_font = nullptr; // used for the info popup

    Registry m_registry;
    std::vector<std::unique_ptr<System>> m_systems;

    std::unique_ptr<SceneLoader>  m_scene_loader;
    std::unique_ptr<EditorTools>  m_editor;
    std::unique_ptr<PromptSystem> m_prompt;

    bool         m_running = false;
    float        m_dt      = 0.f;
    EngineMode   m_mode    = EngineMode::Prompt;
    EngineConfig m_config;
    std::string  m_current_scene_path;

    std::string  m_popup_text;
    float        m_popup_timer = 0.f;
};

} // namespace EduEngine
