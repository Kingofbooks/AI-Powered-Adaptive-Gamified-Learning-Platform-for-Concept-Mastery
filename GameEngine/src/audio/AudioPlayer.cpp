// =============================================================================
// AudioPlayer.cpp
// =============================================================================
// Uses __has_include so the engine compiles and runs silently when
// libsdl2-mixer-dev is not installed.  Install it to enable audio:
//   sudo apt-get install libsdl2-mixer-dev
// =============================================================================
#include "AudioPlayer.h"

#include <SDL2/SDL.h>
#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// Compile-time detection of SDL_mixer
// ---------------------------------------------------------------------------
#if __has_include(<SDL2/SDL_mixer.h>)
#  include <SDL2/SDL_mixer.h>
#  define HAVE_SDL_MIXER 1
#else
#  define HAVE_SDL_MIXER 0
#endif

namespace EduEngine {

// ---------------------------------------------------------------------------
// Module-level state — only meaningful when HAVE_SDL_MIXER == 1
// ---------------------------------------------------------------------------
namespace {
#if HAVE_SDL_MIXER
    bool       s_ok      = false;
    Mix_Chunk* s_collect = nullptr;
    Mix_Chunk* s_jump    = nullptr;
    Mix_Chunk* s_goal    = nullptr;
    Mix_Music* s_bgm     = nullptr;

    void FreeChunk(Mix_Chunk*& c) { if (c) { Mix_FreeChunk(c); c = nullptr; } }
    void FreeMusic(Mix_Music*& m) { if (m) { Mix_FreeMusic(m); m = nullptr; } }
#endif
} // anonymous namespace

// ---------------------------------------------------------------------------
bool AudioPlayer::Init() {
#if HAVE_SDL_MIXER
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "[AudioPlayer] Mix_OpenAudio failed: "
                  << Mix_GetError() << " — running silently.\n";
        return false;
    }
    Mix_AllocateChannels(8);
    s_ok = true;
    std::cout << "[AudioPlayer] Initialized (44100 Hz, stereo).\n";
    return true;
#else
    std::cout << "[AudioPlayer] SDL_mixer not available — audio disabled.\n"
                 "              Install libsdl2-mixer-dev to enable sound.\n";
    return false;
#endif
}

// ---------------------------------------------------------------------------
void AudioPlayer::Quit() {
#if HAVE_SDL_MIXER
    FreeChunk(s_collect);
    FreeChunk(s_jump);
    FreeChunk(s_goal);
    FreeMusic(s_bgm);
    if (s_ok) { Mix_CloseAudio(); Mix_Quit(); s_ok = false; }
#endif
}

// ---------------------------------------------------------------------------
void AudioPlayer::LoadTheme(const std::string& slug) {
#if HAVE_SDL_MIXER
    if (!s_ok) return;

    StopBGM();
    FreeChunk(s_collect);
    FreeChunk(s_jump);
    FreeChunk(s_goal);
    FreeMusic(s_bgm);

    const std::string base = "assets/audio/" + slug + "/";

    auto load_chunk = [&](const std::string& file) -> Mix_Chunk* {
        const std::string path = base + file;
        Mix_Chunk* c = Mix_LoadWAV(path.c_str());
        if (!c)
            std::cerr << "[AudioPlayer] Cannot load " << path
                      << ": " << Mix_GetError() << '\n';
        return c;
    };

    s_collect = load_chunk("collect.wav");
    s_jump    = load_chunk("jump.wav");
    s_goal    = load_chunk("goal.wav");

    const std::string bgm_path = base + "bgm.wav";
    s_bgm = Mix_LoadMUS(bgm_path.c_str());
    if (!s_bgm)
        std::cerr << "[AudioPlayer] Cannot load " << bgm_path
                  << ": " << Mix_GetError() << '\n';

    std::cout << "[AudioPlayer] Theme loaded: " << slug << '\n';
    PlayBGM();
#else
    (void)slug;
#endif
}

// ---------------------------------------------------------------------------
void AudioPlayer::PlayCollect() {
#if HAVE_SDL_MIXER
    if (s_ok && s_collect) Mix_PlayChannel(-1, s_collect, 0);
#endif
}

void AudioPlayer::PlayJump() {
#if HAVE_SDL_MIXER
    if (s_ok && s_jump) Mix_PlayChannel(-1, s_jump, 0);
#endif
}

void AudioPlayer::PlayGoal() {
#if HAVE_SDL_MIXER
    if (s_ok && s_goal) { StopBGM(); Mix_PlayChannel(-1, s_goal, 0); }
#endif
}

void AudioPlayer::PlayBGM() {
#if HAVE_SDL_MIXER
    if (s_ok && s_bgm && !Mix_PlayingMusic()) {
        Mix_VolumeMusic(64);
        Mix_PlayMusic(s_bgm, -1);
    }
#endif
}

void AudioPlayer::StopBGM() {
#if HAVE_SDL_MIXER
    if (s_ok && Mix_PlayingMusic()) Mix_HaltMusic();
#endif
}

} // namespace EduEngine
