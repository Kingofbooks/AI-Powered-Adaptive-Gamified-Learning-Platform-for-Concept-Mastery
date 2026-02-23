// =============================================================================
// TriggerSystem.h  —  Overlap Detection (Collectibles, InfoZones, Goal)
// =============================================================================
#pragma once

#include "../core/ecs/System.h"
#include "../core/ecs/Component.h"

#include <SDL2/SDL.h>

namespace EduEngine { class Engine; }

namespace EduEngine {

class TriggerSystem final : public System {
public:
    explicit TriggerSystem(Engine& engine) : m_engine(engine) {}

    void Update(float dt, Registry& registry) override;

    [[nodiscard]] const char* Name()     const noexcept override { return "TriggerSystem"; }
    [[nodiscard]] int         Priority() const noexcept override { return 20; }

    // Read-only score accessor (simple counter for collectibles).
    [[nodiscard]] int GetScore() const noexcept { return m_score; }

private:
    static bool AABBOverlap(float ax,float ay,float aw,float ah,
                             float bx,float by,float bw,float bh) noexcept {
        return ax < bx+bw && ax+aw > bx && ay < by+bh && ay+ah > by;
    }

    Engine& m_engine;
    int     m_score = 0;
};

} // namespace EduEngine
