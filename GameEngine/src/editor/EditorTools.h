// =============================================================================
// EditorTools.h  —  Two-Mode ImGui Editor Overlay
// =============================================================================
#pragma once

#include "../core/ecs/Registry.h"
#include "../core/ecs/Component.h"

#include <string>

struct SDL_Window;
struct SDL_Renderer;
union  SDL_Event;

namespace EduEngine {
    class Engine;
    class SceneLoader;
    class PromptSystem;
}

namespace EduEngine {

class EditorTools {
public:
    EditorTools()  = default;
    ~EditorTools() = default;

    // ── Lifecycle ──────────────────────────────────────────────────────────
    void Init(SDL_Window* window, SDL_Renderer* renderer);
    void ProcessEvent(const SDL_Event& event);
    void BeginFrame();
    void EndFrame();
    void Shutdown();

    // ── Top-level render calls (from Engine::Render) ───────────────────────
    void RenderModeBar(Engine& engine);
    void RenderPromptMode(PromptSystem& prompt, Engine& engine);
    void RenderBuildMode(Registry& registry, SceneLoader& loader, Engine& engine);
    void RenderPerformanceHUD(float dt);

private:
    // ── Build-mode panels ──────────────────────────────────────────────────
    void RenderHierarchy(Registry& registry, Engine& engine);
    void RenderInspector(Registry& registry);
    void RenderSceneToolbar(SceneLoader& loader, Engine& engine);
    void RenderExportModal(Engine& engine);
    void DoExportGame(Engine& engine, const std::string& dest_dir);

    // ── Inspector sub-panels ───────────────────────────────────────────────
    void InspectTransform(EntityID id,   Registry& r);
    void InspectSprite(EntityID id,      Registry& r);
    void InspectRigidBody(EntityID id,   Registry& r);
    void InspectCollider(EntityID id,    Registry& r);
    void InspectPlayerTag(EntityID id,   Registry& r);
    void InspectCollectible(EntityID id, Registry& r);
    void InspectTextLabel(EntityID id,   Registry& r);
    void InspectPlatform(EntityID id,    Registry& r);
    void InspectInfoZone(EntityID id,    Registry& r);
    void InspectEnemy(EntityID id,       Registry& r);
    void InspectGoal(EntityID id,        Registry& r);
    void RenderAddComponentMenu(EntityID id, Registry& r);

    // ── State ─────────────────────────────────────────────────────────────
    EntityID    m_selected    = NULL_ENTITY;

    // Prompt-mode buffers.
    char  m_prompt_buf[512]  = {};
    char  m_subject_buf[128] = {};
    int   m_grade            = 6;
    int   m_type_idx         = 0;
    bool  m_mock_mode        = true;
    char  m_api_url_buf[256] = {};
    char  m_api_key_buf[256] = {};
    std::string m_preview_json;

    // Scene toolbar buffer.
    char  m_scene_path_buf[512] = {};

    // Export modal + status toast.
    bool         m_show_export_modal = false;
    char         m_export_dest_buf[512] = {};
    std::string  m_export_msg;
    uint32_t     m_export_msg_until = 0;  // SDL_GetTicks() deadline

    // FPS graph.
    static constexpr int FPS_BUF = 90;
    float m_fps_buf[FPS_BUF] = {};
    int   m_fps_head         = 0;

    SDL_Window*   m_window   = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};

} // namespace EduEngine
