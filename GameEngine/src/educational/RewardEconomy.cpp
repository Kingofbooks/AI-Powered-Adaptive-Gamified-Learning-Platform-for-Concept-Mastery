// =============================================================================
// RewardEconomy.cpp
// =============================================================================
#include "RewardEconomy.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <cmath>

using json = nlohmann::json;

namespace EduEngine {

// =============================================================================
// Constructor — seed the default badge catalogue
// =============================================================================

RewardEconomy::RewardEconomy(const std::string& student_id) {
    m_profile.student_id = student_id;

    // Pre-register all badges the engine knows about.
    // Additional badges can be injected from scene JSON.
    RegisterBadge({"first_answer",    "First Steps",
        "Submit your very first reflection answer.", "⭐", false});
    RegisterBadge({"perfect_score",   "Full Marks",
        "Achieve a mastery score of 10/10.",          "🏆", false});
    RegisterBadge({"streak_3",        "On a Roll",
        "Score ≥ 7 on three consecutive questions.",  "🔥", false});
    RegisterBadge({"points_100",      "Century",
        "Accumulate 100 Academic Points.",            "💯", false});
    RegisterBadge({"points_500",      "Scholar",
        "Accumulate 500 Academic Points.",            "📚", false});
    RegisterBadge({"scene_complete",  "Level Clear",
        "Complete a full scene without skipping.",    "🎓", false});
    RegisterBadge({"photosynthesis_1","Green Thumb",
        "Answer a photosynthesis question correctly.","🌱", false});
}

// =============================================================================
// Point economy
// =============================================================================

void RewardEconomy::AddPoints(int points) {
    m_profile.academic_points += points;
    std::cout << "[Rewards] +" << points << " points  (total: "
              << m_profile.academic_points << ")\n";
    CheckPointThresholdBadges();
}

void RewardEconomy::DeductPoints(int points) {
    m_profile.academic_points = std::max(0, m_profile.academic_points - points);
}

// =============================================================================
// Mastery tracking
// =============================================================================

void RewardEconomy::RecordMasteryScore(int score) {
    ++m_profile.total_questions;
    if (score >= 7) ++m_profile.correct_answers;

    // Incremental rolling average  (avoids re-summing all history).
    ++m_profile.mastery_count;
    m_profile.mastery_average += (static_cast<float>(score) - m_profile.mastery_average)
                                / static_cast<float>(m_profile.mastery_count);

    // Badge: first answer ever.
    if (m_profile.total_questions == 1) {
        UnlockBadge("first_answer");
    }
    // Badge: perfect score.
    if (score == 10) {
        UnlockBadge("perfect_score");
    }
}

/*static*/ int RewardEconomy::CalculateReflectionPoints(int point_reward, int mastery_score) {
    // Award a fraction of the maximum points proportional to the mastery score.
    // score 10 → 100%, score 5 → 50%, score 1 → 10%.
    const float ratio = static_cast<float>(mastery_score) / 10.f;
    return static_cast<int>(std::round(point_reward * ratio));
}

// =============================================================================
// Badge management
// =============================================================================

void RewardEconomy::RegisterBadge(Badge badge) {
    // Don't double-register.
    for (const auto& b : m_profile.badges) {
        if (b.id == badge.id) return;
    }
    m_profile.badges.push_back(std::move(badge));
}

bool RewardEconomy::UnlockBadge(const std::string& badge_id) {
    for (auto& b : m_profile.badges) {
        if (b.id == badge_id && !b.unlocked) {
            b.unlocked = true;
            std::cout << "[Rewards] Badge unlocked: " << b.name
                      << "  " << b.icon << '\n';
            return true;
        }
    }
    return false; // Already unlocked or not found.
}

bool RewardEconomy::HasBadge(const std::string& badge_id) const {
    for (const auto& b : m_profile.badges) {
        if (b.id == badge_id) return b.unlocked;
    }
    return false;
}

// ---------------------------------------------------------------------------
// CheckPointThresholdBadges — run after every point addition.
// ---------------------------------------------------------------------------
void RewardEconomy::CheckPointThresholdBadges() {
    const int pts = m_profile.academic_points;
    if (pts >= 100) UnlockBadge("points_100");
    if (pts >= 500) UnlockBadge("points_500");
}

// =============================================================================
// Persistence
// =============================================================================

void RewardEconomy::SaveProfile(const std::string& file_path) const {
    json j;
    j["student_id"]      = m_profile.student_id;
    j["academic_points"] = m_profile.academic_points;
    j["total_questions"] = m_profile.total_questions;
    j["correct_answers"] = m_profile.correct_answers;
    j["mastery_average"] = m_profile.mastery_average;
    j["mastery_count"]   = m_profile.mastery_count;

    json badges_arr = json::array();
    for (const auto& b : m_profile.badges) {
        badges_arr.push_back({
            {"id",          b.id},
            {"name",        b.name},
            {"description", b.description},
            {"icon",        b.icon},
            {"unlocked",    b.unlocked}
        });
    }
    j["badges"] = badges_arr;

    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "[Rewards] Cannot save profile to: " << file_path << '\n';
        return;
    }
    file << j.dump(4); // Pretty-print with 4-space indent.
    std::cout << "[Rewards] Profile saved: " << file_path << '\n';
}

void RewardEconomy::LoadProfile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        // New student — profile will be created on first save.
        std::cout << "[Rewards] No existing profile at: " << file_path
                  << " — starting fresh.\n";
        return;
    }

    try {
        const json j = json::parse(file);

        m_profile.academic_points = j.value("academic_points", 0);
        m_profile.total_questions = j.value("total_questions", 0);
        m_profile.correct_answers = j.value("correct_answers", 0);
        m_profile.mastery_average = j.value("mastery_average", 0.f);
        m_profile.mastery_count   = j.value("mastery_count",   0);

        // Merge saved badge state into registered badges.
        if (j.contains("badges")) {
            for (const auto& bj : j["badges"]) {
                const std::string id       = bj.value("id", "");
                const bool        unlocked = bj.value("unlocked", false);
                for (auto& reg_badge : m_profile.badges) {
                    if (reg_badge.id == id) {
                        reg_badge.unlocked = unlocked;
                        break;
                    }
                }
            }
        }

        std::cout << "[Rewards] Profile loaded: " << file_path
                  << " | Points: " << m_profile.academic_points << '\n';
    } catch (const std::exception& e) {
        std::cerr << "[Rewards] Failed to parse profile: " << e.what() << '\n';
    }
}

} // namespace EduEngine
