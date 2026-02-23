// =============================================================================
// AudioPlayer.h  —  Static audio interface for EduEngine
// =============================================================================
// Thin wrapper around SDL2_mixer.  Any system can call these without holding
// a reference to the Engine or AudioSystem object.
//
// Workflow:
//   1. Engine::Init()      → AudioPlayer::Init()
//   2. PromptSystem / Load → AudioPlayer::LoadTheme("photosynthesis")
//   3. In-game events:
//        jump collected    → AudioPlayer::PlayJump()
//        item collected    → AudioPlayer::PlayCollect()
//        goal reached      → AudioPlayer::PlayGoal()
//   4. Engine::Shutdown()  → AudioPlayer::Quit()
//
// All functions are gracefully no-ops if SDL_mixer is unavailable or audio
// files are missing — the game runs silently rather than crashing.
// =============================================================================
#pragma once

#include <string>

namespace EduEngine {

class AudioPlayer {
public:
    // ── Lifecycle ─────────────────────────────────────────────────────────
    /// Open the audio device.  Call once during engine init.
    static bool Init();

    /// Close the audio device and free all loaded sounds.
    static void Quit();

    // ── Theme loading ─────────────────────────────────────────────────────
    /// Load the four WAV files for the given theme slug.
    /// Called by PromptSystem after generating a scene, and by SceneLoader
    /// when restoring a saved scene.
    /// Path pattern: assets/audio/<slug>/{collect,jump,goal,bgm}.wav
    static void LoadTheme(const std::string& slug);

    // ── Playback ──────────────────────────────────────────────────────────
    static void PlayCollect();    ///< Item picked up.
    static void PlayJump();       ///< Player jumped.
    static void PlayGoal();       ///< Level complete.

    static void PlayBGM();        ///< Start looping background music.
    static void StopBGM();        ///< Stop background music.

    // Non-copyable / non-instantiable
    AudioPlayer()                            = delete;
    AudioPlayer(const AudioPlayer&)          = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;
};

} // namespace EduEngine
