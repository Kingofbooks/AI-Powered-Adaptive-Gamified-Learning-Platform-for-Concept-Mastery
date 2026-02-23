// =============================================================================
// AdaptiveController.h — Mastery-Based Game Path Branching
// =============================================================================
// The AdaptiveController sits between the ReflectionSystem (which produces
// a Mastery Score 1–10) and the SceneLoader (which loads scenes).
//
// Branching rules:
//
//   Score ≥ MASTERY_THRESHOLD (7)  →  Advancement path
//     • Award full reflection points.
//     • Unlock subject-specific badge.
//     • (Optionally) load the next scene.
//
//   Score < MASTERY_THRESHOLD      →  Reinforcement path
//     • Award partial points.
//     • Flag the concept as "needs review".
//     • Load the simplified Reinforcement Module scene.
//
// The controller is intentionally stateless between calls so it can be
// serialised or replaced without touching the rest of the engine.
// =============================================================================
#pragma once

#include "../core/ecs/Registry.h"

#include <string>
#include <unordered_map>
#include <functional>

namespace EduEngine {
    class RewardEconomy;
    class SceneLoader;
}

namespace EduEngine {

// ---------------------------------------------------------------------------
// BranchOutcome — result of a ProcessMasteryScore() call (useful for testing
// and for the editor's debug panel).
// ---------------------------------------------------------------------------
enum class BranchOutcome : uint8_t {
    Advancement,    ///< Student understood the concept.
    Reinforcement,  ///< Student needs extra practice.
    Neutral         ///< Score was 0 (skipped) — no branch taken.
};

// ---------------------------------------------------------------------------
// SubjectConfig — per-subject scene paths and badge ids.
// Registered at startup; loaded from config or hardcoded for defaults.
// ---------------------------------------------------------------------------
struct SubjectConfig {
    std::string subject;
    std::string reinforcement_scene;    ///< Simplified scene to reload.
    std::string advancement_scene;      ///< Next / harder scene.
    std::string mastery_badge_id;       ///< Unlocked on advancement.
    int         base_point_reward = 50; ///< Scaled by mastery score.
};

// ---------------------------------------------------------------------------
// AdaptiveController
// ---------------------------------------------------------------------------
class AdaptiveController {
public:
    /// The engine passes a reference to RewardEconomy so the controller
    /// can award points and badges immediately.
    explicit AdaptiveController(RewardEconomy& rewards);

    // -----------------------------------------------------------------------
    // Core API — called by Engine via the ReflectionSystem callback.
    // -----------------------------------------------------------------------

    BranchOutcome ProcessMasteryScore(int                score,
                                      const std::string& subject,
                                      Registry&          registry,
                                      SceneLoader&       loader);

    // -----------------------------------------------------------------------
    // Configuration
    // -----------------------------------------------------------------------

    /// Register a subject configuration (must be called before any score arrives).
    void RegisterSubject(SubjectConfig config);

    /// Override the global mastery threshold.
    void SetMasteryThreshold(int threshold) noexcept { m_threshold = threshold; }

    [[nodiscard]] int GetMasteryThreshold() const noexcept { return m_threshold; }

    /// Provide a callback to be notified of every branch decision.
    using OnBranchCallback = std::function<void(BranchOutcome, int score)>;
    void SetOnBranchCallback(OnBranchCallback cb) { m_on_branch = std::move(cb); }

    // -----------------------------------------------------------------------
    // Diagnostics
    // -----------------------------------------------------------------------

    [[nodiscard]] BranchOutcome GetLastOutcome() const noexcept { return m_last_outcome; }
    [[nodiscard]] int           GetLastScore()   const noexcept { return m_last_score;   }

private:
    void HandleAdvancement(const SubjectConfig& cfg, int score);
    void HandleReinforcement(const SubjectConfig& cfg, int score,
                             Registry& registry, SceneLoader& loader);

    RewardEconomy& m_rewards;
    int            m_threshold    = 7;     ///< Minimum score for advancement.
    BranchOutcome  m_last_outcome = BranchOutcome::Neutral;
    int            m_last_score   = 0;

    // subject name → configuration
    std::unordered_map<std::string, SubjectConfig> m_subjects;
    OnBranchCallback m_on_branch;

    // Default fallback config used when the subject is not registered.
    SubjectConfig m_default_config{
        "General",
        "data/scenes/reinforcement_general.json",
        "data/scenes/next_scene.json",
        "photosynthesis_1",
        50
    };
};

} // namespace EduEngine
