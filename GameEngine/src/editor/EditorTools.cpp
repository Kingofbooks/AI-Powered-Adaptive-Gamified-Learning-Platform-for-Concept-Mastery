// =============================================================================
// EditorTools.cpp  —  Two-Mode ImGui Editor Implementation
// =============================================================================
#include "EditorTools.h"

#include "../core/Engine.h"
#include "../scene/SceneLoader.h"
#include "../prompt/PromptSystem.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

#include <SDL2/SDL.h>

#include <algorithm>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace EduEngine {

// =============================================================================
// Lifecycle
// =============================================================================

void EditorTools::Init(SDL_Window* window, SDL_Renderer* renderer) {
    m_window   = window;
    m_renderer = renderer;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& s        = ImGui::GetStyle();
    s.WindowRounding     = 6.f;
    s.FrameRounding      = 4.f;
    s.GrabRounding       = 4.f;
    s.WindowPadding      = {12, 10};
    s.FramePadding       = {6, 4};
    s.Colors[ImGuiCol_TitleBgActive]  = ImVec4(0.10f,0.42f,0.32f,1.f);
    s.Colors[ImGuiCol_Header]         = ImVec4(0.12f,0.48f,0.36f,0.55f);
    s.Colors[ImGuiCol_HeaderHovered]  = ImVec4(0.16f,0.62f,0.46f,0.80f);
    s.Colors[ImGuiCol_Button]         = ImVec4(0.12f,0.45f,0.33f,0.80f);
    s.Colors[ImGuiCol_ButtonHovered]  = ImVec4(0.18f,0.62f,0.46f,1.00f);
    s.Colors[ImGuiCol_CheckMark]      = ImVec4(0.20f,0.90f,0.65f,1.00f);

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
}

void EditorTools::ProcessEvent(const SDL_Event& ev) {
    ImGui_ImplSDL2_ProcessEvent(&ev);
}

void EditorTools::BeginFrame() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void EditorTools::EndFrame() {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
}

void EditorTools::Shutdown() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

// =============================================================================
// Top menu bar — mode switcher
// =============================================================================
void EditorTools::RenderModeBar(Engine& engine) {
    if (!ImGui::BeginMainMenuBar()) return;

    const EngineMode cur = engine.GetMode();
    auto ModeBtn = [&](const char* label, EngineMode mode) {
        bool active = (cur == mode);
        if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.18f,0.62f,0.46f,1.f));
        else        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.22f,0.22f,0.27f,1.f));
        if (ImGui::Button(label)) engine.SetMode(mode);
        ImGui::PopStyleColor();
        ImGui::SameLine(0,4);
    };

    ModeBtn("  Prompt Mode  ", EngineMode::Prompt);
    ModeBtn("  Build Mode  ",  EngineMode::Build);
    ModeBtn("  Play  ",        EngineMode::Play);

    // Scene path on the right.
    const std::string& sp = engine.GetScenePath();
    ImGui::SameLine(ImGui::GetWindowWidth() - 400.f);
    ImGui::TextDisabled("Scene: %s", sp.empty() ? "(none)" : sp.c_str());

    ImGui::EndMainMenuBar();
}

// =============================================================================
// PROMPT MODE
// =============================================================================
void EditorTools::RenderPromptMode(PromptSystem& prompt, Engine& engine) {
    const ImGuiIO& io = ImGui::GetIO();
    const float W = io.DisplaySize.x;
    const float H = io.DisplaySize.y - 20.f;

    ImGui::SetNextWindowPos(ImVec2(0, 20),   ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(W, H),   ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.97f);

    ImGui::Begin("##pm", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoScrollbar);

    // Header.
    ImGui::SetWindowFontScale(1.35f);
    ImGui::TextColored(ImVec4(0.3f,0.9f,0.6f,1.f), "  EduEngine  —  Prompt Mode");
    ImGui::SetWindowFontScale(1.f);
    ImGui::TextDisabled("  Describe a game and the engine will build it for you.");
    ImGui::Separator(); ImGui::Spacing();

    // Prompt text area.
    ImGui::Text("Game Description:");
    ImGui::SetNextItemWidth(W - 40.f);
    ImGui::InputTextMultiline("##pb", m_prompt_buf, sizeof(m_prompt_buf),
        ImVec2(-1, 90));
    ImGui::TextDisabled(
        "Examples:  \"A platformer about the solar system for 5th grade\"\n"
        "           \"A game teaching the water cycle for grade 4\"\n"
        "           \"A biology platformer about DNA for 8th graders\"");
    ImGui::Spacing();

    // Parameters.
    ImGui::SetNextItemWidth(220.f);
    ImGui::InputText("Subject (optional)##ps", m_subject_buf, sizeof(m_subject_buf));
    ImGui::SameLine(0,16);
    ImGui::SetNextItemWidth(80.f);
    ImGui::InputInt("Grade##pg", &m_grade,1,1);
    m_grade = std::clamp(m_grade, 1, 12);
    ImGui::Spacing();

    // Provider selection.
    ImGui::Text("AI Provider:");
    ImGui::SameLine();
    if (ImGui::RadioButton("Mock (offline — instant)", m_mock_mode)) {
        m_mock_mode = true;
        prompt.SetMockMode(true);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Custom API", !m_mock_mode)) {
        m_mock_mode = false;
        prompt.SetMockMode(false);
    }
    if (!m_mock_mode) {
        ImGui::SetNextItemWidth(360.f);
        if (ImGui::InputText("Endpoint##pu", m_api_url_buf, sizeof(m_api_url_buf)))
            prompt.SetApiEndpoint(m_api_url_buf);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200.f);
        if (ImGui::InputText("Key##pk", m_api_key_buf, sizeof(m_api_key_buf),
                             ImGuiInputTextFlags_Password))
            prompt.SetApiKey(m_api_key_buf);
    }
    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

    // Generate button.
    const GenerationStatus status = prompt.GetStatus();
    const bool busy = (status == GenerationStatus::Running);

    ImGui::BeginDisabled(busy || m_prompt_buf[0] == '\0');
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.12f,0.55f,0.38f,1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f,0.72f,0.50f,1.f));
    if (ImGui::Button("  Generate Game  ", ImVec2(200,48))) {
        PromptConfig cfg;
        cfg.prompt    = m_prompt_buf;
        cfg.subject   = m_subject_buf;
        cfg.grade     = m_grade;
        cfg.game_type = "platformer";
        prompt.Reset();
        prompt.Generate(cfg);
        m_preview_json.clear();
    }
    ImGui::PopStyleColor(2);
    ImGui::EndDisabled();

    // Status / spinner.
    ImGui::SameLine(0,20);
    if (busy) {
        const char* sp[] = {"|","/","-","\\"};
        ImGui::TextColored(ImVec4(0.4f,0.9f,1.f,1.f),
            "%s  Generating…", sp[(SDL_GetTicks()/200)%4]);
    } else if (status == GenerationStatus::Done && m_preview_json.empty()) {
        m_preview_json = prompt.GetLastResult();
        const std::string saved = prompt.SaveToFile("data/scenes/");
        if (!saved.empty()) {
            engine.LoadScene(saved);
            engine.SetMode(EngineMode::Build);
        }
        prompt.Reset();
    } else if (status == GenerationStatus::Error) {
        ImGui::TextColored(ImVec4(1.f,0.3f,0.3f,1.f),
            "Error: %s", prompt.GetLastError().c_str());
    }

    // JSON preview (read-only).
    if (!m_preview_json.empty()) {
        ImGui::Spacing(); ImGui::Separator();
        ImGui::TextDisabled("Generated JSON (preview — read only):");
        ImGui::InputTextMultiline("##jprev",
            const_cast<char*>(m_preview_json.c_str()),
            m_preview_json.size() + 1,
            ImVec2(-1, H - 400.f),
            ImGuiInputTextFlags_ReadOnly);
    }

    ImGui::End();
}

// =============================================================================
// BUILD MODE
// =============================================================================
void EditorTools::RenderBuildMode(Registry& registry, SceneLoader& loader, Engine& engine) {
    RenderHierarchy(registry, engine);
    RenderInspector(registry);
    RenderSceneToolbar(loader, engine);
    RenderExportModal(engine);
}

void EditorTools::RenderHierarchy(Registry& registry, Engine& engine) {
    ImGui::SetNextWindowSize(ImVec2(255, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(8, 28),     ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene Hierarchy");

    if (ImGui::Button("[+] New Entity")) {
        Entity e = registry.CreateEntity();
        registry.SetName(e.id, "Entity_" + std::to_string(e.id));
        registry.AddComponent(e.id, TransformComponent{});
        registry.AddComponent(e.id, SpriteComponent{});
        m_selected = e.id;
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(m_selected == NULL_ENTITY);
    if (ImGui::Button("[x] Delete")) {
        Entity e{m_selected, 0};
        registry.DestroyEntity(e);
        m_selected = NULL_ENTITY;
    }
    ImGui::EndDisabled();

    ImGui::Separator();
    ImGui::TextDisabled("Entities: %zu", registry.GetAllIDs().size());
    ImGui::Separator();

    std::vector<EntityID> ids(registry.GetAllIDs().begin(), registry.GetAllIDs().end());
    std::sort(ids.begin(), ids.end());

    for (EntityID id : ids) {
        char label[128];
        std::snprintf(label, sizeof(label), "[%2u]  %s", id,
            registry.GetName(id).c_str());
        const bool sel = (id == m_selected);
        if (ImGui::Selectable(label, sel)) m_selected = id;
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            if (registry.HasComponent<PlayerTagComponent>(id))   ImGui::Text("PLAYER");
            if (registry.HasComponent<PlatformComponent>(id))    ImGui::Text("platform");
            if (registry.HasComponent<CollectibleComponent>(id)) ImGui::Text("collectible");
            if (registry.HasComponent<InfoZoneComponent>(id))    ImGui::Text("info zone");
            if (registry.HasComponent<EnemyComponent>(id))       ImGui::Text("enemy");
            if (registry.HasComponent<GoalComponent>(id))        ImGui::Text("goal");
            ImGui::EndTooltip();
        }
    }

    ImGui::End();
}

void EditorTools::RenderInspector(Registry& registry) {
    ImGui::SetNextWindowSize(ImVec2(345, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(270, 28),   ImGuiCond_FirstUseEver);
    ImGui::Begin("Property Inspector");

    if (m_selected == NULL_ENTITY || !registry.GetAllIDs().count(m_selected)) {
        ImGui::TextDisabled("Select an entity in the Hierarchy.");
        ImGui::End(); return;
    }

    char nbuf[128];
    std::strncpy(nbuf, registry.GetName(m_selected).c_str(), sizeof(nbuf));
    if (ImGui::InputText("Name##n", nbuf, sizeof(nbuf)))
        registry.SetName(m_selected, nbuf);
    ImGui::TextColored(ImVec4(0.4f,0.85f,0.6f,1.f), "ID: %u", m_selected);
    ImGui::Separator();

    InspectTransform(m_selected,   registry);
    InspectSprite(m_selected,      registry);
    InspectRigidBody(m_selected,   registry);
    InspectCollider(m_selected,    registry);
    InspectPlayerTag(m_selected,   registry);
    InspectCollectible(m_selected, registry);
    InspectTextLabel(m_selected,   registry);
    InspectPlatform(m_selected,    registry);
    InspectInfoZone(m_selected,    registry);
    InspectEnemy(m_selected,       registry);
    InspectGoal(m_selected,        registry);

    ImGui::Separator();
    RenderAddComponentMenu(m_selected, registry);
    ImGui::End();
}

void EditorTools::RenderSceneToolbar(SceneLoader& loader, Engine& engine) {
    ImGui::SetNextWindowSize(ImVec2(860, 56), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(8, 536),   ImGuiCond_FirstUseEver);
    ImGui::Begin("##stb", nullptr, ImGuiWindowFlags_NoTitleBar);

    const std::string& cur = engine.GetScenePath();
    if (m_scene_path_buf[0] == '\0' && !cur.empty())
        std::strncpy(m_scene_path_buf, cur.c_str(), sizeof(m_scene_path_buf)-1);

    ImGui::SetNextItemWidth(300.f);
    ImGui::InputText("##spath", m_scene_path_buf, sizeof(m_scene_path_buf));
    ImGui::SameLine();
    if (ImGui::Button("Save"))
        loader.SaveScene(m_scene_path_buf, engine.GetRegistry());
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        try { engine.LoadScene(m_scene_path_buf); }
        catch (const std::exception& e) { SDL_Log("Load error: %s", e.what()); }
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload F5") && !cur.empty())
        engine.LoadScene(cur);
    ImGui::SameLine();

    // Export button — opens the export modal.
    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.45f, 0.28f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.65f, 0.40f, 1.f));
    if (ImGui::Button("  Export Game  ")) {
        if (engine.GetScenePath().empty()) {
            m_export_msg       = "No scene loaded — generate or load a scene first.";
            m_export_msg_until = SDL_GetTicks() + 4000;
        } else {
            // Pre-fill destination with home dir
            const char* home = getenv("HOME");
            if (m_export_dest_buf[0] == '\0')
                std::strncpy(m_export_dest_buf,
                             home ? home : "/tmp",
                             sizeof(m_export_dest_buf) - 1);
            m_show_export_modal = true;
        }
    }
    ImGui::PopStyleColor(2);

    // Timed status toast (auto-hides after a few seconds).
    if (!m_export_msg.empty() && SDL_GetTicks() < m_export_msg_until) {
        ImGui::SameLine(0, 14);
        const bool is_err = m_export_msg.rfind("Export failed", 0) == 0 ||
                            m_export_msg.rfind("No scene",     0) == 0 ||
                            m_export_msg.rfind("Cannot",       0) == 0 ||
                            m_export_msg.rfind("Invalid",      0) == 0;
        if (is_err)
            ImGui::TextColored(ImVec4(1.f, 0.35f, 0.35f, 1.f), "%s", m_export_msg.c_str());
        else
            ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.6f, 1.f),  "%s", m_export_msg.c_str());
    }

    ImGui::End();
}

// =============================================================================
// EXPORT MODAL  —  "Save As" dialog with Browse button
// =============================================================================
void EditorTools::RenderExportModal(Engine& engine) {
    if (!m_show_export_modal) return;

    ImGui::OpenPopup("Export Game");
    ImGui::SetNextWindowSize(ImVec2(580, 160), ImGuiCond_Always);

    if (ImGui::BeginPopupModal("Export Game", nullptr,
                               ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Choose a folder to save the exported game into:");
        ImGui::Spacing();

        // Path text field
        ImGui::SetNextItemWidth(420.f);
        ImGui::InputText("##expdir", m_export_dest_buf, sizeof(m_export_dest_buf));
        ImGui::SameLine();

        // Browse button — opens kdialog / zenity folder picker
        if (ImGui::Button("Browse...")) {
            // Try kdialog (KDE / available on this system)
            std::string chosen;
            const char* tryDialogs[] = {
                "kdialog --title \"Export Game — Choose Folder\" "
                        "--getexistingdirectory \"$HOME\" 2>/dev/null",
                "zenity --title \"Export Game\" "
                        "--file-selection --directory 2>/dev/null",
            };
            for (const char* cmd : tryDialogs) {
                FILE* p = popen(cmd, "r");
                if (!p) continue;
                char buf[512] = {};
                if (fgets(buf, sizeof(buf), p)) {
                    chosen = buf;
                    // Strip trailing newline
                    while (!chosen.empty() &&
                           (chosen.back() == '\n' || chosen.back() == '\r'))
                        chosen.pop_back();
                }
                pclose(p);
                if (!chosen.empty()) break;
            }
            if (!chosen.empty())
                std::strncpy(m_export_dest_buf, chosen.c_str(),
                             sizeof(m_export_dest_buf) - 1);
        }

        ImGui::Spacing();
        ImGui::TextDisabled("A folder named after the scene will be created inside the chosen location.");
        ImGui::Spacing();

        // Confirm / Cancel
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.45f, 0.28f, 1.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.15f, 0.65f, 0.40f, 1.f));
        if (ImGui::Button("  Export  ", ImVec2(110, 0))) {
            m_show_export_modal = false;
            ImGui::CloseCurrentPopup();
            DoExportGame(engine, m_export_dest_buf);
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine(0, 12);
        if (ImGui::Button("Cancel", ImVec2(80, 0))) {
            m_show_export_modal = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

// =============================================================================
// DO EXPORT GAME  —  copies files into <dest_dir>/<scene_id>/
// =============================================================================
void EditorTools::DoExportGame(Engine& engine, const std::string& dest_dir) {
    namespace fs  = std::filesystem;
    using     json = nlohmann::json;

    if (dest_dir.empty()) {
        m_export_msg       = "No destination chosen — export cancelled.";
        m_export_msg_until = SDL_GetTicks() + 4000;
        return;
    }

    const std::string& scene_path = engine.GetScenePath();

    // ── Read + parse the scene JSON ───────────────────────────────────────
    std::ifstream fin(scene_path);
    if (!fin) {
        m_export_msg       = "Cannot read scene file: " + scene_path;
        m_export_msg_until = SDL_GetTicks() + 4000;
        return;
    }
    json scene_json;
    try {
        scene_json = json::parse(fin);
    } catch (const std::exception& e) {
        m_export_msg       = std::string("Invalid scene JSON: ") + e.what();
        m_export_msg_until = SDL_GetTicks() + 4000;
        return;
    }

    // ── Extract scene ID ──────────────────────────────────────────────────
    std::string scene_id = "exported_game";
    if (scene_json.contains("scene_meta") &&
        scene_json["scene_meta"].contains("id"))
        scene_id = scene_json["scene_meta"]["id"].get<std::string>();

    // ── Extract theme slug from texture paths ─────────────────────────────
    std::string slug = "default";
    if (scene_json.contains("entities")) {
        for (const auto& ent : scene_json["entities"]) {
            if (!ent.contains("components")) continue;
            if (!ent["components"].contains("sprite")) continue;
            std::string tex = ent["components"]["sprite"].value("texture", "");
            const std::string pfx = "assets/images/";
            size_t pos = tex.find(pfx);
            if (pos != std::string::npos) {
                std::string rest = tex.substr(pos + pfx.size());
                size_t slash = rest.find('/');
                if (slash != std::string::npos) {
                    slug = rest.substr(0, slash);
                    break;
                }
            }
        }
    }

    // ── Build files ───────────────────────────────────────────────────────
    const std::string export_dir = dest_dir + "/" + scene_id;
    const std::string json_fname = scene_id + ".json";

    try {
        fs::create_directories(export_dir + "/data/scenes");
        fs::create_directories(export_dir + "/assets/images");
        fs::create_directories(export_dir + "/assets/fonts");

        // Engine binary
        if (fs::exists("EduEngine")) {
            fs::copy_file("EduEngine",
                          export_dir + "/EduEngine",
                          fs::copy_options::overwrite_existing);
            fs::permissions(export_dir + "/EduEngine",
                            fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                            fs::perm_options::add);
        }

        // Scene JSON
        fs::copy_file(scene_path,
                      export_dir + "/data/scenes/" + json_fname,
                      fs::copy_options::overwrite_existing);

        // Theme images
        const std::string img_src = "assets/images/" + slug;
        if (fs::exists(img_src))
            fs::copy(img_src,
                     export_dir + "/assets/images/" + slug,
                     fs::copy_options::overwrite_existing | fs::copy_options::recursive);

        // Theme audio (optional)
        const std::string aud_src = "assets/audio/" + slug;
        if (fs::exists(aud_src)) {
            fs::create_directories(export_dir + "/assets/audio");
            fs::copy(aud_src,
                     export_dir + "/assets/audio/" + slug,
                     fs::copy_options::overwrite_existing | fs::copy_options::recursive);
        }

        // Fonts
        if (fs::exists("assets/fonts"))
            fs::copy("assets/fonts",
                     export_dir + "/assets/fonts",
                     fs::copy_options::overwrite_existing | fs::copy_options::recursive);

        // run.sh
        {
            std::ofstream sh(export_dir + "/run.sh");
            sh << "#!/bin/bash\n"
               << "# EduEngine — Exported Game: " << scene_id << "\n"
               << "DIR=\"$(cd \"$(dirname \"${BASH_SOURCE[0]}\")\" && pwd)\"\n"
               << "cd \"$DIR\"\n"
               << "DISPLAY=\"${DISPLAY:-:0}\" ./EduEngine \"data/scenes/" << json_fname << "\"\n";
        }
        fs::permissions(export_dir + "/run.sh",
                        fs::perms::owner_exec | fs::perms::group_exec | fs::perms::others_exec,
                        fs::perm_options::add);

        m_export_msg       = "Exported to: " + export_dir;
        m_export_msg_until = SDL_GetTicks() + 7000;
        SDL_Log("[Export] Done -> %s", export_dir.c_str());

    } catch (const std::exception& e) {
        m_export_msg       = std::string("Export failed: ") + e.what();
        m_export_msg_until = SDL_GetTicks() + 5000;
        SDL_Log("[Export] Error: %s", e.what());
    }
}

// =============================================================================
// Performance HUD
// =============================================================================
void EditorTools::RenderPerformanceHUD(float dt) {
    m_fps_buf[m_fps_head] = (dt > 0.f) ? 1.f/dt : 0.f;
    m_fps_head = (m_fps_head + 1) % FPS_BUF;

    ImGui::SetNextWindowSize(ImVec2(230, 74), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(1048, 28), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.5f);
    ImGui::Begin("##perf", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::Text("FPS %.1f  dt %.2f ms", 1.f/dt, dt*1000.f);
    ImGui::PlotLines("##f", m_fps_buf, FPS_BUF, m_fps_head,
        nullptr, 0.f, 120.f, ImVec2(-1, 34));
    ImGui::End();
}

// =============================================================================
// Inspector sub-panels
// =============================================================================
void EditorTools::InspectTransform(EntityID id, Registry& r) {
    if (!r.HasComponent<TransformComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) return;
    auto& t = r.GetComponent<TransformComponent>(id);
    ImGui::DragFloat("X##tx",  &t.x, 1.f);
    ImGui::DragFloat("Y##ty",  &t.y, 1.f);
    ImGui::DragFloat("Sx##tsx",&t.scale_x,0.01f,0.01f,20.f,"Scale X: %.2f");
    ImGui::DragFloat("Sy##tsy",&t.scale_y,0.01f,0.01f,20.f,"Scale Y: %.2f");
    ImGui::DragFloat("Rot##tr",&t.rotation,0.5f,-360.f,360.f,"Rot: %.1f deg");
}
void EditorTools::InspectSprite(EntityID id, Registry& r) {
    if (!r.HasComponent<SpriteComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Sprite")) return;
    auto& s = r.GetComponent<SpriteComponent>(id);
    char b[256]; std::strncpy(b,s.texture_path.c_str(),sizeof(b));
    if (ImGui::InputText("Texture##st",b,sizeof(b))){s.texture_path=b;s.texture=nullptr;}
    float c[4]={s.color.r/255.f,s.color.g/255.f,s.color.b/255.f,s.color.a/255.f};
    if(ImGui::ColorEdit4("Color##sc",c))
        s.color={uint8_t(c[0]*255),uint8_t(c[1]*255),uint8_t(c[2]*255),uint8_t(c[3]*255)};
    ImGui::DragInt("Layer##sl",&s.layer,1,-10,20);
    ImGui::Checkbox("Visible",&s.visible); ImGui::SameLine();
    ImGui::Checkbox("FlipH", &s.flip_h);
}
void EditorTools::InspectRigidBody(EntityID id, Registry& r) {
    if (!r.HasComponent<RigidBodyComponent>(id)) return;
    if (!ImGui::CollapsingHeader("RigidBody")) return;
    auto& rb=r.GetComponent<RigidBodyComponent>(id);
    ImGui::DragFloat("Vx",&rb.velocity_x,1.f);ImGui::DragFloat("Vy",&rb.velocity_y,1.f);
    ImGui::DragFloat("Mass",&rb.mass,0.1f,0.01f,100.f);
    ImGui::DragFloat("GravScale",&rb.gravity_scale,0.05f,0.f,5.f);
    ImGui::Checkbox("Kinematic",&rb.is_kinematic);
}
void EditorTools::InspectCollider(EntityID id, Registry& r) {
    if (!r.HasComponent<ColliderComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Collider")) return;
    auto& c=r.GetComponent<ColliderComponent>(id);
    ImGui::DragFloat("OfsX",&c.offset_x,1.f);ImGui::DragFloat("OfsY",&c.offset_y,1.f);
    ImGui::DragFloat("W",&c.width,1.f,1.f,2000.f);ImGui::DragFloat("H",&c.height,1.f,1.f,2000.f);
    ImGui::Checkbox("Trigger",&c.is_trigger);
}
void EditorTools::InspectPlayerTag(EntityID id, Registry& r) {
    if (!r.HasComponent<PlayerTagComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Player")) return;
    auto& p=r.GetComponent<PlayerTagComponent>(id);
    ImGui::DragFloat("Speed",&p.move_speed,5.f,0.f,1000.f);
    ImGui::DragFloat("JumpForce",&p.jump_force,5.f,-2000.f,0.f);
    ImGui::DragInt("MaxJumps",&p.max_jump_count,1,1,5);
}
void EditorTools::InspectCollectible(EntityID id, Registry& r) {
    if (!r.HasComponent<CollectibleComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Collectible")) return;
    auto& c=r.GetComponent<CollectibleComponent>(id);
    ImGui::DragInt("Points",&c.point_value,1,0,9999);
    char b[128]; std::strncpy(b,c.label.c_str(),sizeof(b));
    if(ImGui::InputText("Label##cl",b,sizeof(b))) c.label=b;
    ImGui::Checkbox("Collected",&c.collected);
}
void EditorTools::InspectTextLabel(EntityID id, Registry& r) {
    if (!r.HasComponent<TextLabelComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Text Label")) return;
    auto& t=r.GetComponent<TextLabelComponent>(id);
    char b[512]; std::strncpy(b,t.text.c_str(),sizeof(b));
    if(ImGui::InputText("Text##lt",b,sizeof(b))) t.text=b;
    ImGui::DragInt("Size##lts",&t.font_size,1,6,128);
}
void EditorTools::InspectPlatform(EntityID id, Registry& r) {
    if (!r.HasComponent<PlatformComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Platform")) return;
    ImGui::Checkbox("One-Way",&r.GetComponent<PlatformComponent>(id).is_one_way);
}
void EditorTools::InspectInfoZone(EntityID id, Registry& r) {
    if (!r.HasComponent<InfoZoneComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Info Zone")) return;
    auto& z=r.GetComponent<InfoZoneComponent>(id);
    char b[512]; std::strncpy(b,z.text.c_str(),sizeof(b));
    if(ImGui::InputTextMultiline("Text##izt",b,sizeof(b),ImVec2(-1,70))) z.text=b;
    ImGui::Checkbox("Repeat",&z.repeat);
    if(ImGui::SmallButton("Reset")) z.triggered=false;
}
void EditorTools::InspectEnemy(EntityID id, Registry& r) {
    if (!r.HasComponent<EnemyComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Enemy")) return;
    auto& e=r.GetComponent<EnemyComponent>(id);
    ImGui::DragFloat("PatrolMinX",&e.patrol_min_x,1.f);
    ImGui::DragFloat("PatrolMaxX",&e.patrol_max_x,1.f);
    ImGui::DragFloat("Speed##esp",&e.speed,1.f,0.f,800.f);
}
void EditorTools::InspectGoal(EntityID id, Registry& r) {
    if (!r.HasComponent<GoalComponent>(id)) return;
    if (!ImGui::CollapsingHeader("Goal")) return;
    ImGui::Checkbox("Reached",&r.GetComponent<GoalComponent>(id).reached);
}

void EditorTools::RenderAddComponentMenu(EntityID id, Registry& r) {
    if (!ImGui::BeginCombo("##ac","[ + Add Component ]")) return;
#define ADDIF(lbl,T) if(!r.HasComponent<T>(id)) if(ImGui::Selectable(lbl)) r.AddComponent<T>(id,T{});
    ADDIF("Transform",      TransformComponent)
    ADDIF("Sprite",         SpriteComponent)
    ADDIF("RigidBody",      RigidBodyComponent)
    ADDIF("Collider",       ColliderComponent)
    ADDIF("Platform",       PlatformComponent)
    ADDIF("Player Tag",     PlayerTagComponent)
    ADDIF("Collectible",    CollectibleComponent)
    ADDIF("Text Label",     TextLabelComponent)
    ADDIF("Info Zone",      InfoZoneComponent)
    ADDIF("Enemy",          EnemyComponent)
    ADDIF("Goal",           GoalComponent)
#undef ADDIF
    ImGui::EndCombo();
}

} // namespace EduEngine
