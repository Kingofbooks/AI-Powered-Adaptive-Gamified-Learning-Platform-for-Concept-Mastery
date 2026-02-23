// =============================================================================
// ReflectionSystem.cpp
// =============================================================================
#include "ReflectionSystem.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

#include <SDL2/SDL.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>

// If libcurl is available define EDUENGINE_USE_CURL and include <curl/curl.h>.
// Otherwise the system falls back to the mock scorer automatically.
// #define EDUENGINE_USE_CURL

#ifdef EDUENGINE_USE_CURL
#  include <curl/curl.h>
#  include <nlohmann/json.hpp>
#endif

namespace EduEngine {

// =============================================================================
// Constructor / Destructor
// =============================================================================

ReflectionSystem::ReflectionSystem() = default;

ReflectionSystem::~ReflectionSystem() {
    // Join any pending background thread to avoid dangling references.
    if (m_score_thread.joinable()) {
        m_score_thread.join();
    }
}

// =============================================================================
// TriggerReflection
// =============================================================================

void ReflectionSystem::TriggerReflection(const std::string& question,
                                         const std::string& subject,
                                         const std::string& hint,
                                         int                point_reward,
                                         int                attempts_allowed) {
    if (m_state != ReflectionState::Inactive) return; // Already active.

    m_question         = question;
    m_subject          = subject;
    m_hint             = hint;
    m_point_reward     = point_reward;
    m_attempts_allowed = attempts_allowed;
    m_attempts_used    = 0;
    m_hint_visible     = false;
    m_last_score       = 0;
    m_ai_feedback.clear();
    std::memset(m_input_buf, 0, INPUT_BUF_SIZE);

    m_state = ReflectionState::AwaitingInput;
    std::cout << "[Reflection] Triggered: " << question << '\n';

    // Tell SDL to accept text input for the ImGui field.
    SDL_StartTextInput();
}

// =============================================================================
// ProcessEvent — forward SDL key events for backspace etc.
// =============================================================================

void ReflectionSystem::ProcessEvent(const SDL_Event& event) {
    // ImGui_ImplSDL2_ProcessEvent already handles the text events for ImGui.
    // We only need special-case handling here if we had a custom input widget.
    (void)event;
}

// =============================================================================
// Update — state machine tick
// =============================================================================

void ReflectionSystem::Update(float dt, Registry& /*registry*/) {
    // Poll whether the async scoring thread delivered a result.
    if (m_state == ReflectionState::AwaitingScore && m_score_ready.load()) {
        std::lock_guard<std::mutex> lock(m_score_mutex);
        ApplyScore(m_pending_score);
        m_ai_feedback = m_pending_feedback;
        m_score_ready.store(false);
    }

    // Countdown the result display before returning to Inactive.
    if (m_state == ReflectionState::Scored) {
        m_result_timer -= dt;
        if (m_result_timer <= 0.f) {
            m_state = ReflectionState::Inactive;
            SDL_StopTextInput();
            std::cout << "[Reflection] Complete. Returning to gameplay.\n";
        }
    }
}

// =============================================================================
// Render — full-screen ImGui overlay
// =============================================================================

void ReflectionSystem::Render(SDL_Renderer* /*renderer*/) {
    if (m_state == ReflectionState::Inactive) return;

    // Darken the game world by drawing a semi-transparent full-screen rect
    // (SDL-level call, before ImGui renders on top).
    // Note: actual SDL colour rect is emitted in Engine::Render(); we just
    // handle the ImGui windows here.

    switch (m_state) {
        case ReflectionState::AwaitingInput: RenderQuestionUI();  break;
        case ReflectionState::AwaitingScore: RenderScoringUI();   break;
        case ReflectionState::Scored:        RenderResultUI();    break;
        default: break;
    }
}

// =============================================================================
// UI helpers
// =============================================================================

void ReflectionSystem::RenderQuestionUI() {
    // Centre a large modal-style window.
    const ImGuiIO& io = ImGui::GetIO();
    const ImVec2 win_size{640.f, 400.f};
    ImGui::SetNextWindowPos(
        ImVec2((io.DisplaySize.x - win_size.x) * 0.5f,
               (io.DisplaySize.y - win_size.y) * 0.5f),
        ImGuiCond_Always);
    ImGui::SetNextWindowSize(win_size, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.96f);

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoScrollbar;

    ImGui::Begin("##reflection_q", nullptr, flags);

    // ── Header ──────────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.3f, 0.9f, 0.5f, 1.f));
    ImGui::TextWrapped("Reflection Checkpoint — %s", m_subject.c_str());
    ImGui::PopStyleColor();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Attempt counter ──────────────────────────────────────────────────
    ImGui::TextDisabled("Attempt %d of %d", m_attempts_used + 1, m_attempts_allowed);
    ImGui::Spacing();

    // ── Question ─────────────────────────────────────────────────────────
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.95f, 0.7f, 1.f));
    ImGui::TextWrapped("%s", m_question.c_str());
    ImGui::PopStyleColor();
    ImGui::Spacing();

    // ── Hint ─────────────────────────────────────────────────────────────
    if (!m_hint.empty()) {
        if (ImGui::SmallButton("Show Hint")) m_hint_visible = !m_hint_visible;
        if (m_hint_visible) {
            ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.f, 1.f), "Hint: %s", m_hint.c_str());
        }
        ImGui::Spacing();
    }

    // ── Text input ────────────────────────────────────────────────────────
    ImGui::Text("Your answer:");
    ImGui::SetNextItemWidth(-1.f); // Full-width.
    // Auto-focus so the student can type immediately.
    if (ImGui::IsWindowAppearing()) {
        ImGui::SetKeyboardFocusHere();
    }
    ImGui::InputTextMultiline("##answer", m_input_buf, INPUT_BUF_SIZE,
                              ImVec2(-1, 120));
    ImGui::Spacing();

    // ── Submit / Skip ─────────────────────────────────────────────────────
    const bool has_answer = (m_input_buf[0] != '\0');

    ImGui::BeginDisabled(!has_answer);
    if (ImGui::Button("Submit Answer", ImVec2(160, 36))) {
        ++m_attempts_used;
        m_state = ReflectionState::AwaitingScore;
        DispatchScoringRequest(std::string(m_input_buf));
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    if (ImGui::Button("Skip (lose points)", ImVec2(160, 36))) {
        // Skipping awards zero mastery and costs points.
        m_last_score = 0;
        m_ai_feedback = "Question skipped.";
        m_state = ReflectionState::Scored;
        m_result_timer = RESULT_DISPLAY_SECONDS;
        if (m_on_score) m_on_score(0, m_subject);
    }

    ImGui::End();
}

void ReflectionSystem::RenderScoringUI() {
    const ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(
        ImVec2(io.DisplaySize.x * 0.5f - 150.f, io.DisplaySize.y * 0.5f - 60.f),
        ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 120), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.9f);

    ImGui::Begin("##scoring", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove);

    // Animated spinner via text cycling.
    const char* spinner[] = {"|", "/", "─", "\\"};
    const int   spin_idx  = (SDL_GetTicks() / 200) % 4;
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.f, 1.f),
        "%s  AI is evaluating your answer...", spinner[spin_idx]);
    ImGui::Spacing();
    ImGui::ProgressBar(-1.f * (SDL_GetTicks() / 1000.f), ImVec2(-1, 0), "");

    ImGui::End();
}

void ReflectionSystem::RenderResultUI() {
    const ImGuiIO& io = ImGui::GetIO();
    const ImVec2 win_size{520.f, 280.f};
    ImGui::SetNextWindowPos(
        ImVec2((io.DisplaySize.x - win_size.x) * 0.5f,
               (io.DisplaySize.y - win_size.y) * 0.5f),
        ImGuiCond_Always);
    ImGui::SetNextWindowSize(win_size, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.95f);

    ImGui::Begin("##result", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove);

    // ── Score badge ───────────────────────────────────────────────────────
    const bool good_score = m_last_score >= 7;
    const ImVec4 score_col = good_score
        ? ImVec4(0.2f, 1.f, 0.4f, 1.f)
        : ImVec4(1.f, 0.4f, 0.2f, 1.f);

    ImGui::PushStyleColor(ImGuiCol_Text, score_col);
    ImGui::SetWindowFontScale(2.2f);
    ImGui::Text(" Mastery Score: %d / 10 ", m_last_score);
    ImGui::SetWindowFontScale(1.f);
    ImGui::PopStyleColor();

    ImGui::Separator();

    if (good_score) {
        ImGui::TextColored(ImVec4(0.4f, 1.f, 0.4f, 1.f),
            "Excellent! Keep going.");
    } else {
        ImGui::TextColored(ImVec4(1.f, 0.7f, 0.3f, 1.f),
            "Let's reinforce this concept.");
    }

    if (!m_ai_feedback.empty()) {
        ImGui::Spacing();
        ImGui::TextWrapped("Feedback: %s", m_ai_feedback.c_str());
    }

    ImGui::Spacing();
    const float progress = 1.f - (m_result_timer / RESULT_DISPLAY_SECONDS);
    ImGui::ProgressBar(progress, ImVec2(-1, 6), "");

    ImGui::End();
}

// =============================================================================
// Scoring
// =============================================================================

void ReflectionSystem::DispatchScoringRequest(const std::string& answer) {
    // Join any previously finished thread (non-blocking: thread is done by now).
    if (m_score_thread.joinable()) {
        m_score_thread.join();
    }

    // Capture by value — the lambda outlives this stack frame.
    const std::string q    = m_question;
    const std::string subj = m_subject;
    const bool use_mock    = m_mock_mode;
    const std::string url  = m_api_url;
    const std::string key  = m_api_key;

    m_score_thread = std::thread([this, answer, q, subj, use_mock, url, key]() {
        int         score    = 5;
        std::string feedback = "Thank you for your response.";

        if (use_mock) {
            // ── Mock implementation ────────────────────────────────────
            // Simulate a 1.5 s "network delay" for realism.
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
            score    = MockScore(answer);
            feedback = (score >= 7)
                ? "Great explanation! Your answer demonstrates solid understanding."
                : "Good try. Review the concept and consider the key terms.";
        }
#ifdef EDUENGINE_USE_CURL
        else {
            // ── Real HTTP POST to AI API ────────────────────────────────
            // Build the request JSON.
            nlohmann::json req;
            req["question"] = q;
            req["answer"]   = answer;
            req["subject"]  = subj;
            const std::string body = req.dump();

            // Set up libcurl.
            CURL* curl = curl_easy_init();
            if (curl) {
                std::string response_buf;

                struct curl_slist* headers = nullptr;
                headers = curl_slist_append(headers, "Content-Type: application/json");
                const std::string auth_header = "Authorization: Bearer " + key;
                headers = curl_slist_append(headers, auth_header.c_str());

                curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
                // Write callback stores response into response_buf.
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                    +[](char* ptr, size_t size, size_t nmemb, void* ud) -> size_t {
                        static_cast<std::string*>(ud)->append(ptr, size * nmemb);
                        return size * nmemb;
                    });
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buf);

                const CURLcode res = curl_easy_perform(curl);
                curl_slist_free_all(headers);
                curl_easy_cleanup(curl);

                if (res == CURLE_OK) {
                    try {
                        auto resp = nlohmann::json::parse(response_buf);
                        score    = resp.value("mastery_score", 5);
                        feedback = resp.value("feedback", feedback);
                    } catch (...) { /* keep defaults */ }
                }
            }
        }
#endif // EDUENGINE_USE_CURL

        // Deliver result back to the main thread.
        {
            std::lock_guard<std::mutex> lock(m_score_mutex);
            m_pending_score    = score;
            m_pending_feedback = feedback;
        }
        m_score_ready.store(true);
    });
}

// ---------------------------------------------------------------------------
// MockScore — basic keyword relevance heuristic.
// In production this is replaced by a real LLM evaluation call.
// ---------------------------------------------------------------------------
int ReflectionSystem::MockScore(const std::string& answer) const {
    if (answer.empty()) return 1;

    // Lowercase copy for case-insensitive comparison.
    std::string low = answer;
    std::transform(low.begin(), low.end(), low.begin(),
        [](unsigned char c){ return std::tolower(c); });

    // Subject-specific keyword banks (extendable or loaded from JSON).
    static const std::unordered_map<std::string, std::vector<std::string>> keyword_banks = {
        {"photosynthesis", {"chlorophyll","light","sun","carbon dioxide","co2",
                            "oxygen","glucose","chloroplast","water","h2o",
                            "energy","plant","leaf","stomat"}},
        {"biology",        {"cell","dna","rna","protein","gene","organism",
                            "mitosis","meiosis","enzyme","membrane"}},
        {"general",        {"because","therefore","which","process","result",
                            "cause","effect","important","describes","means"}}
    };

    // Pick the bank for the current subject; fall back to general.
    std::string subj_low = m_subject;
    std::transform(subj_low.begin(), subj_low.end(), subj_low.begin(),
        [](unsigned char c){ return std::tolower(c); });

    const auto& bank = keyword_banks.count(subj_low)
        ? keyword_banks.at(subj_low)
        : keyword_banks.at("general");

    int hits = 0;
    for (const auto& kw : bank) {
        if (low.find(kw) != std::string::npos) ++hits;
    }

    // Map hits to a 1-10 score (min 1, max 10).
    const int max_hits = static_cast<int>(bank.size());
    const float ratio  = static_cast<float>(hits) / static_cast<float>(max_hits);
    return std::clamp(static_cast<int>(ratio * 10.f) + 1, 1, 10);
}

// ---------------------------------------------------------------------------
void ReflectionSystem::ApplyScore(int score) {
    m_last_score   = score;
    m_state        = ReflectionState::Scored;
    m_result_timer = RESULT_DISPLAY_SECONDS;

    std::cout << "[Reflection] Score received: " << score << "/10"
              << " | Subject: " << m_subject << '\n';

    // Fire callback so AdaptiveController can branch the game path.
    if (m_on_score) {
        m_on_score(score, m_subject);
    }
}

// =============================================================================
// Queries
// =============================================================================

bool ReflectionSystem::IsActive() const noexcept {
    return m_state != ReflectionState::Inactive;
}

} // namespace EduEngine
