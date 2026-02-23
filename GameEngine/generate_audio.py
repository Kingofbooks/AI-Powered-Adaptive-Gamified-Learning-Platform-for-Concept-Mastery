#!/usr/bin/env python3
"""
generate_audio.py  —  Procedural WAV sound generator for EduEngine.

Generates 4 audio files per theme using only Python built-in modules
(wave + struct + math). No pip dependencies required.

Output layout:
    assets/audio/<theme>/collect.wav   — short chime when picking up item
    assets/audio/<theme>/jump.wav      — quick blip when player jumps
    assets/audio/<theme>/goal.wav      — triumphant fanfare at level end
    assets/audio/<theme>/bgm.wav       — looping background melody (~8 s)

Usage:
    python3 generate_audio.py --theme photosynthesis
    python3 generate_audio.py --all
    python3 generate_audio.py --list
    python3 generate_audio.py --all --out assets/audio
"""

import wave
import struct
import math
import os
import argparse
import sys

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------
RATE = 44100   # sample rate (Hz)
MAX_S = 32767  # 16-bit signed max

# ---------------------------------------------------------------------------
# Per-theme data
# Each entry:
#   freq   — base frequency (Hz) — colours the timbre
#   bgm    — 5 MIDI note numbers that form the melodic motif
#   bright — 0..1, blend between sine (0) and sawtooth (1) for character
# ---------------------------------------------------------------------------
THEMES = {
    "photosynthesis": {"freq": 528, "bgm": [60, 62, 64, 67, 69], "bright": 0.15},
    "space":          {"freq": 220, "bgm": [36, 38, 43, 41, 36], "bright": 0.05},
    "water_cycle":    {"freq": 432, "bgm": [57, 59, 60, 62, 64], "bright": 0.10},
    "mathematics":    {"freq": 440, "bgm": [60, 64, 67, 72, 71], "bright": 0.30},
    "history":        {"freq": 392, "bgm": [48, 50, 52, 55, 53], "bright": 0.20},
    "chemistry":      {"freq": 480, "bgm": [62, 65, 67, 70, 69], "bright": 0.25},
    "physics":        {"freq": 410, "bgm": [55, 57, 59, 60, 62], "bright": 0.20},
    "biology":        {"freq": 528, "bgm": [64, 66, 67, 69, 71], "bright": 0.10},
    "geography":      {"freq": 396, "bgm": [52, 55, 57, 60, 59], "bright": 0.15},
    "default":        {"freq": 440, "bgm": [60, 62, 64, 65, 67], "bright": 0.20},
}

# ---------------------------------------------------------------------------
# Utility helpers
# ---------------------------------------------------------------------------

def midi_to_freq(note: int) -> float:
    """Convert MIDI note number to frequency in Hz."""
    return 440.0 * (2.0 ** ((note - 69) / 12.0))


def synth(freq: float, duration: float, volume: float = 0.5,
          bright: float = 0.0) -> list:
    """
    Generate samples for a single tone.
    bright = 0 → pure sine;  bright = 1 → sawtooth-like (adds odd harmonics).
    """
    n = int(duration * RATE)
    samples = []
    for i in range(n):
        t = i / RATE
        s = math.sin(2 * math.pi * freq * t)
        if bright > 0:
            # Add 2nd + 3rd harmonic for brightness
            s += bright * 0.4 * math.sin(2 * math.pi * freq * 2 * t)
            s += bright * 0.2 * math.sin(2 * math.pi * freq * 3 * t)
        samples.append(volume * s)
    return samples


def envelope(samples: list, attack: float = 0.01, release: float = 0.1) -> list:
    """Apply linear attack/release fade to avoid clicks."""
    n = len(samples)
    a_n = int(attack * RATE)
    r_n = int(release * RATE)
    out = list(samples)
    for i in range(min(a_n, n)):
        out[i] *= i / max(a_n, 1)
    for i in range(min(r_n, n)):
        out[n - 1 - i] *= i / max(r_n, 1)
    return out


def mix(a: list, b: list) -> list:
    """Sum two sample lists (padding shorter one with zeros)."""
    length = max(len(a), len(b))
    result = [0.0] * length
    for i, v in enumerate(a):
        result[i] += v
    for i, v in enumerate(b):
        result[i] += v
    return result


def write_wav(path: str, samples: list) -> None:
    """Write sample list (floats -1..1) as a 16-bit mono WAV file."""
    os.makedirs(os.path.dirname(path) if os.path.dirname(path) else ".", exist_ok=True)
    with wave.open(path, 'w') as wf:
        wf.setnchannels(1)
        wf.setsampwidth(2)      # 16-bit
        wf.setframerate(RATE)
        for s in samples:
            clamped = max(-1.0, min(1.0, s))
            wf.writeframes(struct.pack('<h', int(clamped * MAX_S)))

# ---------------------------------------------------------------------------
# Sound generators
# ---------------------------------------------------------------------------

def make_collect(td: dict, out_dir: str) -> None:
    """
    Short ascending chime (≈ 0.35 s).
    Two overlapping tones: base freq rises by a perfect fifth.
    """
    f0 = td["freq"]
    b  = td["bright"]
    n  = int(0.35 * RATE)

    # Rising sweep
    sweep = []
    for i in range(n):
        t = i / RATE
        freq = f0 * (1.0 + (t / 0.35) * 0.5)   # sweeps up to f0 * 1.5
        s = 0.45 * math.sin(2 * math.pi * freq * t)
        if b > 0:
            s += b * 0.18 * math.sin(2 * math.pi * freq * 2 * t)
        sweep.append(s)

    # Bell overtone at f0 * 2
    bell = envelope(synth(f0 * 2.0, 0.25, 0.22, b), attack=0.005, release=0.18)

    samples = envelope(mix(sweep, bell), attack=0.003, release=0.15)
    write_wav(os.path.join(out_dir, "collect.wav"), samples)
    print(f"    collect.wav")


def make_jump(td: dict, out_dir: str) -> None:
    """
    Quick upward frequency sweep (≈ 0.18 s).
    Short, punchy — doesn't distract from gameplay.
    """
    f0 = td["freq"] * 0.75
    n  = int(0.18 * RATE)

    samples = []
    for i in range(n):
        t = i / RATE
        freq = f0 * (1.0 + (t / 0.18) * 0.8)
        samples.append(0.38 * math.sin(2 * math.pi * freq * t))

    samples = envelope(samples, attack=0.003, release=0.10)
    write_wav(os.path.join(out_dir, "jump.wav"), samples)
    print(f"    jump.wav")


def make_goal(td: dict, out_dir: str) -> None:
    """
    Three-note ascending fanfare (≈ 1.4 s).
    Uses the theme's bgm motif: root → third → fifth.
    """
    notes  = td["bgm"]
    b      = td["bright"]
    # Pick 3 notes: root, middle, top
    trio   = [notes[0], notes[2], notes[4]]
    durs   = [0.28, 0.28, 0.70]

    samples = []
    for note, dur in zip(trio, durs):
        freq = midi_to_freq(note)
        seg  = envelope(synth(freq, dur, 0.42, b), attack=0.01, release=0.15)
        # Add octave harmony quietly
        harm = envelope(synth(freq * 2.0, dur, 0.18, b * 0.5), attack=0.01, release=0.15)
        samples += mix(seg, harm)
        # Tiny silence between notes
        samples += [0.0] * int(0.04 * RATE)

    write_wav(os.path.join(out_dir, "goal.wav"), samples)
    print(f"    goal.wav")


def make_bgm(td: dict, out_dir: str) -> None:
    """
    Looping background melody — 4 repetitions of the 5-note motif (~8 s).
    Melody + quiet bass octave below.  Pure sine keeps it non-fatiguing.
    """
    notes  = td["bgm"]
    b      = td["bright"] * 0.5   # softer harmonics for BGM
    beat   = 0.40                  # seconds per beat
    beat_n = int(beat * RATE)

    samples = []
    for _ in range(4):             # 4 loops × 5 notes = 20 beats ≈ 8 s
        for note in notes:
            freq = midi_to_freq(note)
            # Melody — slightly shorter than full beat for breathing room
            mel  = envelope(synth(freq,       beat * 0.82, 0.22, b),
                            attack=0.01, release=0.18)
            # Bass — half-beat duration, octave below
            bass = envelope(synth(freq * 0.5, beat * 0.50, 0.10, 0.0),
                            attack=0.02, release=0.18)

            combined = mix(mel, bass)
            # Pad / trim to exactly one beat
            beat_block = [0.0] * beat_n
            for i, v in enumerate(combined[:beat_n]):
                beat_block[i] += v
            samples += beat_block

    write_wav(os.path.join(out_dir, "bgm.wav"), samples)
    print(f"    bgm.wav")


# ---------------------------------------------------------------------------
# Per-theme generation
# ---------------------------------------------------------------------------

def generate_theme(name: str, out_root: str = "assets/audio") -> None:
    if name not in THEMES:
        print(f"[generate_audio] Unknown theme: '{name}'. "
              f"Available: {', '.join(THEMES)}", file=sys.stderr)
        sys.exit(1)

    td      = THEMES[name]
    out_dir = os.path.join(out_root, name)
    os.makedirs(out_dir, exist_ok=True)

    print(f"  Generating audio → {out_dir}/")
    make_collect(td, out_dir)
    make_jump(td, out_dir)
    make_goal(td, out_dir)
    make_bgm(td, out_dir)
    print(f"  Done — 4 WAV files written.")


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="EduEngine procedural audio generator (WAV, no dependencies).")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--theme",  metavar="NAME",
                       help="Generate audio for a single theme.")
    group.add_argument("--all",    action="store_true",
                       help="Generate audio for all themes.")
    group.add_argument("--list",   action="store_true",
                       help="List available themes and exit.")
    parser.add_argument("--out", metavar="DIR", default="assets/audio",
                        help="Output root directory (default: assets/audio).")
    args = parser.parse_args()

    if args.list:
        print("Available themes:")
        for name in sorted(THEMES):
            print(f"  {name}")
        return

    if args.all:
        print(f"=== Generating audio for all {len(THEMES)} themes ===")
        for name in THEMES:
            generate_theme(name, args.out)
        total = len(THEMES) * 4
        print(f"\n=== Done — {total} WAV files written to {args.out}/ ===")
    else:
        generate_theme(args.theme, args.out)


if __name__ == "__main__":
    main()
