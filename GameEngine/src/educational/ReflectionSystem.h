// =============================================================================
// ReflectionSystem.h — Student Reflection & AI Mastery Scoring
// =============================================================================
// The "Reflection State" is the educational pause point of the engine.
// When a student enters an EducationalTrigger zone, this system:
//
//   1. PAUSES the game world (Engine checks IsActive()).
//   2. Overlays a full-screen ImGui window showing the question.
//   3. Accepts free-form text input from the student.
//   4. Sends the answer to an external AI API via HTTP (using libcurl or
//      a mock implementation for offline environments).
//   5. Receives a Mastery Score (1–10).
//   6. Fires an OnScoreCallback so the AdaptiveController can react.
//   7. RESUMES the game world.
//
// State machine:
//
//   Inactive ──TriggerReflection()──► AwaitingInput
//                                            │
//                                    Student submits answer
//                                            │
//                                            ▼
//                                     AwaitingScore   (async HTTP)
//                                            │
//                                    Score received
//                                            │
//                                            ▼
//                                         Complete ──► Inactive
// =============================================================================
#pragma once

#include "../core/ecs/Registry.h"

#include <SDL2/SDL.h>
#include <functional>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

namespace EduEngine {

// ---------------------------------------------------------------------------
// States of the reflection state machine.
// ---------------------------------------------------------------------------
enum class ReflectionState : uint8_t {
    Inactive,       ///< Normal gameplay — no reflection active.
    AwaitingInput,  ///< Question displayed; student is typing.
    AwaitingScore,  ///< HTTP request sent; waiting for AI response.
    Scored,         ///< Score received; displaying result summary.
    Complete        ///< Transitioning back to Inactive.
};

// ---------------------------------------------------------------------------
// Callback type: invoked when the AI returns a mastery score.
//   score   — integer 1–10 (10 = perfect mastery).
//   subject — topic string for context ("Biology", "Photosynthesis", …).
// ---------------------------------------------------------------------------
using OnScoreCallback = std::function<void(int score, const std::string& subject)>;

// ---------------------------------------------------------------------------
// ReflectionSystem
// ---------------------------------------------------------------------------
class ReflectionSystem {
public:
    ReflectionSystem();
    ~ReflectionSystem();

    // Non-copyable (owns threads and mutex).
    ReflectionSystem(const ReflectionSystem&)            = delete;
    ReflectionSystem& operator=(const ReflectionSystem&) = delete;

    // -----------------------------------------------------------------------
    // Trigger — called by TriggerSystem when the player overlaps an
    // EducationalTriggerComponent zone.
    // -----------------------------------------------------------------------
    void TriggerReflection(const std::string& question,
                           const std::string& subject,
                           const std::string& hint,
                           int                point_reward,
                           int                attempts_allowed = 3);

    // -----------------------------------------------------------------------
    // Engine integration
    // -----------------------------------------------------------------------

    /// Forward SDL events (keyboard text input / backspace handling).
    void ProcessEvent(const SDL_Event& event);

    /// Update the state machine — call every frame (even when world is paused).
    void Update(float dt, Registry& registry);

    /// Render the full-screen reflection overlay via ImGui.
    void Render(SDL_Renderer* renderer);

    // -----------------------------------------------------------------------
    // Callbacks
    // -----------------------------------------------------------------------
    void SetOnScoreCallback(OnScoreCallback cb) { m_on_score = std::move(cb); }

    // -----------------------------------------------------------------------
    // Queries
    // -----------------------------------------------------------------------
    [[nodiscard]] bool            IsActive()       const noexcept;
    [[nodiscard]] ReflectionState GetState()       const noexcept { return m_state; }
    [[nodiscard]] int             GetLastScore()   const noexcept { return m_last_score; }
    [[nodiscard]] int             GetAttempts()    const noexcept { return m_attempts_used; }

    // -----------------------------------------------------------------------
    // API configuration — set your AI endpoint before the engine starts.
    // -----------------------------------------------------------------------
    void SetApiEndpoint(const std::string& url)    { m_api_url = url; }
    void SetApiKey(const std::string& key)         { m_api_key = key; }
    void SetMockMode(bool use_mock)                { m_mock_mode = use_mock; }

private:
    // ── Scoring logic ─────────────────────────────────────────────────────

    /// Dispatch an async HTTP POST to the AI API.
    void DispatchScoringRequest(const std::string& answer);

    /// Mock scorer — simulates a basic keyword match for offline use.
    int MockScore(const std::string& answer) const;

    /// Called (on the main thread) once the score arrives.
    void ApplyScore(int score);

    // ── UI helpers ────────────────────────────────────────────────────────
    void RenderQuestionUI();
    void RenderScoringUI();
    void RenderResultUI();

    // ── State ─────────────────────────────────────────────────────────────
    ReflectionState m_state         = ReflectionState::Inactive;

    // Current question context.
    std::string     m_question;
    std::string     m_subject;
    std::string     m_hint;
    int             m_point_reward  = 0;
    int             m_attempts_allowed = 3;
    int             m_attempts_used = 0;
    bool            m_hint_visible  = false;

    // Text input buffer (ImGui InputText).
    static constexpr int INPUT_BUF_SIZE = 1024;
    char            m_input_buf[INPUT_BUF_SIZE] = {};

    // Scoring result.
    int             m_last_score    = 0;
    std::string     m_ai_feedback;  ///< Optionally returned from API.

    // Timer for the result display phase.
    float           m_result_timer  = 0.f;
    static constexpr float RESULT_DISPLAY_SECONDS = 4.f;

    // Async score retrieval.
    std::thread     m_score_thread;
    std::atomic<bool> m_score_ready{false};
    std::mutex      m_score_mutex;
    int             m_pending_score = 0;
    std::string     m_pending_feedback;

    // API configuration.
    std::string     m_api_url;
    std::string     m_api_key;
    bool            m_mock_mode = true; ///< Default to mock (no network needed).

    OnScoreCallback m_on_score; ///< Fired when a valid score arrives.
};

} // namespace EduEngine
