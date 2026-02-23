// =============================================================================
// PhysicsSystem.h — Simple 2-D AABB Platformer Physics
// =============================================================================
// Simulates gravity and AABB vs AABB collision response for:
//   • Entities with RigidBodyComponent + TransformComponent.
//   • Against static platforms (PlatformComponent + TransformComponent +
//     ColliderComponent).
//
// This is intentionally minimal.  A full physics engine (Box2D) can be
// swapped in by replacing this system without touching any other code.
// =============================================================================
#pragma once

#include "../core/ecs/System.h"
#include "../core/ecs/Component.h"

namespace EduEngine {

class PhysicsSystem final : public System {
public:
    explicit PhysicsSystem(float gravity = 980.f) : m_gravity(gravity) {}

    // -----------------------------------------------------------------------
    void Update(float dt, Registry& registry) override {
        // ── 1. Apply gravity & integrate velocities ────────────────────────
        registry.View<TransformComponent, RigidBodyComponent>(
            [&](EntityID /*id*/, TransformComponent& t, RigidBodyComponent& rb) {
                if (rb.is_kinematic) return;

                // Apply gravity.
                rb.velocity_y += m_gravity * rb.gravity_scale * dt;

                // Integrate.
                t.x += rb.velocity_x * dt;
                t.y += rb.velocity_y * dt;

                // Mark as airborne by default; PhysicsSystem sets on_ground.
                rb.on_ground = false;
            });

        // ── 2. AABB platform collision ─────────────────────────────────────
        // Collect all static platforms.
        struct Platform {
            float x, y, w, h;
            bool one_way;
        };
        std::vector<Platform> platforms;

        registry.View<TransformComponent, ColliderComponent, PlatformComponent>(
            [&](EntityID /*id*/, const TransformComponent& t,
                const ColliderComponent& c, const PlatformComponent& p) {
                platforms.push_back({
                    t.x + c.offset_x, t.y + c.offset_y, c.width, c.height, p.is_one_way});
            });

        // Resolve each dynamic body against all platforms.
        registry.View<TransformComponent, RigidBodyComponent, ColliderComponent>(
            [&](EntityID /*id*/, TransformComponent& t, RigidBodyComponent& rb,
                const ColliderComponent& c) {
                if (rb.is_kinematic) return;

                const float bx = t.x + c.offset_x;
                const float by = t.y + c.offset_y;
                const float bw = c.width;
                const float bh = c.height;

                for (const auto& plat : platforms) {
                    if (!AABBOverlap(bx, by, bw, bh, plat.x, plat.y, plat.w, plat.h))
                        continue;

                    // Depth of penetration along each axis.
                    const float overlap_y_top    = (by + bh) - plat.y;
                    const float overlap_y_bottom = (plat.y + plat.h) - by;
                    const float overlap_x_left   = (bx + bw) - plat.x;
                    const float overlap_x_right  = (plat.x + plat.w) - bx;

                    // Find the shallowest penetration axis.
                    const float min_overlap_y = std::min(overlap_y_top, overlap_y_bottom);
                    const float min_overlap_x = std::min(overlap_x_left, overlap_x_right);

                    if (min_overlap_y <= min_overlap_x) {
                        // Vertical resolution.
                        if (overlap_y_top < overlap_y_bottom) {
                            // Landing on top of platform.
                            if (!plat.one_way || rb.velocity_y >= 0.f) {
                                t.y     -= overlap_y_top;
                                rb.velocity_y = 0.f;
                                rb.on_ground  = true;
                            }
                        } else {
                            // Hitting underside.
                            if (!plat.one_way) {
                                t.y          += overlap_y_bottom;
                                rb.velocity_y = 0.f;
                            }
                        }
                    } else {
                        // Horizontal resolution (walls).
                        if (!plat.one_way) {
                            if (overlap_x_left < overlap_x_right) {
                                t.x -= overlap_x_left;
                            } else {
                                t.x += overlap_x_right;
                            }
                            rb.velocity_x = 0.f;
                        }
                    }
                }
            });
    }

    // -----------------------------------------------------------------------
    void SetGravity(float g) noexcept { m_gravity = g; }
    [[nodiscard]] float GetGravity() const noexcept { return m_gravity; }

    [[nodiscard]] const char* Name() const noexcept override { return "PhysicsSystem"; }
    [[nodiscard]] int Priority() const noexcept override { return 10; }

private:
    static bool AABBOverlap(float ax, float ay, float aw, float ah,
                             float bx, float by, float bw, float bh) noexcept {
        return ax < bx + bw && ax + aw > bx
            && ay < by + bh && ay + ah > by;
    }

    float m_gravity;
};

} // namespace EduEngine
