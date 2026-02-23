// =============================================================================
// Component.h  —  All ECS Component Definitions
// =============================================================================
// Components are PURE DATA.  No logic, no virtual functions.
// Systems operate on them; entities merely own collections of them.
// =============================================================================
#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <SDL2/SDL.h>

namespace EduEngine {

// ─────────────────────────────────────────────────────────────────────────────
//  SPATIAL / PHYSICS
// ─────────────────────────────────────────────────────────────────────────────

struct TransformComponent {
    float x        = 0.f;
    float y        = 0.f;
    float scale_x  = 1.f;
    float scale_y  = 1.f;
    float rotation = 0.f;
};

struct RigidBodyComponent {
    float velocity_x    = 0.f;
    float velocity_y    = 0.f;
    float mass          = 1.f;
    float gravity_scale = 1.f;
    bool  is_kinematic  = false;
    bool  on_ground     = false;
};

struct ColliderComponent {
    float offset_x   = 0.f;
    float offset_y   = 0.f;
    float width      = 32.f;
    float height     = 32.f;
    bool  is_trigger = false;
};

// ─────────────────────────────────────────────────────────────────────────────
//  RENDERING
// ─────────────────────────────────────────────────────────────────────────────

struct SpriteComponent {
    std::string   texture_path;
    SDL_Color     color{255, 255, 255, 255};
    int           layer      = 0;
    int           frame_x    = 0;
    int           frame_y    = 0;
    int           frame_w    = 0;  // 0 = full texture width
    int           frame_h    = 0;
    bool          visible    = true;
    bool          flip_h     = false;
    SDL_Texture*  texture    = nullptr; // runtime, filled by RenderSystem
};

struct TextLabelComponent {
    std::string text;
    SDL_Color   color{255, 255, 0, 255};
    int         font_size = 16;
    int         layer     = 10;
};

// ─────────────────────────────────────────────────────────────────────────────
//  GAMEPLAY
// ─────────────────────────────────────────────────────────────────────────────

/// Marks the player-controlled entity.
struct PlayerTagComponent {
    float move_speed     = 220.f;
    float jump_force     = -580.f;
    int   max_jump_count = 2;
    int   current_jumps  = 0;
};

/// Coin/token the player walks over.
struct CollectibleComponent {
    int         point_value = 10;
    std::string label;       // floating text shown on pickup
    bool        collected   = false;
};

/// Static collidable surface (floor, wall, platform).
struct PlatformComponent {
    bool is_one_way = false;
};

/// Marks the level-end goal zone.
struct GoalComponent {
    bool reached = false;
};

/// Info sign / NPC: shows a text popup when the player touches it.
struct InfoZoneComponent {
    std::string text;
    float       show_timer = 0.f;  // seconds remaining (set by TriggerSystem)
    bool        repeat     = false;
    bool        triggered  = false;
};

/// Simple patrol enemy.
struct EnemyComponent {
    float patrol_min_x = 0.f;
    float patrol_max_x = 400.f;
    float speed        = 80.f;
    bool  moving_right = true;
};

} // namespace EduEngine
