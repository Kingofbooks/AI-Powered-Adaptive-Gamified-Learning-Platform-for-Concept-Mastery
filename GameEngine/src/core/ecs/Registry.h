// =============================================================================
// Registry.h — Central ECS Registry (Header-Only Template Implementation)
// =============================================================================
// The Registry is the heart of the ECS.  It:
//   1. Allocates and recycles EntityIDs with a generation counter.
//   2. Stores components in typed, contiguous ComponentPool<T> containers.
//   3. Provides a View<Ts...>() helper to iterate over entity subsets.
//
// Design choices:
//   • Sparse-set storage per component type — O(1) add/remove, cache-friendly
//     iteration, no upfront pool sizing.
//   • Type-ID via std::type_index (standard, portable, no macros needed).
//   • Generation counter per slot prevents dangling-handle bugs.
// =============================================================================
#pragma once

#include "Entity.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

namespace EduEngine {

// ─────────────────────────────────────────────────────────────────────────────
// ComponentTypeID — maps a C++ type to a runtime integer key.
// ─────────────────────────────────────────────────────────────────────────────
using ComponentTypeID = std::type_index;

template<typename T>
[[nodiscard]] ComponentTypeID GetComponentTypeID() noexcept {
    return std::type_index(typeid(T));
}

// ─────────────────────────────────────────────────────────────────────────────
// IComponentPool — type-erased base for polymorphic component storage.
// ─────────────────────────────────────────────────────────────────────────────
struct IComponentPool {
    virtual ~IComponentPool() = default;
    virtual void RemoveComponent(EntityID entity) = 0;
    virtual bool HasComponent(EntityID entity)    const = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// ComponentPool<T> — dense-array storage with sparse entity→index mapping.
// ─────────────────────────────────────────────────────────────────────────────
template<typename T>
class ComponentPool final : public IComponentPool {
public:
    // -----------------------------------------------------------------------
    // Add a component to the given entity.  Returns a reference to the stored
    // component so the caller can immediately modify it.
    // -----------------------------------------------------------------------
    T& AddComponent(EntityID entity, T component) {
        assert(!HasComponent(entity) && "AddComponent: entity already owns this type.");

        const std::size_t new_index = m_components.size();
        m_entity_to_index[entity]   = new_index;
        m_index_to_entity.push_back(entity);
        m_components.push_back(std::move(component));
        return m_components.back();
    }

    // -----------------------------------------------------------------------
    // Retrieve a mutable reference.  Asserts in debug if entity doesn't own T.
    // -----------------------------------------------------------------------
    [[nodiscard]] T& GetComponent(EntityID entity) {
        assert(HasComponent(entity) && "GetComponent: entity does not own this type.");
        return m_components[m_entity_to_index.at(entity)];
    }
    [[nodiscard]] const T& GetComponent(EntityID entity) const {
        assert(HasComponent(entity) && "GetComponent: entity does not own this type.");
        return m_components[m_entity_to_index.at(entity)];
    }

    // -----------------------------------------------------------------------
    // O(1) removal using swap-with-last trick to keep the dense array packed.
    // -----------------------------------------------------------------------
    void RemoveComponent(EntityID entity) override {
        if (!HasComponent(entity)) return;

        const std::size_t removed_idx = m_entity_to_index.at(entity);
        const std::size_t last_idx    = m_components.size() - 1u;

        if (removed_idx != last_idx) {
            // Move last element into the gap.
            m_components[removed_idx]    = std::move(m_components[last_idx]);
            EntityID last_entity         = m_index_to_entity[last_idx];
            m_entity_to_index[last_entity] = removed_idx;
            m_index_to_entity[removed_idx] = last_entity;
        }

        m_components.pop_back();
        m_index_to_entity.pop_back();
        m_entity_to_index.erase(entity);
    }

    [[nodiscard]] bool HasComponent(EntityID entity) const override {
        return m_entity_to_index.count(entity) > 0;
    }

    // -----------------------------------------------------------------------
    // Accessors for direct iteration (used by View<> and the editor).
    // -----------------------------------------------------------------------
    [[nodiscard]] std::vector<T>&        GetAll()        { return m_components; }
    [[nodiscard]] const std::vector<T>&  GetAll()  const { return m_components; }
    [[nodiscard]] const std::vector<EntityID>& GetEntities() const { return m_index_to_entity; }

private:
    std::vector<T>                             m_components;      ///< Dense component array.
    std::vector<EntityID>                      m_index_to_entity; ///< index → owning entity.
    std::unordered_map<EntityID, std::size_t>  m_entity_to_index; ///< entity → dense index.
};

// ─────────────────────────────────────────────────────────────────────────────
// Registry — the public API consumed by the rest of the engine.
// ─────────────────────────────────────────────────────────────────────────────
class Registry {
public:
    // =========================================================================
    // Entity lifecycle
    // =========================================================================

    /// Allocate a new Entity.  Reuses recycled slots first.
    [[nodiscard]] Entity CreateEntity() {
        Entity e{};
        if (!m_free_slots.empty()) {
            e.id = m_free_slots.back();
            m_free_slots.pop_back();
            // Generation was already bumped when the slot was freed.
            e.generation = m_generations[e.id];
        } else {
            e.id = m_next_id++;
            m_generations.push_back(0u);
            e.generation = 0u;
        }
        m_living.insert(e.id);
        return e;
    }

    /// Destroy an entity and remove all its components.
    void DestroyEntity(Entity e) {
        if (!IsAlive(e)) return;

        for (auto& [type_id, pool] : m_pools) {
            pool->RemoveComponent(e.id);
        }
        m_names.erase(e.id);
        m_tags.erase(e.id);
        m_living.erase(e.id);
        m_free_slots.push_back(e.id);
        ++m_generations[e.id]; // Invalidate existing handles.
    }

    /// Destroy every living entity (used on scene change / hot-reload).
    void DestroyAllEntities() {
        // Collect first to avoid iterator invalidation.
        const std::vector<EntityID> snapshot(m_living.begin(), m_living.end());
        for (EntityID raw_id : snapshot) {
            Entity e{ raw_id, m_generations[raw_id] };
            DestroyEntity(e);
        }
    }

    /// Returns true if the handle refers to an entity that is still alive.
    [[nodiscard]] bool IsAlive(Entity e) const noexcept {
        return e.id != NULL_ENTITY
            && e.id < m_generations.size()
            && m_generations[e.id] == e.generation
            && m_living.count(e.id) > 0;
    }

    // =========================================================================
    // Name & tag helpers  (editor-facing metadata — not gameplay-critical)
    // =========================================================================

    void SetName(EntityID id, std::string name) { m_names[id] = std::move(name); }

    [[nodiscard]] const std::string& GetName(EntityID id) const {
        static const std::string fallback = "<unnamed>";
        auto it = m_names.find(id);
        return it != m_names.end() ? it->second : fallback;
    }

    void AddTag(EntityID id, const std::string& tag) { m_tags[id].insert(tag); }

    [[nodiscard]] bool HasTag(EntityID id, const std::string& tag) const {
        auto it = m_tags.find(id);
        return it != m_tags.end() && it->second.count(tag) > 0;
    }

    // =========================================================================
    // Component management  (fully templated — zero virtual dispatch overhead)
    // =========================================================================

    template<typename T>
    T& AddComponent(EntityID id, T component = T{}) {
        return Pool<T>().AddComponent(id, std::move(component));
    }

    template<typename T>
    [[nodiscard]] T& GetComponent(EntityID id) {
        return Pool<T>().GetComponent(id);
    }

    template<typename T>
    [[nodiscard]] const T& GetComponent(EntityID id) const {
        return ConstPool<T>().GetComponent(id);
    }

    template<typename T>
    [[nodiscard]] bool HasComponent(EntityID id) const noexcept {
        auto it = m_pools.find(GetComponentTypeID<T>());
        return it != m_pools.end() && it->second->HasComponent(id);
    }

    template<typename T>
    void RemoveComponent(EntityID id) {
        Pool<T>().RemoveComponent(id);
    }

    // =========================================================================
    // View<T1, T2, …>() — iterate entities owning ALL listed component types.
    //
    // Usage:
    //   registry.View<TransformComponent, SpriteComponent>(
    //       [](EntityID id, TransformComponent& t, SpriteComponent& s) {
    //           /* render s at position t */
    //       });
    //
    // The implementation iterates the smallest pool first (approximate — we
    // always iterate the first listed type) and skips entities missing any
    // of the other types.  For a production engine you'd sort pools by size.
    // =========================================================================
    template<typename... Ts, typename Func>
    void View(Func&& func) {
        static_assert(sizeof...(Ts) > 0, "View requires at least one component type.");

        // Pick the primary pool (first template arg) to iterate over.
        auto& primary = Pool<std::tuple_element_t<0, std::tuple<Ts...>>>();

        for (EntityID id : primary.GetEntities()) {
            // Fold-expression: confirm every other component is present.
            if ((HasComponent<Ts>(id) && ...)) {
                func(id, GetComponent<Ts>(id)...);
            }
        }
    }

    // =========================================================================
    // Introspection helpers  (used by the ImGui Editor)
    // =========================================================================

    [[nodiscard]] const std::unordered_set<EntityID>& GetAllIDs() const {
        return m_living;
    }

    [[nodiscard]] const std::unordered_map<EntityID, std::string>& GetAllNames() const {
        return m_names;
    }

    // Query which component type IDs a given entity owns.
    [[nodiscard]] std::vector<ComponentTypeID> GetComponentTypes(EntityID id) const {
        std::vector<ComponentTypeID> result;
        for (auto& [type_id, pool] : m_pools) {
            if (pool->HasComponent(id)) result.push_back(type_id);
        }
        return result;
    }

private:
    // -----------------------------------------------------------------------
    // Internal pool accessors — lazily create pools on first use.
    // -----------------------------------------------------------------------
    template<typename T>
    ComponentPool<T>& Pool() {
        const auto key = GetComponentTypeID<T>();
        auto  it       = m_pools.find(key);
        if (it == m_pools.end()) {
            m_pools[key] = std::make_unique<ComponentPool<T>>();
            it           = m_pools.find(key);
        }
        return *static_cast<ComponentPool<T>*>(it->second.get());
    }

    template<typename T>
    const ComponentPool<T>& ConstPool() const {
        const auto key = GetComponentTypeID<T>();
        auto  it       = m_pools.find(key);
        assert(it != m_pools.end() && "ConstPool: component type never registered.");
        return *static_cast<const ComponentPool<T>*>(it->second.get());
    }

    // -----------------------------------------------------------------------
    // Member data
    // -----------------------------------------------------------------------
    EntityID                                              m_next_id = 1u; // 0 reserved as NULL_ENTITY
    std::vector<uint32_t>                                 m_generations;  // per-slot generation counter
    std::vector<EntityID>                                 m_free_slots;   // recycled IDs
    std::unordered_set<EntityID>                          m_living;       // currently alive IDs
    std::unordered_map<ComponentTypeID,
        std::unique_ptr<IComponentPool>>                  m_pools;        // type → pool
    std::unordered_map<EntityID, std::string>             m_names;        // editor display names
    std::unordered_map<EntityID,
        std::unordered_set<std::string>>                  m_tags;         // gameplay tags
};

} // namespace EduEngine
