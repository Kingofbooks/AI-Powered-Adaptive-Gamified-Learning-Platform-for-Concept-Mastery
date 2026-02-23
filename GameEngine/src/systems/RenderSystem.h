// =============================================================================
// RenderSystem.h — SDL2 Sprite & Text Rendering System
// =============================================================================
// Renders all entities that own a TransformComponent + SpriteComponent,
// sorted by layer value (lowest first).  Also renders TextLabelComponents.
//
// Asset loading strategy:
//   Textures are loaded on-demand and cached in an internal map keyed by
//   texture_path.  The cache is cleared on scene reload.
// =============================================================================
#pragma once

#include "../core/ecs/System.h"
#include "../core/ecs/Component.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <unordered_map>
#include <string>
#include <vector>

namespace EduEngine {

class RenderSystem final : public System {
public:
    explicit RenderSystem(SDL_Renderer* renderer) : m_renderer(renderer) {}

    ~RenderSystem() override {
        // Free all cached textures.
        for (auto& [path, tex] : m_texture_cache) {
            if (tex) SDL_DestroyTexture(tex);
        }
        if (m_default_font) TTF_CloseFont(m_default_font);
    }

    // -----------------------------------------------------------------------
    // Load the default font used for TextLabelComponents.
    // Call once after Init().
    // -----------------------------------------------------------------------
    void SetDefaultFont(const std::string& font_path, int pt_size = 16) {
        if (m_default_font) TTF_CloseFont(m_default_font);
        m_default_font = TTF_OpenFont(font_path.c_str(), pt_size);
        if (!m_default_font) {
            SDL_Log("[RenderSystem] Could not load font '%s': %s",
                    font_path.c_str(), TTF_GetError());
        }
        m_default_font_path = font_path;
        m_default_font_size = pt_size;
    }

    // -----------------------------------------------------------------------
    // Clear the texture cache (call on scene reload / hot-reload).
    // -----------------------------------------------------------------------
    void ClearCache() {
        for (auto& [path, tex] : m_texture_cache) {
            if (tex) SDL_DestroyTexture(tex);
        }
        m_texture_cache.clear();
    }

    // -----------------------------------------------------------------------
    // System interface
    // -----------------------------------------------------------------------

    void Update([[maybe_unused]] float dt,
                [[maybe_unused]] Registry& registry) override {
        // RenderSystem has no update logic — it only draws.
    }

    void Render(SDL_Renderer* renderer, Registry& registry) override {
        // ── Collect renderables sorted by layer ──────────────────────────
        struct RenderItem {
            EntityID id;
            int      layer;
        };
        std::vector<RenderItem> items;
        items.reserve(64);

        registry.View<TransformComponent, SpriteComponent>(
            [&](EntityID id, TransformComponent&, SpriteComponent& s) {
                if (s.visible) items.push_back({id, s.layer});
            });

        std::stable_sort(items.begin(), items.end(),
            [](const RenderItem& a, const RenderItem& b){ return a.layer < b.layer; });

        // ── Draw sprites ─────────────────────────────────────────────────
        for (const auto& item : items) {
            const auto& t = registry.GetComponent<TransformComponent>(item.id);
            auto&       s = registry.GetComponent<SpriteComponent>(item.id);

            if (!s.texture_path.empty()) {
                // Load texture on first use.
                if (!s.texture) {
                    s.texture = LoadTexture(s.texture_path);
                }
                if (s.texture) {
                    // Query native size.
                    int w, h;
                    SDL_QueryTexture(s.texture, nullptr, nullptr, &w, &h);
                    if (s.frame_w > 0) w = s.frame_w;
                    if (s.frame_h > 0) h = s.frame_h;

                    SDL_Rect src{s.frame_x, s.frame_y, w, h};
                    SDL_FRect dst{
                        t.x, t.y,
                        w * t.scale_x,
                        h * t.scale_y
                    };
                    SDL_SetTextureColorMod(s.texture, s.color.r, s.color.g, s.color.b);
                    SDL_SetTextureAlphaMod(s.texture, s.color.a);

                    const SDL_RendererFlip flip = s.flip_h
                        ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
                    SDL_RenderCopyExF(renderer, s.texture, &src, &dst,
                                     t.rotation, nullptr, flip);
                }
            } else {
                // No texture — draw a solid coloured rectangle.
                SDL_SetRenderDrawColor(renderer,
                    s.color.r, s.color.g, s.color.b, s.color.a);
                // Use collider size if available, else 32×32.
                float w = 32.f, h = 32.f;
                if (registry.HasComponent<ColliderComponent>(item.id)) {
                    const auto& c = registry.GetComponent<ColliderComponent>(item.id);
                    w = c.width; h = c.height;
                }
                SDL_FRect rect{t.x, t.y, w * t.scale_x, h * t.scale_y};
                SDL_RenderFillRectF(renderer, &rect);
            }
        }

        // ── Draw text labels ──────────────────────────────────────────────
        registry.View<TransformComponent, TextLabelComponent>(
            [&](EntityID id, const TransformComponent& t, const TextLabelComponent& lbl) {
                if (lbl.text.empty()) return;
                RenderText(renderer, lbl.text, t.x, t.y, lbl.color, lbl.font_size);
            });
    }

    [[nodiscard]] const char* Name() const noexcept override {
        return "RenderSystem";
    }
    [[nodiscard]] int Priority() const noexcept override { return 100; } // Run last.

private:
    SDL_Texture* LoadTexture(const std::string& path) {
        auto it = m_texture_cache.find(path);
        if (it != m_texture_cache.end()) return it->second;

        SDL_Surface* surf = IMG_Load(path.c_str()); // Supports BMP, PNG, JPG, etc.
        if (!surf) {
            SDL_Log("[RenderSystem] Failed to load '%s': %s", path.c_str(), SDL_GetError());
            m_texture_cache[path] = nullptr;
            return nullptr;
        }
        SDL_Texture* tex = SDL_CreateTextureFromSurface(m_renderer, surf);
        SDL_FreeSurface(surf);
        m_texture_cache[path] = tex;
        return tex;
    }

    void RenderText(SDL_Renderer* renderer, const std::string& text,
                    float x, float y, SDL_Color color, int /*pt_size*/) {
        if (!m_default_font) return;

        SDL_Surface* surf = TTF_RenderUTF8_Blended(m_default_font, text.c_str(), color);
        if (!surf) return;

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_FreeSurface(surf);
        if (!tex) return;

        int w, h;
        SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
        const SDL_FRect dst{x, y, static_cast<float>(w), static_cast<float>(h)};
        SDL_RenderCopyF(renderer, tex, nullptr, &dst);
        SDL_DestroyTexture(tex); // Text textures are not cached (dynamic content).
    }

    SDL_Renderer* m_renderer   = nullptr;
    TTF_Font*     m_default_font = nullptr;
    std::string   m_default_font_path;
    int           m_default_font_size = 16;

    std::unordered_map<std::string, SDL_Texture*> m_texture_cache;
};

} // namespace EduEngine
