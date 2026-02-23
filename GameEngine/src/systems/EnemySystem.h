// =============================================================================
// EnemySystem.h  —  Simple Patrol Enemy Logic
// =============================================================================
#pragma once

#include "../core/ecs/System.h"
#include "../core/ecs/Component.h"

namespace EduEngine {

class EnemySystem final : public System {
public:
    void Update(float dt, Registry& registry) override {
        registry.View<TransformComponent, EnemyComponent>(
            [&](EntityID /*id*/, TransformComponent& t, EnemyComponent& e) {
                t.x += (e.moving_right ? e.speed : -e.speed) * dt;
                if (t.x >= e.patrol_max_x) { e.moving_right = false; }
                if (t.x <= e.patrol_min_x) { e.moving_right = true;  }
                // Flip sprite visually.
                if (registry.HasComponent<SpriteComponent>(
                        registry.GetAllIDs().empty() ? NULL_ENTITY : 0u)) { /* skip */ }
            });
    }
    [[nodiscard]] const char* Name()     const noexcept override { return "EnemySystem"; }
    [[nodiscard]] int         Priority() const noexcept override { return 15; }
};

} // namespace EduEngine
