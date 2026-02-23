// =============================================================================
// RewardEconomy.h — Academic Points & Badge System
// =============================================================================
// Tracks student achievement through two parallel mechanisms:
//
//   1. Academic Points — a running integer score awarded for:
//        • collecting educational tokens
//        • correct reflection answers (scaled by mastery score)
//        • completing scenes without skipping
//
//   2. Badges — named achievements that unlock when conditions are met.
//      Badge definitions are loaded from the scene JSON or hard-coded here.
//      All state is serialised to a per-student JSON profile on disk.
//
// Persistence format  (data/profiles/<student_id>.json):
//   {
//     "student_id":     "student_001",
//     "academic_points": 340,
//     "total_questions": 12,
//     "correct_answers": 9,
//     "mastery_average": 7.5,
//     "badges": [
//       { "id": "first_answer", "name": "First Steps",
//         "description": "Submit your first answer.", "unlocked": true }
//     ]
//   }
// =============================================================================
#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace EduEngine {

// ---------------------------------------------------------------------------
// Badge — a named achievement.
// ---------------------------------------------------------------------------
struct Badge {
    std::string id;
    std::string name;
    std::string description;
    std::string icon;       ///< Emoji or asset path shown in the editor panel.
    bool        unlocked = false;
};

// ---------------------------------------------------------------------------
// StudentProfile — serialisable snapshot of a student's progress.
// ---------------------------------------------------------------------------
struct StudentProfile {
    std::string student_id;
    int         academic_points  = 0;
    int         total_questions  = 0;    ///< Total reflection questions answered.
    int         correct_answers  = 0;    ///< Questions scoring ≥ 7.
    float       mastery_average  = 0.f;  ///< Rolling average of mastery scores.
    int         mastery_count    = 0;    ///< Number of scored answers (for average).
    std::vector<Badge> badges;
};

// ---------------------------------------------------------------------------
// RewardEconomy
// ---------------------------------------------------------------------------
class RewardEconomy {
public:
    explicit RewardEconomy(const std::string& student_id);

    // -----------------------------------------------------------------------
    // Point economy
    // -----------------------------------------------------------------------

    /// Add points and check badge thresholds.
    void AddPoints(int points);

    /// Deduct points (clamped to 0 — can't go negative).
    void DeductPoints(int points);

    [[nodiscard]] int GetPoints() const noexcept {
        return m_profile.academic_points;
    }

    // -----------------------------------------------------------------------
    // Mastery tracking
    // -----------------------------------------------------------------------

    /// Called by AdaptiveController each time a score arrives.
    void RecordMasteryScore(int score);

    /// Points awarded for a reflection = point_reward * (score / 10.0).
    static int CalculateReflectionPoints(int point_reward, int mastery_score);

    // -----------------------------------------------------------------------
    // Badge management
    // -----------------------------------------------------------------------

    /// Register a badge definition.  Call at startup for all possible badges.
    void RegisterBadge(Badge badge);

    /// Attempt to unlock a badge by ID.  Returns true if newly unlocked.
    bool UnlockBadge(const std::string& badge_id);

    [[nodiscard]] bool HasBadge(const std::string& badge_id) const;

    // -----------------------------------------------------------------------
    // Persistence
    // -----------------------------------------------------------------------

    void SaveProfile(const std::string& file_path) const;
    void LoadProfile(const std::string& file_path);

    // -----------------------------------------------------------------------
    // Accessors
    // -----------------------------------------------------------------------

    [[nodiscard]] const StudentProfile& GetProfile() const noexcept {
        return m_profile;
    }

private:
    // Check all badge unlock conditions that are point-threshold-based.
    void CheckPointThresholdBadges();

    StudentProfile m_profile;
};

} // namespace EduEngine
