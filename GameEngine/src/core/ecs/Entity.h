// =============================================================================
// Entity.h — ECS Entity Definition
// =============================================================================
// An Entity is nothing more than a unique numeric identifier (uint32_t).
// The ECS pattern deliberately keeps data (Components) and logic (Systems)
// separate from the entity identity, enabling extreme composability.
//
// Design note: We embed a 'generation' counter alongside the raw ID so that
// a recycled slot can never be mistakenly treated as a previously destroyed
// entity.  A handle is only valid when BOTH the id AND the generation match
// what the Registry stored at creation time.
// =============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace EduEngine {

// ---------------------------------------------------------------------------
// EntityID — raw index into the component pools.
// ---------------------------------------------------------------------------
using EntityID = uint32_t;

/// Sentinel value: no entity / null reference.
inline constexpr EntityID NULL_ENTITY = 0u;

// ---------------------------------------------------------------------------
// Entity — a lightweight, copyable handle returned to game code.
// Comparing two Entity handles checks BOTH id and generation so stale
// references (pointing to a recycled slot) are automatically detected.
// ---------------------------------------------------------------------------
struct Entity {
    EntityID  id         = NULL_ENTITY; ///< Slot index inside the Registry.
    uint32_t  generation = 0u;          ///< Incremented each time the slot is recycled.

    // Convenience: treat as bool to test for validity.
    [[nodiscard]] explicit operator bool() const noexcept {
        return id != NULL_ENTITY;
    }

    [[nodiscard]] bool operator==(const Entity& o) const noexcept {
        return id == o.id && generation == o.generation;
    }
    [[nodiscard]] bool operator!=(const Entity& o) const noexcept {
        return !(*this == o);
    }
};

/// Null / invalid entity constant — use anywhere a "no entity" sentinel is needed.
inline constexpr Entity INVALID_ENTITY{ NULL_ENTITY, 0u };

} // namespace EduEngine

// ---------------------------------------------------------------------------
// std::hash specialisation so Entity can be used as an unordered_map key.
// ---------------------------------------------------------------------------
namespace std {
template<>
struct hash<EduEngine::Entity> {
    std::size_t operator()(const EduEngine::Entity& e) const noexcept {
        // Mix id and generation into a single hash.
        std::size_t h1 = std::hash<uint32_t>{}(e.id);
        std::size_t h2 = std::hash<uint32_t>{}(e.generation);
        return h1 ^ (h2 << 16u);
    }
};
} // namespace std
