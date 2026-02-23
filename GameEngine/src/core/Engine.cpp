// =============================================================================
// Engine.cpp
// =============================================================================
#include "Engine.h"

#include "../scene/SceneLoader.h"
#include "../editor/EditorTools.h"
#include "../prompt/PromptSystem.h"
#include "../audio/AudioPlayer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <algorithm>
#include <iostream>

namespace EduEngine {

Engine::Engine()  = default;
Engine::~Engine() = default;

// =============================================================================
bool Engine::Init(const EngineConfig& config) {
    m_config = config;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "[Engine] SDL_Init failed: " << SDL_GetError() << '\n';
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG)))
        std::cerr << "[Engine] IMG_Init warning: " << IMG_GetError() << '\n';
    if (TTF_Init() != 0) {
        std::cerr << "[Engine] TTF_Init failed: " << TTF_GetError() << '\n';
        return false;
    }

    m_window = SDL_CreateWindow(
        m_config.window_title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_config.window_width, m_config.window_height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!m_window) {
        std::cerr << "[Engine] Window creation failed: " << SDL_GetError() << '\n';
        return false;
    }

    const uint32_t rf = SDL_RENDERER_ACCELERATED
                      | (m_config.vsync ? SDL_RENDERER_PRESENTVSYNC : 0);
    m_renderer = SDL_CreateRenderer(m_window, -1, rf);
    if (!m_renderer) {
        std::cerr << "[Engine] Renderer creation failed: " << SDL_GetError() << '\n';
        return false;
    }
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    // HUD font for info-zone popups (non-fatal if file not found).
    m_hud_font = TTF_OpenFont("assets/fonts/OpenSans-Regular.ttf", 20);

    // Sub-systems.
    m_scene_loader = std::make_unique<SceneLoader>();
    m_prompt       = std::make_unique<PromptSystem>();
    m_editor       = std::make_unique<EditorTools>();
    m_editor->Init(m_window, m_renderer);

    AudioPlayer::Init();

    m_mode = m_config.start_in_prompt_mode ? EngineMode::Prompt
           : m_config.start_in_play_mode   ? EngineMode::Play
           :                                 EngineMode::Build;

    // Try loading a previous session scene (non-fatal).
    try {
        m_scene_loader->LoadScene(m_config.default_scene, m_registry);
        m_current_scene_path = m_config.default_scene;
    } catch (...) { /* start with empty canvas */ }

    std::sort(m_systems.begin(), m_systems.end(),
        [](const auto& a, const auto& b){ return a->Priority() < b->Priority(); });

    m_running = true;
    std::cout << "[Engine] Ready. Mode: "
              << (m_mode == EngineMode::Prompt ? "Prompt" : "Build") << '\n';
    return true;
}

// =============================================================================
void Engine::Run() {
    uint64_t last = SDL_GetPerformanceCounter();
    const uint64_t freq = SDL_GetPerformanceFrequency();

    while (m_running) {
        const uint64_t now = SDL_GetPerformanceCounter();
        m_dt = std::min(static_cast<float>(now - last) / static_cast<float>(freq), 0.05f);
        last = now;

        ProcessEvents();
        if (m_mode != EngineMode::Prompt) Update(m_dt);
        Render(m_dt);
    }
}

// =============================================================================
void Engine::Shutdown() {
    AudioPlayer::Quit();
    if (m_editor)   m_editor->Shutdown();
    if (m_hud_font) { TTF_CloseFont(m_hud_font); m_hud_font = nullptr; }
    if (m_renderer) { SDL_DestroyRenderer(m_renderer); m_renderer = nullptr; }
    if (m_window)   { SDL_DestroyWindow(m_window);     m_window   = nullptr; }
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    std::cout << "[Engine] Shutdown.\n";
}

// =============================================================================
void Engine::LoadScene(const std::string& json_path) {
    m_registry.DestroyAllEntities();
    m_scene_loader->LoadScene(json_path, m_registry);
    m_current_scene_path = json_path;
}

// =============================================================================
void Engine::RegisterSystem(std::unique_ptr<System> system) {
    std::cout << "[Engine] System registered: " << system->Name() << '\n';
    m_systems.push_back(std::move(system));
}

// =============================================================================
void Engine::ProcessEvents() {
    SDL_Event ev{};
    while (SDL_PollEvent(&ev)) {
        m_editor->ProcessEvent(ev);
        if (m_mode != EngineMode::Prompt)
            for (auto& s : m_systems) s->ProcessEvents(ev, m_registry);

        if (ev.type == SDL_QUIT) m_running = false;
        if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_F5
            && !m_current_scene_path.empty())
            LoadScene(m_current_scene_path);
    }
}

// =============================================================================
void Engine::Update(float dt) {
    for (auto& s : m_systems) s->Update(dt, m_registry);
    if (m_popup_timer > 0.f) m_popup_timer -= dt;
}

// =============================================================================
void Engine::Render(float dt) {
    SDL_SetRenderDrawColor(m_renderer, 15, 15, 25, 255);
    SDL_RenderClear(m_renderer);

    if (m_mode != EngineMode::Prompt) {
        for (auto& s : m_systems) s->Render(m_renderer, m_registry);
        if (m_popup_timer > 0.f) RenderInfoPopup(m_renderer);
    }

    // ImGui overlay — always on top.
    m_editor->BeginFrame();
    m_editor->RenderModeBar(*this);

    if (m_mode == EngineMode::Prompt) {
        m_editor->RenderPromptMode(*m_prompt, *this);
    } else {
        m_editor->RenderBuildMode(m_registry, *m_scene_loader, *this);
        m_editor->RenderPerformanceHUD(dt);
    }

    m_editor->EndFrame();
    SDL_RenderPresent(m_renderer);
}

// =============================================================================
void Engine::RenderInfoPopup(SDL_Renderer* r) {
    if (!m_hud_font || m_popup_text.empty()) return;

    SDL_SetRenderDrawColor(r, 0, 0, 0, 175);
    SDL_Rect bar{0, m_config.window_height - 140, m_config.window_width, 140};
    SDL_RenderFillRect(r, &bar);

    SDL_Color col{255, 245, 200, 255};
    SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(
        m_hud_font, m_popup_text.c_str(), col,
        static_cast<uint32_t>(m_config.window_width - 40));
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_FreeSurface(surf);
    if (!tex) return;
    int w, h;
    SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
    SDL_Rect dst{20, m_config.window_height - 130, w, h};
    SDL_RenderCopy(r, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

} // namespace EduEngine
