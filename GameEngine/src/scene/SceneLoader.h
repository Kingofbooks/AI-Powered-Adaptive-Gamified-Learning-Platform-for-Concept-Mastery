// =============================================================================
// SceneLoader.h  —  Data-Driven Scene Loading & Saving
// =============================================================================
#pragma once

#include "../core/ecs/Registry.h"
#include <nlohmann/json.hpp>
#include <string>

namespace EduEngine {

struct WorldSettings {
    float gravity = 980.f;
    int   bg_r = 15, bg_g = 15, bg_b = 25, bg_a = 255;
};

struct SceneMeta {
    std::string id, title, subject, grade, ai_prompt;
};

class SceneLoader {
public:
    void LoadScene(const std::string& json_path, Registry& registry);
    void HotReload(const std::string& json_path, Registry& registry);

    /// Serialise the current registry state back to JSON.
    void SaveScene(const std::string& json_path, const Registry& registry) const;

    [[nodiscard]] const SceneMeta&     GetMeta()    const noexcept { return m_meta; }
    [[nodiscard]] const WorldSettings& GetWorld()   const noexcept { return m_world; }
    [[nodiscard]] bool                 IsLoaded()   const noexcept { return m_loaded; }
    [[nodiscard]] const std::string&   GetLastPath()const noexcept { return m_last_path; }
    [[nodiscard]] const nlohmann::json& GetDoc()    const noexcept { return m_doc; }

private:
    void ParseMeta(const nlohmann::json& j);
    void ParseWorld(const nlohmann::json& j);
    void SpawnEntity(const nlohmann::json& j, Registry& registry);

    void ParseTransform(EntityID e,  const nlohmann::json& j, Registry& r);
    void ParseSprite(EntityID e,     const nlohmann::json& j, Registry& r);
    void ParseRigidBody(EntityID e,  const nlohmann::json& j, Registry& r);
    void ParseCollider(EntityID e,   const nlohmann::json& j, Registry& r);
    void ParsePlayerTag(EntityID e,  const nlohmann::json& j, Registry& r);
    void ParseCollectible(EntityID e,const nlohmann::json& j, Registry& r);
    void ParseTextLabel(EntityID e,  const nlohmann::json& j, Registry& r);
    void ParsePlatform(EntityID e,   const nlohmann::json& j, Registry& r);
    void ParseInfoZone(EntityID e,   const nlohmann::json& j, Registry& r);
    void ParseEnemy(EntityID e,      const nlohmann::json& j, Registry& r);
    void ParseGoal(EntityID e,       const nlohmann::json& j, Registry& r);

    SceneMeta      m_meta;
    WorldSettings  m_world;
    nlohmann::json m_doc;
    std::string    m_last_path;
    bool           m_loaded = false;
};

} // namespace EduEngine
