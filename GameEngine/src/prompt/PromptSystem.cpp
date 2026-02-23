// =============================================================================
// PromptSystem.cpp  —  Mock generator + API scaffolding
// =============================================================================
#include "PromptSystem.h"
#include "../audio/AudioPlayer.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

using json = nlohmann::json;

namespace EduEngine {

// =============================================================================
// Destructor — join background thread.
// =============================================================================
PromptSystem::~PromptSystem() {
    if (m_thread.joinable()) m_thread.join();
}

// =============================================================================
// Generate — kick off async work.
// =============================================================================
void PromptSystem::Generate(const PromptConfig& config) {
    if (m_status.load() == 1 /*Running*/) return; // Don't stack requests.
    if (m_thread.joinable()) m_thread.join();

    m_status.store(1); // Running
    m_error.clear();

    m_thread = std::thread([this, config]() {
        try {
            std::string result;
            if (m_mock_mode) {
                // Simulate a brief thinking delay so the spinner is visible.
                std::this_thread::sleep_for(std::chrono::milliseconds(900));
                result = GenerateMock(config);
            } else {
                result = CallApi(config);
            }
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_result_json = std::move(result);
            }
            m_status.store(2); // Done
        } catch (const std::exception& e) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_error = e.what();
            m_status.store(3); // Error
        }
    });
}

// =============================================================================
GenerationStatus PromptSystem::GetStatus() const noexcept {
    switch (m_status.load()) {
        case 1:  return GenerationStatus::Running;
        case 2:  return GenerationStatus::Done;
        case 3:  return GenerationStatus::Error;
        default: return GenerationStatus::Idle;
    }
}

void PromptSystem::Reset() noexcept {
    m_status.store(0);
    m_error.clear();
}

// =============================================================================
// SaveToFile
// =============================================================================
std::string PromptSystem::SaveToFile(const std::string& output_dir) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_result_json.empty()) return {};

    // Parse to pretty-print.
    json doc = json::parse(m_result_json);
    std::string scene_id = doc["scene_meta"].value("id", "generated_scene");

    // Sanitise ID for file use.
    for (char& c : scene_id)
        if (!std::isalnum(c) && c != '_') c = '_';

    const std::string path = output_dir + scene_id + ".json";
    std::ofstream f(path);
    if (!f.is_open()) {
        std::cerr << "[PromptSystem] Cannot write to: " << path << '\n';
        return {};
    }
    f << doc.dump(2);
    std::cout << "[PromptSystem] Scene saved: " << path << '\n';
    return path;
}

// =============================================================================
// ThemeSlug  —  maps display name → generate_assets.py key
// =============================================================================
std::string PromptSystem::ThemeSlug(const std::string& name) {
    if (name.find("photo")   != std::string::npos) return "photosynthesis";
    if (name.find("space")   != std::string::npos ||
        name.find("solar")   != std::string::npos) return "space";
    if (name.find("water")   != std::string::npos) return "water_cycle";
    if (name.find("math")    != std::string::npos) return "mathematics";
    if (name.find("hist")    != std::string::npos) return "history";
    if (name.find("chem")    != std::string::npos) return "chemistry";
    if (name.find("phys")    != std::string::npos) return "physics";
    if (name.find("bio")     != std::string::npos) return "biology";
    if (name.find("geo")     != std::string::npos) return "geography";
    return "default";
}

// =============================================================================
// GenerateMock
// =============================================================================
std::string PromptSystem::GenerateMock(const PromptConfig& cfg) const {
    return BuildScene(cfg).dump(2);
}

// =============================================================================
// BuildScene  —  the heart of the procedural generator.
// =============================================================================
json PromptSystem::BuildScene(const PromptConfig& cfg) const {
    SubjectTheme theme = DetectTheme(cfg.prompt + " " + cfg.subject);

    // Determine the asset slug and generate sprites + audio
    // (non-fatal if Python missing).
    m_current_slug = ThemeSlug(theme.name);
    {
        const std::string cmd = "python3 generate_assets.py --theme "
                              + m_current_slug + " 2>/dev/null";
        int ret = std::system(cmd.c_str());
        if (ret != 0)
            std::cerr << "[PromptSystem] Sprite generation skipped "
                         "(python3/Pillow not available — coloured rects used)\n";
    }
    {
        const std::string cmd = "python3 generate_audio.py --theme "
                              + m_current_slug + " 2>/dev/null";
        int ret = std::system(cmd.c_str());
        if (ret != 0)
            std::cerr << "[PromptSystem] Audio generation skipped "
                         "(python3 not available — running silently)\n";
        else
            AudioPlayer::LoadTheme(m_current_slug);
    }

    // How many platforms / enemies scales with grade.
    const int num_platforms  = std::clamp(3 + (cfg.grade / 3), 3, 7);
    const int num_collectibles = std::min(static_cast<int>(theme.collectibles.size()), num_platforms + 1);
    const int num_enemies    = (cfg.grade >= 5) ? std::min(cfg.grade / 3, 3) : 0;
    const int num_info_zones = std::min(static_cast<int>(theme.facts.size()), 3);

    // Scene ID derived from prompt.
    std::string raw_id = cfg.subject.empty() ? theme.name : cfg.subject;
    std::string scene_id = raw_id + "_grade" + std::to_string(cfg.grade);
    std::transform(scene_id.begin(), scene_id.end(), scene_id.begin(),
        [](unsigned char c){ return std::tolower(c); });
    for (char& c : scene_id) if (!std::isalnum(c)) c = '_';

    // Scene title (capitalise first letter of each word).
    std::string title = theme.name;
    if (!title.empty()) title[0] = static_cast<char>(std::toupper(
        static_cast<unsigned char>(title[0])));
    title += " — Grade " + std::to_string(cfg.grade);

    json scene;
    scene["scene_meta"] = {
        {"id",        scene_id},
        {"title",     title},
        {"subject",   theme.name},
        {"grade",     std::to_string(cfg.grade)},
        {"ai_prompt", cfg.prompt}
    };
    scene["world_settings"] = {
        {"gravity",          980.0},
        {"background_color", {theme.bg_r, theme.bg_g, theme.bg_b, 255}}
    };

    json entities = json::array();

    // ── Mandatory entities ─────────────────────────────────────────────────
    entities.push_back(MakeBackground(theme));   // layer 0, no collider
    entities.push_back(MakePlayer(theme));
    entities.push_back(MakeGround(theme));

    // ── Platforms ──────────────────────────────────────────────────────────
    for (int i = 0; i < num_platforms; ++i)
        entities.push_back(MakePlatform(theme, i, num_platforms));

    // ── Collectibles ───────────────────────────────────────────────────────
    for (int i = 0; i < num_collectibles; ++i)
        entities.push_back(MakeCollectible(theme, i));

    // ── Info zones ─────────────────────────────────────────────────────────
    for (int i = 0; i < num_info_zones; ++i)
        entities.push_back(MakeInfoZone(theme, i));

    // ── Enemies ────────────────────────────────────────────────────────────
    for (int i = 0; i < num_enemies; ++i)
        entities.push_back(MakeEnemy(i));

    // ── Goal ───────────────────────────────────────────────────────────────
    entities.push_back(MakeGoal(theme));

    scene["entities"] = entities;
    return scene;
}

// =============================================================================
// DetectTheme
// =============================================================================
PromptSystem::SubjectTheme PromptSystem::DetectTheme(const std::string& prompt) const {
    std::string low = prompt;
    std::transform(low.begin(), low.end(), low.begin(),
        [](unsigned char c){ return std::tolower(c); });

    // ── Subject themes ──────────────────────────────────────────────────────
    // Each has: name, collectibles (pickup labels), facts (info-zone texts),
    //           background RGB, platform RGB, collectible RGB.

    if (low.find("photosyn") != std::string::npos ||
        low.find("chloro")   != std::string::npos) {
        return {"photosynthesis",
            {"CO₂", "H₂O", "Sunlight", "Glucose", "O₂", "Chlorophyll"},
            {"Photosynthesis converts light energy into glucose.",
             "CO₂ enters leaves through tiny pores called stomata.",
             "Chlorophyll gives plants their green colour.",
             "Oxygen is released as a by-product of photosynthesis."},
            12, 28, 12,    // dark green bg
            30, 90, 30,    // green platforms
            80, 220, 80};  // bright green collectibles
    }
    if (low.find("solar") != std::string::npos ||
        low.find("planet") != std::string::npos ||
        low.find("space")  != std::string::npos ||
        low.find("astro")  != std::string::npos) {
        return {"space & solar system",
            {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Star"},
            {"The Sun is a medium-sized star at the centre of our solar system.",
             "Mars has the largest volcano in the solar system: Olympus Mons.",
             "Jupiter is so large that all other planets could fit inside it.",
             "Light from the Sun takes 8 minutes to reach Earth."},
            5, 5, 25,       // deep space bg
            40, 40, 100,    // dark blue platforms
            220, 180, 60};  // golden collectibles
    }
    if (low.find("water cycle") != std::string::npos ||
        low.find("evapor")      != std::string::npos ||
        low.find("precipit")    != std::string::npos) {
        return {"water cycle",
            {"Evaporation", "Condensation", "Precipitation", "Runoff", "Transpiration"},
            {"Water evaporates from the ocean surface due to solar energy.",
             "Water vapour condenses into clouds as it rises and cools.",
             "Precipitation returns water to Earth as rain, snow, or hail.",
             "Plants release water vapour through transpiration."},
            10, 20, 50,     // deep blue bg
            30, 60, 120,    // blue platforms
            100, 180, 255}; // light blue collectibles
    }
    if (low.find("math") != std::string::npos ||
        low.find("algebra") != std::string::npos ||
        low.find("geometr") != std::string::npos ||
        low.find("fraction") != std::string::npos) {
        return {"mathematics",
            {"π", "∑", "∫", "√", "2²", "x²", "Δ"},
            {"Pi (π) ≈ 3.14159 is the ratio of a circle's circumference to its diameter.",
             "The Pythagorean theorem: a² + b² = c².",
             "A prime number is only divisible by 1 and itself.",
             "Zero was invented in ancient India around 500 CE."},
            20, 15, 30,     // dark purple bg
            60, 40, 100,    // purple platforms
            255, 200, 50};  // gold collectibles
    }
    if (low.find("history") != std::string::npos ||
        low.find("ancient") != std::string::npos ||
        low.find("civil")   != std::string::npos ||
        low.find("war")     != std::string::npos) {
        return {"history",
            {"Scroll", "Crown", "Shield", "Sword", "Map", "Coin"},
            {"Ancient Egypt built the Great Pyramid around 2560 BCE.",
             "The Roman Empire lasted from 27 BCE to 476 CE.",
             "The Renaissance was a cultural movement from the 14th to 17th century.",
             "The Industrial Revolution began in Britain around 1760."},
            30, 20, 10,     // dark brown bg
            80, 55, 25,     // brown platforms
            200, 160, 60};  // bronze collectibles
    }
    if (low.find("chemistry") != std::string::npos ||
        low.find("element")   != std::string::npos ||
        low.find("atom")      != std::string::npos ||
        low.find("molecule")  != std::string::npos) {
        return {"chemistry",
            {"H₂O", "O₂", "NaCl", "CO₂", "Fe", "Au", "Atom"},
            {"The periodic table has 118 confirmed elements.",
             "An atom consists of protons, neutrons, and electrons.",
             "Water (H₂O) is called a universal solvent.",
             "Gold (Au) is a noble metal that does not rust."},
            10, 5, 25,      // deep purple bg
            60, 20, 80,     // purple platforms
            120, 220, 180}; // teal collectibles
    }
    if (low.find("physics") != std::string::npos ||
        low.find("force")   != std::string::npos ||
        low.find("energy")  != std::string::npos ||
        low.find("gravity") != std::string::npos) {
        return {"physics",
            {"Force", "Energy", "Mass", "Velocity", "Gravity", "Wave"},
            {"Newton's First Law: an object at rest stays at rest.",
             "E = mc² describes mass-energy equivalence (Einstein).",
             "Gravity accelerates objects at 9.8 m/s² on Earth.",
             "Sound travels at approximately 343 m/s in air."},
            5, 10, 30,      // dark blue bg
            20, 50, 120,    // blue platforms
            60, 180, 255};  // electric blue collectibles
    }
    if (low.find("biology") != std::string::npos ||
        low.find("cell")    != std::string::npos ||
        low.find("dna")     != std::string::npos ||
        low.find("animal")  != std::string::npos) {
        return {"biology",
            {"DNA", "Cell", "Protein", "Gene", "Enzyme", "Nucleus"},
            {"The human body has about 37 trillion cells.",
             "DNA is a double helix made of base pairs: A-T and G-C.",
             "The cell membrane controls what enters and leaves the cell.",
             "Mitosis is cell division that produces two identical daughter cells."},
            10, 25, 15,     // dark green bg
            25, 70, 35,     // forest green platforms
            100, 240, 130}; // bright green collectibles
    }
    if (low.find("geograph") != std::string::npos ||
        low.find("continent") != std::string::npos ||
        low.find("climate")   != std::string::npos) {
        return {"geography",
            {"Africa", "Asia", "Europe", "Amazon", "Equator", "Mountain"},
            {"Earth has 7 continents and 5 major oceans.",
             "The Amazon rainforest covers over 5.5 million km².",
             "The Sahara is the world's largest hot desert.",
             "The Mariana Trench is the deepest point on Earth at ~11 km."},
            5, 20, 40,      // ocean blue bg
            20, 80, 40,     // land green platforms
            60, 180, 100};  // green collectibles
    }

    // ── Default / generic theme ─────────────────────────────────────────────
    return {"learning adventure",
        {"⭐", "💡", "📚", "🔬", "🏆", "🎯"},
        {"Keep exploring and never stop asking questions!",
         "Every expert was once a beginner.",
         "Learning is the most powerful tool you have.",
         "Curiosity is the engine of discovery."},
        18, 12, 30,     // dark purple bg
        55, 35, 90,     // purple platforms
        200, 120, 255}; // lavender collectibles
}

// =============================================================================
// Entity builders
// =============================================================================

// Shorthand: asset path for current theme.
static std::string asset(const std::string& slug, const std::string& file) {
    return "assets/images/" + slug + "/" + file;
}

json PromptSystem::MakeBackground(const SubjectTheme& /*t*/) const {
    // Full-screen backdrop at layer 0, no collider.
    return {
        {"name", "Background"},
        {"tags", json::array({"background"})},
        {"components", {
            {"transform", {{"x", 0}, {"y", 0}, {"scale_x", 1.0}, {"scale_y", 1.0}}},
            {"sprite",    {{"texture", asset(m_current_slug, "background.png")},
                           {"color", {255, 255, 255, 255}}, {"layer", 0}}}
        }}
    };
}

json PromptSystem::MakePlayer(const SubjectTheme& /*t*/) const {
    // player.png is 24×32; scale 1.5 → renders 36×48, matches collider display.
    return {
        {"name", "Player"},
        {"tags", json::array({"player"})},
        {"components", {
            {"transform",  {{"x", 80}, {"y", 520}, {"scale_x", 1.5}, {"scale_y", 1.5}}},
            {"sprite",     {{"texture", asset(m_current_slug, "player.png")},
                            {"color", {255, 255, 255, 255}}, {"layer", 5}}},
            {"rigid_body", {{"mass", 1.0}, {"gravity_scale", 1.0}}},
            {"collider",   {{"offset_x", 4}, {"width", 24}, {"height", 32}}},
            {"player_tag", {{"move_speed", 220}, {"jump_force", -580}, {"max_jumps", 2}}}
        }}
    };
}

json PromptSystem::MakeGround(const SubjectTheme& t) const {
    // platform.png is 32×20; scale to fill 1280×100.
    return {
        {"name", "Ground"},
        {"tags", json::array({"platform"})},
        {"components", {
            {"transform", {{"x", 0}, {"y", 620}, {"scale_x", 40.0}, {"scale_y", 5.0}}},
            {"sprite",    {{"texture", asset(m_current_slug, "platform.png")},
                           {"color", {t.plat_r, t.plat_g, t.plat_b, 255}}, {"layer", 2}}},
            {"collider",  {{"width", 1280}, {"height", 100}}},
            {"platform",  {{"is_one_way", false}}}
        }}
    };
}

json PromptSystem::MakePlatform(const SubjectTheme& t, int idx, int total) const {
    const float spacing   = 1100.f / static_cast<float>(total);
    const float x         = 120.f + idx * spacing;
    const float y_options[] = {480.f, 400.f, 330.f, 450.f, 370.f, 310.f, 420.f};
    const float y         = y_options[idx % 7];
    const float widths[]  = {160.f, 140.f, 180.f, 130.f, 170.f, 150.f, 190.f};
    const float w         = widths[idx % 7];
    // platform.png is 32×20; scale_x stretches it to the collider width.
    const float scale_x   = w / 32.f;

    return {
        {"name", "Platform_" + std::to_string(idx + 1)},
        {"tags", json::array({"platform"})},
        {"components", {
            {"transform", {{"x", x}, {"y", y}, {"scale_x", scale_x}, {"scale_y", 1.0}}},
            {"sprite",    {{"texture", asset(m_current_slug, "platform.png")},
                           {"color", {t.plat_r, t.plat_g, t.plat_b, 255}}, {"layer", 2}}},
            {"collider",  {{"width", w}, {"height", 20}}},
            {"platform",  {{"is_one_way", true}}}
        }}
    };
}

json PromptSystem::MakeCollectible(const SubjectTheme& t, int idx) const {
    const std::string& label   = t.collectibles[idx % t.collectibles.size()];
    const float spacing        = 1100.f / static_cast<float>(t.collectibles.size() + 1);
    const float x              = 150.f + idx * spacing;
    const float y_options[]    = {360.f, 280.f, 440.f, 310.f, 390.f};
    const float y              = y_options[idx % 5];
    const std::string tex      = asset(m_current_slug,
                                       "collectible_" + std::to_string(idx) + ".png");
    return {
        {"name", "Collect_" + label},
        {"tags", json::array({"collectible"})},
        {"components", {
            {"transform",   {{"x", x}, {"y", y}}},
            {"sprite",      {{"texture", tex},
                             {"color", {t.coll_r, t.coll_g, t.coll_b, 220}}, {"layer", 4}}},
            {"collider",    {{"width", 28}, {"height", 28}, {"is_trigger", true}}},
            {"collectible", {{"point_value", 15}, {"label", label}}},
            {"text_label",  {{"text", label}, {"color", {255, 255, 200, 255}},
                             {"font_size", 13}, {"layer", 6}}}
        }}
    };
}

json PromptSystem::MakeInfoZone(const SubjectTheme& t, int idx) const {
    const std::string& fact = t.facts[idx % t.facts.size()];
    const float x_options[] = {380.f, 680.f, 950.f};
    const float x           = x_options[idx % 3];

    return {
        {"name", "InfoSign_" + std::to_string(idx + 1)},
        {"tags", json::array({"info"})},
        {"components", {
            {"transform",  {{"x", x}, {"y", 590.f}}},
            {"sprite",     {{"color", {220, 180, 60, 220}}, {"layer", 3}}},
            {"collider",   {{"width", 36}, {"height", 36}, {"is_trigger", true}}},
            {"text_label", {{"text", "?"}, {"color", {30, 20, 5, 255}},
                            {"font_size", 20}, {"layer", 5}}},
            {"info_zone",  {{"text", fact}, {"repeat", false}}}
        }}
    };
}

json PromptSystem::MakeEnemy(int idx) const {
    const float x_options[]  = {400.f, 700.f, 950.f};
    const float x            = x_options[idx % 3];
    const float patrol_width = 150.f + idx * 30.f;

    return {
        {"name", "Enemy_" + std::to_string(idx + 1)},
        {"tags", json::array({"enemy"})},
        {"components", {
            {"transform",  {{"x", x}, {"y", 588.f}}},
            {"sprite",     {{"texture", asset(m_current_slug, "enemy.png")},
                            {"color", {255, 255, 255, 230}}, {"layer", 4}}},
            {"collider",   {{"width", 30}, {"height", 32}}},
            {"enemy",      {{"patrol_min_x", x - 20.f},
                            {"patrol_max_x", x + patrol_width},
                            {"speed", 70.f + idx * 15.f}}}
        }}
    };
}

json PromptSystem::MakeGoal(const SubjectTheme& t) const {
    return {
        {"name", "Goal_Flag"},
        {"tags", json::array({"goal"})},
        {"components", {
            {"transform", {{"x", 1160.f}, {"y", 558.f}}},
            {"sprite",    {{"texture", asset(m_current_slug, "goal.png")},
                           {"color", {t.coll_r, t.coll_g, t.coll_b, 255}}, {"layer", 4}}},
            {"collider",  {{"width", 40}, {"height", 60}, {"is_trigger", true}}},
            {"goal",      {}},
            {"text_label",{{"text", "GOAL"}, {"color", {255, 255, 255, 255}}, {"layer", 6}}}
        }}
    };
}

// =============================================================================
// CallApi  —  delegates to call_claude_api.py via Agno (Python AI agent).
//
// Requirements (one-time setup):
//   pip install agno anthropic
//   export ANTHROPIC_API_KEY="sk-ant-..."
//
// To swap in a custom / friend's model, edit call_claude_api.py — the C++
// side here never changes.
// =============================================================================
std::string PromptSystem::CallApi(const PromptConfig& cfg) const {
    // Write config to a temp file to avoid any shell-injection risk.
    const std::string cfg_path = "/tmp/edu_prompt_cfg.json";
    {
        json cfg_json = {
            {"prompt",  cfg.prompt},
            {"subject", cfg.subject},
            {"grade",   cfg.grade}
        };
        std::ofstream f(cfg_path);
        if (!f.is_open())
            throw std::runtime_error("Cannot write temp config: " + cfg_path);
        f << cfg_json.dump();
    }

    // Run the Python bridge and capture stdout.
    const std::string cmd = "python3 call_claude_api.py " + cfg_path + " 2>/dev/null";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("[PromptSystem] popen failed for call_claude_api.py");

    std::string result;
    char buf[4096];
    while (fgets(buf, sizeof(buf), pipe))
        result += buf;
    int exit_code = pclose(pipe);

    if (exit_code != 0 || result.empty())
        throw std::runtime_error(
            "[PromptSystem] call_claude_api.py failed (exit " +
            std::to_string(exit_code) + ").\n"
            "Check: ANTHROPIC_API_KEY is set, and run: pip install agno anthropic");

    // Validate the JSON before returning (throws if malformed).
    auto parsed = json::parse(result);  // throws if malformed
    return parsed.dump();               // re-serialise for consistent formatting
}

} // namespace EduEngine
