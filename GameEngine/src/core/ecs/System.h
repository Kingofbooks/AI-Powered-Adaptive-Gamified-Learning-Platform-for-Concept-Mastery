// =============================================================================
// System.h — Abstract Base Class for All ECS Systems
// =============================================================================
// A System encapsulates pure logic that acts on component data stored in the
// Registry.  Systems have NO state of their own that isn't explicitly needed
// for their computation.
//
// Lifecycle called by the Engine each frame:
//   1. ProcessEvents(SDL_Event&) — optional input handling.
//   2. Update(dt, registry)     — simulation / physics / game logic.
//   3. Render(renderer, registry) — SDL draw calls.
// =============================================================================
#pragma once

#include "Registry.h"
#include <SDL2/SDL.h>

namespace EduEngine {

class System {
public:
    virtual ~System() = default;

    // -----------------------------------------------------------------------
    // ProcessEvents — called once per SDL event pulled from the queue.
    // Override only if the system cares about raw input events.
    // -----------------------------------------------------------------------
    virtual void ProcessEvents([[maybe_unused]] const SDL_Event& event,
                               [[maybe_unused]] Registry& registry) {}

    // -----------------------------------------------------------------------
    // Update — advance simulation by delta_time seconds.
    // delta_time is capped at 0.05 s by the Engine to prevent tunnelling.
    // -----------------------------------------------------------------------
    virtual void Update(float delta_time, Registry& registry) = 0;

    // -----------------------------------------------------------------------
    // Render — emit SDL draw calls.  The Engine calls this AFTER all Updates.
    // -----------------------------------------------------------------------
    virtual void Render([[maybe_unused]] SDL_Renderer* renderer,
                        [[maybe_unused]] Registry& registry) {}

    // -----------------------------------------------------------------------
    // Priority — systems with a lower value run first.  Override to reorder.
    // -----------------------------------------------------------------------
    [[nodiscard]] virtual int Priority() const noexcept { return 0; }

    // -----------------------------------------------------------------------
    // Name — human-readable identifier shown in the editor overlay.
    // -----------------------------------------------------------------------
    [[nodiscard]] virtual const char* Name() const noexcept = 0;
};

} // namespace EduEngine
