// =============================================================================
// SceneLoader.cpp
// =============================================================================
#include "SceneLoader.h"
#include "../core/ecs/Component.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

using json = nlohmann::json;

namespace EduEngine {

void SceneLoader::LoadScene(const std::string& path, Registry& registry) {
    std::cout << "[SceneLoader] Loading: " << path << '\n';
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("Cannot open: " + path);
    try { m_doc = json::parse(f, nullptr, true, true); }
    catch (const json::parse_error& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }
    if (m_doc.contains("scene_meta"))    ParseMeta(m_doc["scene_meta"]);
    if (m_doc.contains("world_settings")) ParseWorld(m_doc["world_settings"]);
    if (m_doc.contains("entities"))
        for (const auto& e : m_doc["entities"]) SpawnEntity(e, registry);
    m_last_path = path;
    m_loaded    = true;
    std::cout << "[SceneLoader] Loaded \"" << m_meta.title << "\"\n";
}

void SceneLoader::HotReload(const std::string& path, Registry& registry) {
    registry.DestroyAllEntities();
    LoadScene(path, registry);
}

// =============================================================================
// SaveScene
// =============================================================================
void SceneLoader::SaveScene(const std::string& path, const Registry& registry) const {
    json doc;
    if (m_doc.contains("scene_meta"))     doc["scene_meta"]     = m_doc["scene_meta"];
    if (m_doc.contains("world_settings")) doc["world_settings"] = m_doc["world_settings"];

    std::vector<EntityID> ids(registry.GetAllIDs().begin(), registry.GetAllIDs().end());
    std::sort(ids.begin(), ids.end());

    json entities = json::array();
    for (EntityID id : ids) {
        json ej;
        ej["name"] = registry.GetName(id);
        json c = json::object();

        if (registry.HasComponent<TransformComponent>(id)) {
            const auto& t = registry.GetComponent<TransformComponent>(id);
            c["transform"] = {{"x",t.x},{"y",t.y},{"scale_x",t.scale_x},
                              {"scale_y",t.scale_y},{"rotation",t.rotation}};
        }
        if (registry.HasComponent<SpriteComponent>(id)) {
            const auto& s = registry.GetComponent<SpriteComponent>(id);
            c["sprite"] = {{"texture",s.texture_path},
                {"color",{s.color.r,s.color.g,s.color.b,s.color.a}},
                {"layer",s.layer},{"visible",s.visible},{"flip_h",s.flip_h}};
        }
        if (registry.HasComponent<RigidBodyComponent>(id)) {
            const auto& rb = registry.GetComponent<RigidBodyComponent>(id);
            c["rigid_body"] = {{"mass",rb.mass},{"gravity_scale",rb.gravity_scale},
                               {"is_kinematic",rb.is_kinematic}};
        }
        if (registry.HasComponent<ColliderComponent>(id)) {
            const auto& col = registry.GetComponent<ColliderComponent>(id);
            c["collider"] = {{"offset_x",col.offset_x},{"offset_y",col.offset_y},
                             {"width",col.width},{"height",col.height},
                             {"is_trigger",col.is_trigger}};
        }
        if (registry.HasComponent<PlayerTagComponent>(id)) {
            const auto& p = registry.GetComponent<PlayerTagComponent>(id);
            c["player_tag"] = {{"move_speed",p.move_speed},{"jump_force",p.jump_force},
                               {"max_jumps",p.max_jump_count}};
        }
        if (registry.HasComponent<CollectibleComponent>(id)) {
            const auto& col = registry.GetComponent<CollectibleComponent>(id);
            c["collectible"] = {{"point_value",col.point_value},{"label",col.label}};
        }
        if (registry.HasComponent<TextLabelComponent>(id)) {
            const auto& t = registry.GetComponent<TextLabelComponent>(id);
            c["text_label"] = {{"text",t.text},
                {"color",{t.color.r,t.color.g,t.color.b,t.color.a}},
                {"font_size",t.font_size},{"layer",t.layer}};
        }
        if (registry.HasComponent<PlatformComponent>(id)) {
            c["platform"] = {{"is_one_way",
                registry.GetComponent<PlatformComponent>(id).is_one_way}};
        }
        if (registry.HasComponent<InfoZoneComponent>(id)) {
            const auto& z = registry.GetComponent<InfoZoneComponent>(id);
            c["info_zone"] = {{"text",z.text},{"repeat",z.repeat}};
        }
        if (registry.HasComponent<EnemyComponent>(id)) {
            const auto& e = registry.GetComponent<EnemyComponent>(id);
            c["enemy"] = {{"patrol_min_x",e.patrol_min_x},
                          {"patrol_max_x",e.patrol_max_x},{"speed",e.speed}};
        }
        if (registry.HasComponent<GoalComponent>(id))
            c["goal"] = json::object();

        ej["components"] = c;
        entities.push_back(ej);
    }
    doc["entities"] = entities;

    std::ofstream out(path);
    if (!out.is_open()) { std::cerr << "[SceneLoader] Cannot save: " << path << '\n'; return; }
    out << doc.dump(2);
    std::cout << "[SceneLoader] Saved: " << path << '\n';
}

// =============================================================================
// Parsers
// =============================================================================
void SceneLoader::ParseMeta(const json& j) {
    m_meta.id=j.value("id","scene"); m_meta.title=j.value("title","Untitled");
    m_meta.subject=j.value("subject",""); m_meta.grade=j.value("grade","");
    m_meta.ai_prompt=j.value("ai_prompt","");
}
void SceneLoader::ParseWorld(const json& j) {
    m_world.gravity = j.value("gravity",980.f);
    if (j.contains("background_color") && j["background_color"].size()>=4) {
        m_world.bg_r=j["background_color"][0]; m_world.bg_g=j["background_color"][1];
        m_world.bg_b=j["background_color"][2]; m_world.bg_a=j["background_color"][3];
    }
}
void SceneLoader::SpawnEntity(const json& j, Registry& registry) {
    Entity e = registry.CreateEntity();
    registry.SetName(e.id, j.value("name","Entity"));
    if (j.contains("tags"))
        for (const auto& t : j["tags"]) registry.AddTag(e.id, t.get<std::string>());
    if (!j.contains("components")) return;
    const auto& c = j["components"];
    if (c.contains("transform"))  ParseTransform(e.id, c["transform"],  registry);
    if (c.contains("sprite"))     ParseSprite(e.id,    c["sprite"],     registry);
    if (c.contains("rigid_body")) ParseRigidBody(e.id, c["rigid_body"], registry);
    if (c.contains("collider"))   ParseCollider(e.id,  c["collider"],   registry);
    if (c.contains("player_tag")) ParsePlayerTag(e.id, c["player_tag"], registry);
    if (c.contains("collectible"))ParseCollectible(e.id,c["collectible"],registry);
    if (c.contains("text_label")) ParseTextLabel(e.id, c["text_label"], registry);
    if (c.contains("platform"))   ParsePlatform(e.id,  c["platform"],   registry);
    if (c.contains("info_zone"))  ParseInfoZone(e.id,  c["info_zone"],  registry);
    if (c.contains("enemy"))      ParseEnemy(e.id,     c["enemy"],      registry);
    if (c.contains("goal"))       ParseGoal(e.id,      c["goal"],       registry);
    std::cout << "[SceneLoader]  + " << registry.GetName(e.id) << " (id="<<e.id<<")\n";
}
void SceneLoader::ParseTransform(EntityID e, const json& j, Registry& r) {
    r.AddComponent(e, TransformComponent{j.value("x",0.f),j.value("y",0.f),
        j.value("scale_x",1.f),j.value("scale_y",1.f),j.value("rotation",0.f)});
}
void SceneLoader::ParseSprite(EntityID e, const json& j, Registry& r) {
    SpriteComponent s;
    s.texture_path=j.value("texture","");
    s.layer=j.value("layer",0); s.visible=j.value("visible",true); s.flip_h=j.value("flip_h",false);
    if (j.contains("color")&&j["color"].size()>=4)
        s.color={uint8_t(j["color"][0].get<int>()),uint8_t(j["color"][1].get<int>()),
                 uint8_t(j["color"][2].get<int>()),uint8_t(j["color"][3].get<int>())};
    r.AddComponent(e, s);
}
void SceneLoader::ParseRigidBody(EntityID e, const json& j, Registry& r) {
    r.AddComponent(e, RigidBodyComponent{0.f,0.f,
        j.value("mass",1.f),j.value("gravity_scale",1.f),j.value("is_kinematic",false)});
}
void SceneLoader::ParseCollider(EntityID e, const json& j, Registry& r) {
    r.AddComponent(e, ColliderComponent{j.value("offset_x",0.f),j.value("offset_y",0.f),
        j.value("width",32.f),j.value("height",32.f),j.value("is_trigger",false)});
}
void SceneLoader::ParsePlayerTag(EntityID e, const json& j, Registry& r) {
    PlayerTagComponent p;
    p.move_speed=j.value("move_speed",220.f);
    p.jump_force=j.value("jump_force",-580.f);
    p.max_jump_count=j.value("max_jumps",2);
    r.AddComponent(e, p);
}
void SceneLoader::ParseCollectible(EntityID e, const json& j, Registry& r) {
    r.AddComponent(e, CollectibleComponent{j.value("point_value",10),j.value("label","")});
}
void SceneLoader::ParseTextLabel(EntityID e, const json& j, Registry& r) {
    TextLabelComponent t;
    t.text=j.value("text",""); t.font_size=j.value("font_size",16); t.layer=j.value("layer",10);
    if (j.contains("color")&&j["color"].size()>=4)
        t.color={uint8_t(j["color"][0].get<int>()),uint8_t(j["color"][1].get<int>()),
                 uint8_t(j["color"][2].get<int>()),uint8_t(j["color"][3].get<int>())};
    r.AddComponent(e, t);
}
void SceneLoader::ParsePlatform(EntityID e, const json& j, Registry& r) {
    r.AddComponent(e, PlatformComponent{j.value("is_one_way",false)});
}
void SceneLoader::ParseInfoZone(EntityID e, const json& j, Registry& r) {
    InfoZoneComponent z; z.text=j.value("text",""); z.repeat=j.value("repeat",false);
    r.AddComponent(e, z);
}
void SceneLoader::ParseEnemy(EntityID e, const json& j, Registry& r) {
    EnemyComponent en;
    en.patrol_min_x=j.value("patrol_min_x",0.f); en.patrol_max_x=j.value("patrol_max_x",400.f);
    en.speed=j.value("speed",80.f);
    r.AddComponent(e, en);
}
void SceneLoader::ParseGoal(EntityID e, const json&, Registry& r) {
    r.AddComponent(e, GoalComponent{});
}

} // namespace EduEngine
