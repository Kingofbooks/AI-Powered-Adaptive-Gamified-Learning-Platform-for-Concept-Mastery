// =============================================================================
// InputSystem.h — Player Input Handling (Keyboard)
// =============================================================================
// Maps keyboard events to forces/velocities on entities tagged as players.
// The system reads from the entity's PlayerTagComponent for speed/jump values,
// making it fully data-driven and tweakable from the editor.
// =============================================================================
#pragma once

#include "../core/ecs/System.h"
#include "../core/ecs/Component.h"
#include "../audio/AudioPlayer.h"

#include <SDL2/SDL.h>

namespace EduEngine {

class InputSystem final : public System {
public:
    // -----------------------------------------------------------------------
    // ProcessEvents — capture key-down/up state for smooth movement.
    // -----------------------------------------------------------------------
    void ProcessEvents(const SDL_Event& event, Registry& /*registry*/) override {
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:  case SDLK_a: m_left  = true; break;
                case SDLK_RIGHT: case SDLK_d: m_right = true; break;
                case SDLK_SPACE: case SDLK_w: case SDLK_UP:
                    m_jump_pressed = true; break;
                default: break;
            }
        }
        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:  case SDLK_a: m_left  = false; break;
                case SDLK_RIGHT: case SDLK_d: m_right = false; break;
                case SDLK_SPACE: case SDLK_w: case SDLK_UP:
                    m_jump_pressed = false; break;
                default: break;
            }
        }
    }

    // -----------------------------------------------------------------------
    // Update — apply horizontal velocity and jump force.
    // -----------------------------------------------------------------------
    void Update(float /*dt*/, Registry& registry) override {
        registry.View<PlayerTagComponent, RigidBodyComponent>(
            [&](EntityID /*id*/, PlayerTagComponent& p, RigidBodyComponent& rb) {
                // ── Horizontal ──────────────────────────────────────────────
                rb.velocity_x = 0.f;
                if (m_left)  rb.velocity_x -= p.move_speed;
                if (m_right) rb.velocity_x += p.move_speed;

                // ── Jump ────────────────────────────────────────────────────
                if (m_jump_pressed && !m_jump_consumed) {
                    if (rb.on_ground || p.current_jumps < p.max_jump_count) {
                        rb.velocity_y   = p.jump_force;
                        p.current_jumps++;
                        m_jump_consumed = true;
                        AudioPlayer::PlayJump();
                    }
                }
                if (!m_jump_pressed) {
                    m_jump_consumed = false;
                }

                // Reset jump counter when grounded.
                if (rb.on_ground) {
                    p.current_jumps = 0;
                }
            });
    }

    [[nodiscard]] const char* Name() const noexcept override { return "InputSystem"; }
    [[nodiscard]] int Priority() const noexcept override { return 0; } // First.

private:
    bool m_left          = false;
    bool m_right         = false;
    bool m_jump_pressed  = false;
    bool m_jump_consumed = false; ///< Prevents held-key multi-jump.
};

} // namespace EduEngine
