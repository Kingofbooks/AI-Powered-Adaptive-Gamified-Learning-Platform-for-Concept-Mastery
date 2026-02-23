// =============================================================================
// AdaptiveController.cpp
// =============================================================================
#include "AdaptiveController.h"
#include "RewardEconomy.h"
#include "../scene/SceneLoader.h"

#include <algorithm>
#include <iostream>

namespace EduEngine {

// =============================================================================
// Constructor
// =============================================================================

AdaptiveController::AdaptiveController(RewardEconomy& rewards)
    : m_rewards(rewards) {
    // Register default subject configurations.
    // In production, these come from a JSON config file or the AI back-end.
    RegisterSubject({
        "photosynthesis",
        "data/scenes/reinforce_photosynthesis.json",
        "data/scenes/photosynthesis_advanced.json",
        "photosynthesis_1",
        50
    });
    RegisterSubject({
        "biology",
        "data/scenes/reinforce_biology.json",
        "data/scenes/biology_advanced.json",
        "first_answer",
        40
    });
}

// =============================================================================
// RegisterSubject
// =============================================================================

void AdaptiveController::RegisterSubject(SubjectConfig config) {
    // Normalise key to lowercase for case-insensitive lookup.
    std::string key = config.subject;
    std::transform(key.begin(), key.end(), key.begin(),
        [](unsigned char c){ return std::tolower(c); });
    m_subjects[key] = std::move(config);
}

// =============================================================================
// ProcessMasteryScore — main entry point
// =============================================================================

BranchOutcome AdaptiveController::ProcessMasteryScore(int                score,
                                                       const std::string& subject,
                                                       Registry&          registry,
                                                       SceneLoader&       loader) {
    m_last_score = score;

    if (score == 0) {
        // Skipped answer — no branching, but record the attempt.
        m_last_outcome = BranchOutcome::Neutral;
        m_rewards.RecordMasteryScore(0);
        std::cout << "[Adaptive] Question skipped. No branch taken.\n";
        if (m_on_branch) m_on_branch(BranchOutcome::Neutral, score);
        return BranchOutcome::Neutral;
    }

    // Normalise subject for map lookup.
    std::string key = subject;
    std::transform(key.begin(), key.end(), key.begin(),
        [](unsigned char c){ return std::tolower(c); });

    const SubjectConfig& cfg = m_subjects.count(key)
        ? m_subjects.at(key)
        : m_default_config;

    // Record score in the reward economy.
    m_rewards.RecordMasteryScore(score);

    // Branch decision.
    if (score >= m_threshold) {
        HandleAdvancement(cfg, score);
        m_last_outcome = BranchOutcome::Advancement;
    } else {
        HandleReinforcement(cfg, score, registry, loader);
        m_last_outcome = BranchOutcome::Reinforcement;
    }

    if (m_on_branch) m_on_branch(m_last_outcome, score);
    return m_last_outcome;
}

// =============================================================================
// Advancement path
// =============================================================================

void AdaptiveController::HandleAdvancement(const SubjectConfig& cfg, int score) {
    // Award points scaled to the mastery score.
    const int points = RewardEconomy::CalculateReflectionPoints(cfg.base_point_reward, score);
    m_rewards.AddPoints(points);

    // Unlock subject mastery badge.
    if (!cfg.mastery_badge_id.empty()) {
        m_rewards.UnlockBadge(cfg.mastery_badge_id);
    }

    std::cout << "[Adaptive] ADVANCEMENT path — score " << score
              << "/10 | +" << points << " pts | badge: "
              << cfg.mastery_badge_id << '\n';

    // Note: actual scene loading (advancement_scene) is driven by the
    // game's narrative logic, not forced here, to avoid interrupting the
    // player mid-session.  The Engine can poll GetLastOutcome() to decide.
}

// =============================================================================
// Reinforcement path
// =============================================================================

void AdaptiveController::HandleReinforcement(const SubjectConfig& cfg,
                                              int score,
                                              Registry&    registry,
                                              SceneLoader& loader) {
    // Award partial points.
    const int points = RewardEconomy::CalculateReflectionPoints(cfg.base_point_reward, score);
    m_rewards.AddPoints(points);

    std::cout << "[Adaptive] REINFORCEMENT path — score " << score
              << "/10 | +" << points << " pts\n"
              << "           Loading reinforcement module: "
              << cfg.reinforcement_scene << '\n';

    // Trigger the simplified reinforcement scene.
    // This destroys all current entities and rebuilds from the simplified JSON.
    try {
        loader.HotReload(cfg.reinforcement_scene, registry);
    } catch (const std::exception& e) {
        std::cerr << "[Adaptive] Could not load reinforcement scene: "
                  << e.what() << " — continuing current scene.\n";
    }
}

} // namespace EduEngine
