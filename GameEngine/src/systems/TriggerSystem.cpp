// =============================================================================
// TriggerSystem.cpp
// =============================================================================
#include "TriggerSystem.h"
#include "../core/Engine.h"
#include "../audio/AudioPlayer.h"

namespace EduEngine {

void TriggerSystem::Update(float /*dt*/, Registry& registry) {
    // ── Find player ──────────────────────────────────────────────────────
    EntityID player_id = NULL_ENTITY;
    registry.View<TransformComponent, PlayerTagComponent, ColliderComponent>(
        [&](EntityID id, TransformComponent&, PlayerTagComponent&, ColliderComponent&) {
            player_id = id;
        });
    if (player_id == NULL_ENTITY) return;

    const auto& pt = registry.GetComponent<TransformComponent>(player_id);
    const auto& pc = registry.GetComponent<ColliderComponent>(player_id);
    const float px = pt.x + pc.offset_x, py = pt.y + pc.offset_y;
    const float pw = pc.width,            ph = pc.height;

    // ── Collectibles ──────────────────────────────────────────────────────
    registry.View<TransformComponent, ColliderComponent, CollectibleComponent>(
        [&](EntityID id, const TransformComponent& t, const ColliderComponent& c,
            CollectibleComponent& col) {
            if (col.collected) return;
            if (AABBOverlap(px,py,pw,ph, t.x+c.offset_x, t.y+c.offset_y, c.width, c.height)) {
                col.collected = true;
                m_score += col.point_value;
                if (registry.HasComponent<SpriteComponent>(id))
                    registry.GetComponent<SpriteComponent>(id).visible = false;
                AudioPlayer::PlayCollect();
                SDL_Log("[Trigger] Collected: %s (+%d)", col.label.c_str(), col.point_value);
            }
        });

    // ── Info zones ────────────────────────────────────────────────────────
    registry.View<TransformComponent, ColliderComponent, InfoZoneComponent>(
        [&](EntityID /*id*/, const TransformComponent& t, const ColliderComponent& c,
            InfoZoneComponent& z) {
            if (z.triggered && !z.repeat) return;
            if (AABBOverlap(px,py,pw,ph, t.x+c.offset_x, t.y+c.offset_y, c.width, c.height)) {
                if (!z.triggered || z.repeat) {
                    z.triggered  = true;
                    z.show_timer = 5.f;
                    m_engine.ShowInfoPopup(z.text, 5.f);
                    SDL_Log("[Trigger] InfoZone: %s", z.text.c_str());
                }
            }
        });

    // ── Goal ──────────────────────────────────────────────────────────────
    registry.View<TransformComponent, ColliderComponent, GoalComponent>(
        [&](EntityID /*id*/, const TransformComponent& t, const ColliderComponent& c,
            GoalComponent& g) {
            if (g.reached) return;
            if (AABBOverlap(px,py,pw,ph, t.x+c.offset_x, t.y+c.offset_y, c.width, c.height)) {
                g.reached = true;
                m_engine.ShowInfoPopup("Level Complete! Great job!", 4.f);
                AudioPlayer::PlayGoal();
                SDL_Log("[Trigger] GOAL reached! Score: %d", m_score);
            }
        });
}

} // namespace EduEngine
