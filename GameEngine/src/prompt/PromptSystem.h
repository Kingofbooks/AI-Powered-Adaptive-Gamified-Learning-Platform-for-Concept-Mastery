// =============================================================================
// PromptSystem.h  —  AI Prompt → Playable Scene Generator
// =============================================================================
// The teacher types a description like:
//   "A platformer about the water cycle for 6th grade"
//
// PromptSystem turns that into a complete, valid scene JSON and writes it to
// disk so SceneLoader can immediately load and run it.
//
// Two modes:
//   Mock (default, offline)  — keyword-based procedural generator.
//                              Instantly produces themed, playable scenes.
//   API (online)             — sends prompt to an LLM endpoint that returns
//                              the scene JSON directly.
//                              Set api_url + api_key before using.
// =============================================================================
#pragma once

#include <nlohmann/json.hpp>

#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace EduEngine {

// ---------------------------------------------------------------------------
// PromptConfig  —  parameters collected from the UI.
// ---------------------------------------------------------------------------
struct PromptConfig {
    std::string prompt;          ///< Free-text description from the teacher.
    std::string subject;         ///< Optional override (e.g. "photosynthesis").
    int         grade     = 6;   ///< 1-12, affects difficulty / entity count.
    std::string game_type = "platformer"; ///< "platformer" | "top-down" (future)
};

// ---------------------------------------------------------------------------
// GenerationStatus  —  polled by the editor each frame.
// ---------------------------------------------------------------------------
enum class GenerationStatus {
    Idle,
    Running,   ///< Background thread is working.
    Done,      ///< JSON ready — call GetLastResult().
    Error      ///< Call GetLastError() for the message.
};

// ---------------------------------------------------------------------------
// PromptSystem
// ---------------------------------------------------------------------------
class PromptSystem {
public:
    PromptSystem()  = default;
    ~PromptSystem();

    PromptSystem(const PromptSystem&)            = delete;
    PromptSystem& operator=(const PromptSystem&) = delete;

    // ── Generation ─────────────────────────────────────────────────────────

    /// Start async generation.  Returns immediately.
    /// Poll GetStatus() each frame, then call GetLastResult() when Done.
    void Generate(const PromptConfig& config);

    // ── Status polling ──────────────────────────────────────────────────────
    [[nodiscard]] GenerationStatus GetStatus()    const noexcept;
    [[nodiscard]] const std::string& GetLastResult() const noexcept { return m_result_json; }
    [[nodiscard]] const std::string& GetLastError()  const noexcept { return m_error; }

    // Reset to Idle after consuming the result.
    void Reset() noexcept;

    // ── API configuration ───────────────────────────────────────────────────
    void SetApiEndpoint(const std::string& url) { m_api_url = url; }
    void SetApiKey(const std::string& key)      { m_api_key = key; }
    void SetMockMode(bool v)                    { m_mock_mode = v; }
    [[nodiscard]] bool IsMockMode() const noexcept { return m_mock_mode; }

    // ── Save helper ─────────────────────────────────────────────────────────
    /// Write the last result JSON to disk.  Returns the saved path.
    std::string SaveToFile(const std::string& output_dir = "data/scenes/") const;

private:
    // ── Mock generator ──────────────────────────────────────────────────────
    std::string GenerateMock(const PromptConfig& cfg) const;
    nlohmann::json BuildScene(const PromptConfig& cfg) const;

    // Subject theme data.
    struct SubjectTheme {
        std::string          name;
        std::vector<std::string> collectibles; ///< Labels on pickup items.
        std::vector<std::string> facts;        ///< InfoZone popup texts.
        int bg_r, bg_g, bg_b;                  ///< Background colour.
        int plat_r, plat_g, plat_b;            ///< Platform colour.
        int coll_r, coll_g, coll_b;            ///< Collectible colour.
    };
    SubjectTheme DetectTheme(const std::string& prompt) const;

    // JSON entity builders.
    nlohmann::json MakeBackground(const SubjectTheme& t)                const;
    nlohmann::json MakePlayer(const SubjectTheme& t)                    const;
    nlohmann::json MakeGround(const SubjectTheme& t)                    const;
    nlohmann::json MakePlatform(const SubjectTheme& t, int idx, int total) const;
    nlohmann::json MakeCollectible(const SubjectTheme& t, int idx)      const;
    nlohmann::json MakeInfoZone(const SubjectTheme& t, int idx)         const;
    nlohmann::json MakeGoal(const SubjectTheme& t)                      const;
    nlohmann::json MakeEnemy(int idx)                                    const;

    // Converts the theme display name to the slug used by generate_assets.py.
    static std::string ThemeSlug(const std::string& theme_name);

    // Set by BuildScene() before any Make*() call so builders can reference it.
    mutable std::string m_current_slug;

    // ── API call ─────────────────────────────────────────────────────────
    std::string CallApi(const PromptConfig& cfg) const;

    // ── Async state ──────────────────────────────────────────────────────
    std::thread          m_thread;
    std::atomic<int>     m_status{0}; // 0=Idle 1=Running 2=Done 3=Error
    mutable std::mutex   m_mutex;
    std::string          m_result_json;
    std::string          m_error;

    // ── Config ────────────────────────────────────────────────────────────
    bool        m_mock_mode = true;
    std::string m_api_url;
    std::string m_api_key;
};

} // namespace EduEngine
