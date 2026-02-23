#!/usr/bin/env python3
"""
call_claude_api.py  —  AI scene-JSON generator for EduEngine (Ollama edition).

Uses Ollama — completely FREE, runs locally, no API key needed.

Called automatically by PromptSystem::CallApi() in the C++ engine.
You can also run it manually:

    python3 call_claude_api.py /tmp/edu_prompt_cfg.json

The config file must contain JSON like:
    { "prompt": "teach about photosynthesis", "subject": "biology", "grade": 5 }

Output: valid EduEngine scene JSON on stdout.

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
ONE-TIME SETUP (10 minutes):
  1. Install Ollama:  https://ollama.com/download
     or: curl -fsSL https://ollama.com/install.sh | sh
  2. Pull a model:    ollama pull llama3.1
  3. Install requests (usually already there):
                      pip install requests
  4. Done — no API key, no cost, runs offline.
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

CONFIGURATION (environment variables, all optional):
  OLLAMA_MODEL   Which model to use  (default: llama3.1)
  OLLAMA_HOST    Ollama server URL    (default: http://localhost:11434)

  Example:
    export OLLAMA_MODEL=mistral
    export OLLAMA_HOST=http://localhost:11434

TO USE YOUR FRIEND'S CUSTOM MODEL (future):
  If it's Ollama-compatible, set OLLAMA_MODEL to the model name they give you.
  If it's a different server: set OLLAMA_HOST to their server URL.
  No code changes needed.
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
"""

import json
import os
import re
import sys
import time

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
OLLAMA_HOST  = os.getenv("OLLAMA_HOST",  "http://localhost:11434")
OLLAMA_MODEL = os.getenv("OLLAMA_MODEL", "llama3.1")
MAX_RETRIES  = 3       # attempts before giving up
TIMEOUT_SEC  = 120     # per request (local models can be slow)

# ---------------------------------------------------------------------------
# Prompt — note the strong JSON-only instruction repeated several times.
# Local models need more explicit direction than cloud models.
# ---------------------------------------------------------------------------
SYSTEM_PROMPT = """\
You are a JSON generator for EduEngine, a 2D educational platformer game engine.
You output ONLY valid JSON — no explanation, no markdown, no code fences.
Your entire response must be a single JSON object that starts with { and ends with }.

WORLD DIMENSIONS: 1280 x 720 pixels.

JSON STRUCTURE:
{
  "scene_meta": {
    "id": "<snake_case_id>",
    "title": "<Human Readable Title>",
    "subject": "<topic>",
    "grade": "<number as string>",
    "ai_prompt": "<original prompt>"
  },
  "world_settings": {
    "gravity": 980.0,
    "background_color": [R, G, B, 255]
  },
  "entities": [ ...list of entity objects... ]
}

ENTITY OBJECT FORMAT:
{
  "name": "<unique name>",
  "tags": ["<one of: player/platform/collectible/enemy/goal/info/background>"],
  "components": {
    "transform":   { "x": N, "y": N, "scale_x": 1.0, "scale_y": 1.0 },
    "sprite":      { "texture": "assets/images/<theme>/file.png", "color": [R,G,B,A], "layer": N },
    "rigid_body":  { "mass": 1.0, "gravity_scale": 1.0 },
    "collider":    { "width": N, "height": N, "offset_x": 0, "is_trigger": false },
    "player_tag":  { "move_speed": 220, "jump_force": -580, "max_jumps": 2 },
    "platform":    { "is_one_way": true },
    "collectible": { "point_value": 15, "label": "<educational term>" },
    "text_label":  { "text": "<text>", "color": [R,G,B,A], "font_size": 14, "layer": 6 },
    "info_zone":   { "text": "<educational fact>", "repeat": false },
    "enemy":       { "patrol_min_x": N, "patrol_max_x": N, "speed": 80.0 },
    "goal":        {}
  }
}

REQUIRED ENTITIES (must include all of these):
1. ONE background (tags:["background"], sprite layer:0, transform x:0 y:0 scale_x:1 scale_y:1, no collider)
2. ONE player    (tags:["player"], x:80 y:520, has rigid_body + collider + player_tag)
3. ONE ground    (tags:["platform"], x:0 y:620 scale_x:40 scale_y:5, collider width:1280 height:100, is_one_way:false)
4. 3-5 platforms (tags:["platform"], y between 280-580, x between 100-1100, is_one_way:true)
5. 3-5 collectibles (tags:["collectible"], collider is_trigger:true, educational labels)
6. 1-3 info signs (tags:["info"], x between 200-1000 y:590, collider is_trigger:true)
7. ONE goal flag (tags:["goal"], x:1160 y:558, collider is_trigger:true)
8. 0-2 enemies if grade >= 5 (tags:["enemy"], has rigid_body + collider + enemy component)

TEXTURE PATHS — use the theme slug from the topic:
  assets/images/<slug>/background.png   (scale_x:1 scale_y:1 fills the screen)
  assets/images/<slug>/player.png
  assets/images/<slug>/platform.png
  assets/images/<slug>/collectible_0.png  (use _0 through _4 for different collectibles)
  assets/images/<slug>/enemy.png
  assets/images/<slug>/goal.png

THEME SLUG MAPPING:
  photosynthesis → photosynthesis
  space/solar    → space
  water cycle    → water_cycle
  math           → mathematics
  history        → history
  chemistry      → chemistry
  physics        → physics
  biology        → biology
  geography      → geography
  other          → default

Remember: output ONLY the JSON object, starting with { and ending with }.\
"""

# Concise user message — full detail is in the system prompt
USER_TEMPLATE = """\
Generate an EduEngine scene JSON for:
Topic: {prompt}
Subject: {subject}
Grade: {grade}

Output ONLY the JSON.\
"""


# ---------------------------------------------------------------------------
# Ollama HTTP client (no extra packages — uses only `requests`)
# ---------------------------------------------------------------------------
def _call_ollama(user_message: str, attempt: int = 1) -> str:
    """
    Send a chat request to the local Ollama server.
    Returns the raw text response from the model.
    Raises RuntimeError if the server is unreachable or returns an error.
    """
    try:
        import requests
    except ImportError:
        raise RuntimeError(
            "The 'requests' package is not installed.\n"
            "Run:  pip install requests"
        )

    url = f"{OLLAMA_HOST}/api/chat"
    payload = {
        "model":  OLLAMA_MODEL,
        "stream": False,
        "format": "json",    # Ollama JSON mode — forces valid JSON output
        "messages": [
            {"role": "system",  "content": SYSTEM_PROMPT},
            {"role": "user",    "content": user_message},
        ],
        "options": {
            "temperature": 0.3 if attempt == 1 else 0.1,  # lower temp on retry
            "num_predict": 4096,
        },
    }

    try:
        resp = requests.post(url, json=payload, timeout=TIMEOUT_SEC)
    except requests.exceptions.ConnectionError:
        raise RuntimeError(
            f"Cannot connect to Ollama at {OLLAMA_HOST}.\n"
            "Make sure Ollama is running:  ollama serve\n"
            f"And the model is pulled:     ollama pull {OLLAMA_MODEL}"
        )
    except requests.exceptions.Timeout:
        raise RuntimeError(
            f"Ollama request timed out after {TIMEOUT_SEC}s. "
            "Try a smaller model: export OLLAMA_MODEL=phi3.5"
        )

    if resp.status_code != 200:
        raise RuntimeError(
            f"Ollama returned HTTP {resp.status_code}: {resp.text[:300]}"
        )

    data = resp.json()
    return data["message"]["content"]


# ---------------------------------------------------------------------------
# JSON extraction — handles edge cases where model wraps output despite instructions
# ---------------------------------------------------------------------------
def _extract_json(text: str) -> str:
    """
    Try to pull a valid JSON object out of `text`.
    1. If it parses directly → done.
    2. Strip markdown fences → try again.
    3. Regex-find the outermost { ... } block → try again.
    Raises json.JSONDecodeError if nothing works.
    """
    # Attempt 1: raw
    try:
        json.loads(text)
        return text
    except json.JSONDecodeError:
        pass

    # Attempt 2: strip markdown fences
    stripped = text.strip()
    if stripped.startswith("```"):
        lines = stripped.splitlines()
        inner = "\n".join(lines[1:])
        if inner.rstrip().endswith("```"):
            inner = inner.rstrip()[:-3]
        try:
            json.loads(inner)
            return inner
        except json.JSONDecodeError:
            pass

    # Attempt 3: find outermost { ... } block
    match = re.search(r'\{[\s\S]*\}', text)
    if match:
        candidate = match.group(0)
        json.loads(candidate)   # raises if still invalid
        return candidate

    # Nothing worked — raise so the caller can retry
    raise json.JSONDecodeError("No valid JSON object found", text, 0)


# ---------------------------------------------------------------------------
# Main generation function (used by both CLI and backend)
# ---------------------------------------------------------------------------
def generate_scene(prompt: str, subject: str, grade: int) -> str:
    """
    Generate a scene JSON string using the local Ollama model.
    Retries up to MAX_RETRIES times on JSON parse failure.
    Returns a valid JSON string or raises RuntimeError.
    """
    user_msg = USER_TEMPLATE.format(
        prompt=prompt,
        subject=subject or "general",
        grade=grade,
    )

    last_error = None
    for attempt in range(1, MAX_RETRIES + 1):
        if attempt > 1:
            print(f"[call_claude_api] Retry {attempt}/{MAX_RETRIES}…", file=sys.stderr)
            time.sleep(1)

        try:
            raw = _call_ollama(user_msg, attempt)
            return _extract_json(raw)
        except json.JSONDecodeError as e:
            last_error = f"JSON parse failed on attempt {attempt}: {e}"
            print(f"[call_claude_api] {last_error}", file=sys.stderr)
            # Add an extra nudge to the user message on next try
            user_msg = (
                f"{USER_TEMPLATE.format(prompt=prompt, subject=subject or 'general', grade=grade)}\n"
                "IMPORTANT: Your response must be ONLY a JSON object. Start with {{ and end with }}."
            )
        except RuntimeError as e:
            # Server / network errors are not worth retrying
            raise

    raise RuntimeError(
        f"Failed to get valid JSON after {MAX_RETRIES} attempts.\n"
        f"Last error: {last_error}\n"
        f"Try a more capable model: export OLLAMA_MODEL=llama3.1:70b"
    )


# ---------------------------------------------------------------------------
# CLI entry point (called by PromptSystem.cpp via popen)
# ---------------------------------------------------------------------------
def main():
    if len(sys.argv) < 2:
        print("Usage: python3 call_claude_api.py <config_json_file>", file=sys.stderr)
        print('Config format: { "prompt": "...", "subject": "...", "grade": 5 }', file=sys.stderr)
        sys.exit(1)

    config_path = sys.argv[1]
    try:
        with open(config_path) as f:
            cfg = json.load(f)
    except (FileNotFoundError, json.JSONDecodeError) as e:
        print(f"[call_claude_api] Cannot read config '{config_path}': {e}", file=sys.stderr)
        sys.exit(1)

    prompt  = cfg.get("prompt",  "educational platformer game")
    subject = cfg.get("subject", "")
    grade   = int(cfg.get("grade", 5))

    # Quick check: can we import requests?
    try:
        import requests  # noqa: F401
    except ImportError:
        print(
            "[call_claude_api] 'requests' package not found.\n"
            "Run: pip install requests",
            file=sys.stderr,
        )
        sys.exit(1)

    # Quick connectivity check before spending time on generation
    try:
        import requests
        r = requests.get(f"{OLLAMA_HOST}/api/tags", timeout=5)
        if r.status_code != 200:
            raise ConnectionError()
    except Exception:
        print(
            f"[call_claude_api] Ollama server not reachable at {OLLAMA_HOST}.\n"
            "Start it with:  ollama serve\n"
            f"Pull the model: ollama pull {OLLAMA_MODEL}",
            file=sys.stderr,
        )
        sys.exit(1)

    try:
        scene_json = generate_scene(prompt, subject, grade)
        print(scene_json)
    except RuntimeError as e:
        print(f"[call_claude_api] {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"[call_claude_api] Unexpected error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
