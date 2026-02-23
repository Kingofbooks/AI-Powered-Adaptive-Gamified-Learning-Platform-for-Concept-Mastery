// =============================================================================
// main.cpp  —  EduEngine Entry Point
// =============================================================================
//
// Workflow:
//   1. Launch → opens in PROMPT MODE.
//   2. Type a description, hit "Generate Game".
//   3. Engine builds a playable scene and switches to BUILD MODE automatically.
//   4. In Build Mode: edit entities, tweak components, save/load JSON.
//   5. Switch to PLAY to run the game without editor UI.
//   6. F5 reloads the current scene file at any time.
//
// Controls (Play / Build):
//   A/D or ←/→  — Move player
//   Space/W/↑   — Jump (double-jump)
//   F5          — Hot-reload scene
// =============================================================================

#include "core/Engine.h"
#include "scene/SceneLoader.h"
#include "prompt/PromptSystem.h"

#include "systems/InputSystem.h"
#include "systems/PhysicsSystem.h"
#include "systems/EnemySystem.h"
#include "systems/TriggerSystem.h"
#include "systems/RenderSystem.h"

#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "EduEngine — AI-Powered Educational Game Engine\n"
                 "  Prompt Mode:  type a description → AI generates a game\n"
                 "  Build Mode:   edit entities and components visually\n"
                 "  Play Mode:    run the game fullscreen\n\n";

    // ── Engine config ─────────────────────────────────────────────────────
    EduEngine::EngineConfig config;
    config.window_title          = "EduEngine";
    config.window_width          = 1280;
    config.window_height         = 720;
    config.vsync                 = true;
    config.default_scene         = "data/scenes/last_scene.json";
    config.start_in_prompt_mode  = true;

    // If a scene path is passed as an argument (e.g. from an exported game's
    // run.sh), load that scene directly and start in Play Mode (no editor).
    if (argc >= 2) {
        config.default_scene        = argv[1];
        config.start_in_prompt_mode = false;
        config.start_in_play_mode   = true;
    }

    EduEngine::Engine engine;
    if (!engine.Init(config)) return 1;

    // ── Register systems ──────────────────────────────────────────────────
    // Priority order: Input(0) → Enemy(15) → Physics(10 — runs before enemy)
    //   → Trigger(20) → Render(100)
    engine.RegisterSystem(std::make_unique<EduEngine::InputSystem>());

    {
        float gravity = engine.GetSceneLoader().GetWorld().gravity;
        engine.RegisterSystem(std::make_unique<EduEngine::PhysicsSystem>(gravity));
    }

    engine.RegisterSystem(std::make_unique<EduEngine::EnemySystem>());
    engine.RegisterSystem(std::make_unique<EduEngine::TriggerSystem>(engine));

    {
        auto render = std::make_unique<EduEngine::RenderSystem>(engine.GetRenderer());
        render->SetDefaultFont("assets/fonts/OpenSans-Regular.ttf", 16);
        engine.RegisterSystem(std::move(render));
    }

    // ── Run ───────────────────────────────────────────────────────────────
    engine.Run();
    engine.Shutdown();
    return 0;
}
