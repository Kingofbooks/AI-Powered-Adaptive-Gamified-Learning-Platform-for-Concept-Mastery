import json
import os
import re
from typing import Optional, Dict, Any
from dotenv import load_dotenv
from openai import OpenAI
import requests

# Try to import Gemini (backup)
try:
    import google.generativeai as genai
    GEMINI_AVAILABLE = True
except ImportError:
    GEMINI_AVAILABLE = False

# Try to import Groq (tertiary)
try:
    from groq import Groq
    GROQ_AVAILABLE = True
except ImportError:
    GROQ_AVAILABLE = False

# Load environment variables from .env file
load_dotenv()

# Theme configuration
THEME_CONFIG = {
    "photosynthesis": {
        "slug": "photosynthesis",
        "background_color": [12, 28, 12, 255],
        "platform_color": [30, 90, 30, 255],
        "collectible_color": [80, 220, 80, 220],
        "keywords": ["photosyn", "chloro", "plant", "leaf", "glucose", "chlorophyll"],
        "collectibles": ["CO₂", "H₂O", "Sunlight", "Glucose", "O₂", "Chlorophyll"],
        "facts": [
            "Photosynthesis converts light energy into chemical energy stored as glucose.",
            "CO₂ enters leaves through tiny pores called stomata.",
            "Chlorophyll is the green pigment in leaves that captures sunlight.",
            "Oxygen (O₂) is released as a byproduct of photosynthesis."
        ]
    },
    "space": {
        "slug": "space",
        "background_color": [5, 5, 25, 255],
        "platform_color": [40, 40, 100, 255],
        "collectible_color": [220, 180, 60, 220],
        "keywords": ["solar", "planet", "space", "astro", "star", "galaxy", "orbit", "moon", "nasa"],
        "collectibles": ["Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Star"],
        "facts": [
            "The Sun is a medium-sized star at the centre of our solar system.",
            "Mars has the largest volcano: Olympus Mons at 22 km high.",
            "Jupiter is so large that all other planets could fit inside it.",
            "Light from the Sun takes 8 minutes to reach Earth."
        ]
    },
    "water_cycle": {
        "slug": "water_cycle",
        "background_color": [10, 20, 50, 255],
        "platform_color": [30, 60, 120, 255],
        "collectible_color": [100, 180, 255, 220],
        "keywords": ["water cycle", "evapor", "precipit", "condensat", "rain", "cloud", "river"],
        "collectibles": ["Evaporation", "Condensation", "Precipitation", "Runoff", "Transpiration"],
        "facts": [
            "Water evaporates from oceans when heated by the Sun.",
            "Water vapour condenses into clouds as it rises and cools.",
            "Precipitation returns water to Earth as rain, snow, or hail.",
            "Plants release water vapour through transpiration."
        ]
    },
    "mathematics": {
        "slug": "mathematics",
        "background_color": [20, 15, 30, 255],
        "platform_color": [60, 40, 100, 255],
        "collectible_color": [255, 200, 50, 220],
        "keywords": ["math", "algebra", "geometr", "fraction", "calculus", "equation", "number", "pi"],
        "collectibles": ["π", "∑", "∫", "√", "2²", "x²", "Δ"],
        "facts": [
            "Pi (π) ≈ 3.14159 is the ratio of a circle's circumference to diameter.",
            "The Pythagorean theorem: a² + b² = c².",
            "A prime number has only two factors: 1 and itself.",
            "Zero was invented in ancient India around 500 CE."
        ]
    },
    "history": {
        "slug": "history",
        "background_color": [30, 20, 10, 255],
        "platform_color": [80, 55, 25, 255],
        "collectible_color": [200, 160, 60, 220],
        "keywords": ["history", "ancient", "civil", "war", "empire", "roman", "egypt", "medieval"],
        "collectibles": ["Scroll", "Crown", "Shield", "Sword", "Map", "Coin"],
        "facts": [
            "Ancient Egypt built the Great Pyramid around 2560 BCE.",
            "The Roman Empire lasted from 27 BCE to 476 CE.",
            "The Renaissance was a cultural rebirth from the 14th to 17th century.",
            "The Industrial Revolution began in Britain around 1760 CE."
        ]
    },
    "chemistry": {
        "slug": "chemistry",
        "background_color": [10, 5, 25, 255],
        "platform_color": [60, 20, 80, 255],
        "collectible_color": [120, 220, 180, 220],
        "keywords": ["chemistry", "element", "atom", "molecule", "periodic", "reaction", "compound"],
        "collectibles": ["H₂O", "O₂", "NaCl", "CO₂", "Fe", "Au", "Atom"],
        "facts": [
            "The periodic table has 118 confirmed elements.",
            "An atom consists of protons, neutrons, and electrons.",
            "Water (H₂O) is called a universal solvent.",
            "Gold (Au) is a noble metal and does not corrode."
        ]
    },
    "physics": {
        "slug": "physics",
        "background_color": [5, 10, 30, 255],
        "platform_color": [20, 50, 120, 255],
        "collectible_color": [60, 180, 255, 220],
        "keywords": ["physics", "force", "energy", "gravity", "velocity", "motion", "newton", "wave"],
        "collectibles": ["Force", "Energy", "Mass", "Velocity", "Gravity", "Wave"],
        "facts": [
            "Newton's First Law: an object at rest stays at rest unless acted on.",
            "E = mc² relates mass and energy (Einstein, 1905).",
            "Gravity accelerates objects at 9.8 m/s² on Earth.",
            "Sound travels at approximately 343 m/s through air."
        ]
    },
    "biology": {
        "slug": "biology",
        "background_color": [10, 25, 15, 255],
        "platform_color": [25, 70, 35, 255],
        "collectible_color": [100, 240, 130, 220],
        "keywords": ["biology", "cell", "dna", "animal", "plant", "organ", "mitosis", "gene", "protein"],
        "collectibles": ["DNA", "Cell", "Protein", "Gene", "Enzyme", "Nucleus"],
        "facts": [
            "The human body has approximately 37 trillion cells.",
            "DNA is a double helix made of base pairs: A-T and G-C.",
            "The cell membrane controls what enters and exits the cell.",
            "Mitosis produces two identical daughter cells from one parent cell."
        ]
    },
    "geography": {
        "slug": "geography",
        "background_color": [5, 20, 40, 255],
        "platform_color": [20, 80, 40, 255],
        "collectible_color": [60, 180, 100, 220],
        "keywords": ["geograph", "continent", "climate", "ocean", "mountain", "river", "country"],
        "collectibles": ["Africa", "Asia", "Europe", "Amazon", "Equator", "Mountain"],
        "facts": [
            "Earth has 7 continents and 5 major oceans.",
            "The Amazon rainforest covers over 5.5 million km².",
            "The Sahara is the world's largest hot desert.",
            "The Mariana Trench is the deepest point on Earth at ~11 km."
        ]
    },
    "default": {
        "slug": "default",
        "background_color": [18, 12, 30, 255],
        "platform_color": [55, 35, 90, 255],
        "collectible_color": [200, 120, 255, 220],
        "keywords": [],
        "collectibles": ["⭐", "💡", "📚", "🔬", "🏆", "🎯"],
        "facts": [
            "Keep exploring and never stop asking questions!",
            "Every expert was once a beginner.",
            "Learning is the most powerful tool you have.",
            "Curiosity is the engine of discovery."
        ]
    }
}

class TeacherAIAgent:
    """AI Agent that generates EduEngine scene JSON for interactive educational platformers"""
    
    def __init__(self, api_key: Optional[str] = None, gemini_key: Optional[str] = None, groq_key: Optional[str] = None):
        # OpenAI Configuration
        self.openai_api_key = api_key or os.getenv("OPENAI_API_KEY")
        self.openai_available = bool(self.openai_api_key) and "your_openai" not in self.openai_api_key.lower()
        
        if self.openai_available:
            self.openai_client = OpenAI(api_key=self.openai_api_key)
            print("✅ OpenAI client initialized")
        else:
            self.openai_client = None
            print("⚠️ OpenAI key not configured (using Gemini/Groq/Ollama fallback)")
        
        # Gemini Configuration (Secondary Backup)
        self.gemini_api_key = gemini_key or os.getenv("GEMINI_API_KEY")
        self.gemini_available = GEMINI_AVAILABLE and bool(self.gemini_api_key)
        
        if self.gemini_available:
            genai.configure(api_key=self.gemini_api_key)
            self.gemini_model = genai.GenerativeModel('gemini-2.0-flash')
            print("✅ Gemini client initialized (secondary backup - free tier)")
        else:
            self.gemini_model = None
            print("⚠️ Gemini key not configured")
        
        # Groq Configuration (Tertiary Backup - Free)
        self.groq_api_key = groq_key or os.getenv("GROQ_API_KEY")
        self.groq_available = GROQ_AVAILABLE and bool(self.groq_api_key)
        
        if self.groq_available:
            self.groq_client = Groq(api_key=self.groq_api_key)
            print("✅ Groq client initialized (tertiary backup - free tier)")
        else:
            self.groq_client = None
            print("⚠️ Groq key not configured")
        
        # Ollama Configuration (Quaternary - Local)
        self.ollama_url = os.getenv("OLLAMA_URL", "http://localhost:11434/api/generate")
        self.ollama_model = os.getenv("OLLAMA_MODEL", "phi3")
        self.ollama_available = self._check_ollama_available()
        
        if self.ollama_available:
            print(f"✅ Ollama available ({self.ollama_model} at {self.ollama_url})")
        else:
            print(f"⚠️ Ollama not available at {self.ollama_url}")
    
    def _check_ollama_available(self) -> bool:
        """Check if Ollama service is running"""
        try:
            response = requests.get(self.ollama_url.replace("/api/generate", ""), timeout=5)
            return response.status_code < 500
        except Exception as e:
            print(f"  (Ollama health check: {str(e)[:50]})")
            return False
        
    def detect_theme(self, prompt: str) -> str:
        """Detect theme from prompt keywords"""
        prompt_lower = prompt.lower()
        for theme_name, config in THEME_CONFIG.items():
            if theme_name == "default":
                continue
            for keyword in config["keywords"]:
                if keyword.lower() in prompt_lower:
                    return theme_name
        return "default"
    
    def detect_grade(self, prompt: str) -> str:
        """Extract grade level from prompt"""
        import re
        # Look for patterns like "grade 5", "year 3", "5th grade", etc.
        patterns = [
            r'grade\s+(\d+)',
            r'year\s+(\d+)',
            r'(\d+)(?:st|nd|rd|th)\s+grade',
            r'level\s+(\d+)',
        ]
        for pattern in patterns:
            match = re.search(pattern, prompt.lower())
            if match:
                grade = int(match.group(1))
                return str(min(max(grade, 1), 12))  # Clamp to 1-12
        return "5"  # Default to grade 5
    
    def calculate_level_params(self, grade: int) -> Dict[str, int]:
        """Calculate level design parameters based on grade"""
        grade_num = int(grade)
        num_platforms = min(max(3 + (grade_num // 3), 4), 7)
        num_enemies = 0 if grade_num < 5 else min((grade_num // 3), 3)
        num_info_signs = 3

        return {
            "num_platforms": num_platforms,
            "num_enemies": num_enemies,
            "num_info_signs": num_info_signs
        }
    
    def generate_scene(self, prompt: str) -> Dict[str, Any]:
        """
        Generate a complete EduEngine scene JSON based on a natural language prompt
        
        Args:
            prompt: Natural language description of the educational game
                   Example: "teach photosynthesis for grade 5"
        
        Returns:
            Complete scene JSON as dictionary
        """
        # Detect theme and grade
        theme = self.detect_theme(prompt)
        grade = self.detect_grade(prompt)
        theme_config = THEME_CONFIG[theme]
        level_params = self.calculate_level_params(grade)
        
        # Build level content counts
        num_collectibles = level_params['num_platforms'] + 2
        collectibles_list = theme_config['collectibles']
        facts_list = theme_config['facts']

        # Create the AI prompt - no // comments inside JSON, use explicit examples instead
        ai_prompt = f"""You are a JSON generator for EduEngine, a 2D educational platformer game engine.
Generate a complete EduEngine scene JSON for: "{prompt}"

OUTPUT RULES:
- Return ONLY valid JSON starting with {{ and ending with }}
- No markdown, no code fences, NO comments (JSON does not support // comments)
- All texture paths: "assets/images/{theme}/<filename>.png"
- The "grade" field must be a STRING: "{grade}" (not a number)

REQUIRED TOP-LEVEL STRUCTURE:
{{
  "scene_meta": {{"id":"{theme}_grade{grade}","title":"{theme.replace('_',' ').title()} — Grade {grade}","subject":"{theme}","grade":"{grade}","ai_prompt":"{prompt}"}},
  "world_settings": {{"gravity":980.0,"background_color":{theme_config['background_color']}}},
  "entities": [ ... see below ... ]
}}

REQUIRED ENTITIES — include ALL of these in this order:

1. BACKGROUND (exactly 1, first in array, no collider):
{{"name":"Background","tags":["background"],"components":{{"transform":{{"x":0,"y":0,"scale_x":1.0,"scale_y":1.0}},"sprite":{{"texture":"assets/images/{theme}/background.png","color":[255,255,255,255],"layer":0}}}}}}

2. PLAYER (exactly 1, x=80, y=520):
{{"name":"Player","tags":["player"],"components":{{"transform":{{"x":80,"y":520,"scale_x":1.5,"scale_y":1.5}},"sprite":{{"texture":"assets/images/{theme}/player.png","color":[255,255,255,255],"layer":5}},"rigid_body":{{"mass":1.0,"gravity_scale":1.0}},"collider":{{"offset_x":4,"width":24,"height":32}},"player_tag":{{"move_speed":220,"jump_force":-580,"max_jumps":2}}}}}}

3. GROUND (exactly 1, x=0, y=620):
{{"name":"Ground","tags":["platform"],"components":{{"transform":{{"x":0,"y":620,"scale_x":40.0,"scale_y":5.0}},"sprite":{{"texture":"assets/images/{theme}/platform.png","color":{theme_config['platform_color']},"layer":2}},"collider":{{"width":1280,"height":100}},"platform":{{"is_one_way":false}}}}}}

4. FLOATING PLATFORMS (generate exactly {level_params['num_platforms']}):
Spread x from 120 to 1100. Y range 280-580. Alternate heights.
scale_x = collider_width / 32.0. Use is_one_way=true. No rigid_body.
Example: {{"name":"Platform_1","tags":["platform"],"components":{{"transform":{{"x":120,"y":480,"scale_x":5.0,"scale_y":1.0}},"sprite":{{"texture":"assets/images/{theme}/platform.png","color":{theme_config['platform_color']},"layer":2}},"collider":{{"width":160,"height":20}},"platform":{{"is_one_way":true}}}}}}

5. COLLECTIBLES (generate exactly {num_collectibles}, place above platforms, y 260-540):
Use is_trigger=true. Cycle collectible_0.png through collectible_4.png.
Labels from this list: {', '.join(collectibles_list)}
Example: {{"name":"Collect_{collectibles_list[0]}","tags":["collectible"],"components":{{"transform":{{"x":220,"y":420}},"sprite":{{"texture":"assets/images/{theme}/collectible_0.png","color":{theme_config['collectible_color']},"layer":4}},"collider":{{"width":28,"height":28,"is_trigger":true}},"collectible":{{"point_value":15,"label":"{collectibles_list[0]}"}},"text_label":{{"text":"{collectibles_list[0]}","color":[255,255,200,255],"font_size":13,"layer":6}}}}}}

6. INFO SIGNS (generate exactly 3, y=590, no texture in sprite):
Use x positions 380, 680, 950. is_trigger=true.
Facts to use: "{facts_list[0]}" | "{facts_list[1]}" | "{facts_list[2] if len(facts_list) > 2 else facts_list[0]}"
Example: {{"name":"InfoSign_1","tags":["info"],"components":{{"transform":{{"x":380,"y":590}},"sprite":{{"color":[220,180,60,220],"layer":3}},"collider":{{"width":36,"height":36,"is_trigger":true}},"text_label":{{"text":"?","color":[30,20,5,255],"font_size":20,"layer":5}},"info_zone":{{"text":"{facts_list[0]}","repeat":false}}}}}}

7. ENEMIES (generate exactly {level_params['num_enemies']}, only for grade {grade} >= 5, y=588):
Include rigid_body. patrol_min_x = x-20, patrol_max_x = x+150.
Example: {{"name":"Enemy_1","tags":["enemy"],"components":{{"transform":{{"x":450,"y":588}},"sprite":{{"texture":"assets/images/{theme}/enemy.png","color":[255,255,255,230],"layer":4}},"collider":{{"width":30,"height":32}},"rigid_body":{{"mass":1.0,"gravity_scale":1.0}},"enemy":{{"patrol_min_x":430.0,"patrol_max_x":600.0,"speed":80.0}}}}}}

8. GOAL FLAG (exactly 1, x=1160, y=558):
{{"name":"Goal_Flag","tags":["goal"],"components":{{"transform":{{"x":1160,"y":558}},"sprite":{{"texture":"assets/images/{theme}/goal.png","color":{theme_config['collectible_color']},"layer":4}},"collider":{{"width":40,"height":60,"is_trigger":true}},"goal":{{}},"text_label":{{"text":"GOAL","color":[255,255,255,255],"layer":6}}}}}}

STRICT RULES:
- All entity names must be unique
- x values: 0 to 1280, y values: 0 to 720
- Platforms: is_trigger absent or false (solid collision)
- Collectibles: is_trigger MUST be true
- Background: NO collider component
- Enemies: only if grade >= 5, must have rigid_body
- Do NOT put rigid_body on platforms

Generate the full entities array now. Return ONLY the complete valid JSON object."""

        try:
            # Skip OpenAI if key is not configured
            if not self.openai_available:
                raise ValueError("OpenAI key not configured, using Gemini")
            
            response = self.openai_client.chat.completions.create(
                model="gpt-4o",
                max_tokens=4000,
                messages=[{"role": "user", "content": ai_prompt}]
            )
            
            response_text = response.choices[0].message.content.strip()
            
            # Try to parse JSON
            try:
                scene_data = json.loads(response_text)
            except json.JSONDecodeError:
                # Try to extract JSON from the response
                json_match = re.search(r'\{.*\}', response_text, re.DOTALL)
                if json_match:
                    scene_data = json.loads(json_match.group())
                else:
                    raise ValueError(f"Could not extract valid JSON from response: {response_text[:200]}")
            
            # Validate and fix the scene
            scene_data = self._validate_and_fix_scene(scene_data, theme, grade, theme_config)
            print(f"✅ OpenAI generated scene successfully")
            
            return scene_data
            
        except Exception as e_openai:
            print(f"⚠️ OpenAI error: {e_openai}")
            
            # Try Gemini as fallback
            if self.gemini_available:
                try:
                    print("🔄 Falling back to Gemini...")
                    response = self.gemini_model.generate_content(ai_prompt)
                    response_text = response.text.strip()
                    
                    # Try to parse JSON
                    try:
                        scene_data = json.loads(response_text)
                    except json.JSONDecodeError:
                        # Try to extract JSON from the response
                        json_match = re.search(r'\{.*\}', response_text, re.DOTALL)
                        if json_match:
                            scene_data = json.loads(json_match.group())
                        else:
                            raise ValueError(f"Could not extract valid JSON from Gemini response: {response_text[:200]}")
                    
                    # Validate and fix the scene
                    scene_data = self._validate_and_fix_scene(scene_data, theme, grade, theme_config)
                    print(f"✅ Gemini generated scene successfully")
                    
                    return scene_data
                    
                except Exception as e_gemini:
                    print(f"❌ Gemini error: {e_gemini}")
            else:
                print("⚠️ Gemini not available as fallback")
            
            # Try Groq as tertiary fallback (free tier)
            if self.groq_available:
                try:
                    print("🔄 Falling back to Groq...")
                    response = self.groq_client.chat.completions.create(
                        model="llama-3.1-70b-versatile",
                        max_tokens=4000,
                        messages=[{"role": "user", "content": ai_prompt}]
                    )
                    response_text = response.choices[0].message.content.strip()
                    
                    # Try to parse JSON
                    try:
                        scene_data = json.loads(response_text)
                    except json.JSONDecodeError:
                        # Try to extract JSON from the response
                        json_match = re.search(r'\{.*\}', response_text, re.DOTALL)
                        if json_match:
                            scene_data = json.loads(json_match.group())
                        else:
                            raise ValueError(f"Could not extract valid JSON from Groq response: {response_text[:200]}")
                    
                    # Validate and fix the scene
                    scene_data = self._validate_and_fix_scene(scene_data, theme, grade, theme_config)
                    print(f"✅ Groq generated scene successfully")
                    
                    return scene_data
                    
                except Exception as e_groq:
                    print(f"❌ Groq error: {e_groq}")
            else:
                print("⚠️ Groq not available as fallback")
            
            # Try Ollama as quaternary fallback (local)
            if self.ollama_available:
                try:
                    print(f"🔄 Falling back to Ollama ({self.ollama_model})...")
                    print("⏳ (Ollama may take 1-3 minutes first time... please wait)")
                    
                    # Import timeout from config
                    from config import OLLAMA_TIMEOUT
                    
                    response = requests.post(
                        self.ollama_url,
                        json={
                            "model": self.ollama_model,
                            "prompt": ai_prompt,
                            "stream": False
                        },
                        timeout=OLLAMA_TIMEOUT  # 300 seconds = 5 minutes for local LLM
                    )
                    
                    response.raise_for_status()
                    response_text = response.json().get("response", "").strip()
                    
                    # Try to parse JSON
                    try:
                        scene_data = json.loads(response_text)
                    except json.JSONDecodeError:
                        # Try to extract JSON from the response
                        json_match = re.search(r'\{.*\}', response_text, re.DOTALL)
                        if json_match:
                            scene_data = json.loads(json_match.group())
                        else:
                            raise ValueError(f"Could not extract valid JSON from Ollama response: {response_text[:200]}")
                    
                    # Validate and fix the scene
                    scene_data = self._validate_and_fix_scene(scene_data, theme, grade, theme_config)
                    print(f"✅ Ollama generated scene successfully")
                    
                    return scene_data
                    
                except Exception as e_ollama:
                    print(f"❌ Ollama error: {e_ollama}")
            else:
                print("⚠️ Ollama not available - no local instance running at http://localhost:11434")
            
            # Return a minimal valid scene if all fail
            print("⚠️ Generating minimal fallback scene...")
            return self._create_minimal_scene(prompt, theme, grade, theme_config, level_params)
    
    def _validate_and_fix_scene(self, scene: Dict, theme: str, grade: str, theme_config: Dict) -> Dict:
        """Validate and fix scene data to ensure it matches specification"""
        if "scene_meta" not in scene:
            scene["scene_meta"] = {}
        if "world_settings" not in scene:
            scene["world_settings"] = {}
        if "entities" not in scene:
            scene["entities"] = []
        
        # Ensure scene_meta has required fields
        scene["scene_meta"]["subject"] = theme
        scene["scene_meta"]["grade"] = str(grade)
        if "title" not in scene["scene_meta"]:
            scene["scene_meta"]["title"] = f"{theme.replace('_', ' ').title()} — Grade {grade}"
        if "id" not in scene["scene_meta"]:
            scene["scene_meta"]["id"] = f"{theme}_grade{grade}".lower()
        
        # Ensure world_settings has required fields
        scene["world_settings"]["gravity"] = 980.0
        scene["world_settings"]["background_color"] = theme_config["background_color"]
        
        # Validate entities
        self._validate_entities(scene["entities"], theme, grade, theme_config)
        
        return scene
    
    def _validate_entities(self, entities: list, theme: str, grade: str, theme_config: Dict):
        """Validate and auto-repair entities to match specification"""

        # --- Normalize texture keys: AI sometimes generates "texture_path" instead of "texture" ---
        for entity in entities:
            sprite = entity.get("components", {}).get("sprite", None)
            if sprite and "texture_path" in sprite and "texture" not in sprite:
                sprite["texture"] = sprite.pop("texture_path")

        # --- Check for required entities ---
        has_background = any("background" in e.get("tags", []) for e in entities)
        has_player = any("player" in e.get("tags", []) for e in entities)
        has_ground = any(
            "platform" in e.get("tags", []) and e.get("name") == "Ground"
            for e in entities
        )
        has_goal = any("goal" in e.get("tags", []) for e in entities)

        # --- Auto-repair: insert missing required entities ---
        if not has_background:
            print("  [validator] Missing Background — inserting")
            entities.insert(0, self._create_background_entity(theme))
        if not has_player:
            print("  [validator] Missing Player — inserting")
            entities.insert(1, self._create_player_entity(theme))
        if not has_ground:
            print("  [validator] Missing Ground — inserting")
            entities.insert(2, self._create_ground_entity(theme, theme_config))
        if not has_goal:
            print("  [validator] Missing Goal_Flag — inserting")
            entities.append(self._create_goal_entity(theme, theme_config))

        # --- Clamp all entity positions to world bounds ---
        for entity in entities:
            transform = entity.get("components", {}).get("transform", None)
            if transform:
                transform["x"] = max(0, min(1280, transform.get("x", 0)))
                transform["y"] = max(0, min(720, transform.get("y", 0)))
    
    def _create_minimal_scene(self, prompt: str, theme: str, grade: str,
                             theme_config: Dict, level_params: Dict) -> Dict:
        """Create a complete fallback scene when AI generation fails — includes all entity types"""
        scene_id = f"{theme}_grade{grade}".lower()
        title = f"{theme.replace('_', ' ').title()} — Grade {grade}"
        grade_num = int(grade)

        # Fixed platform layout: (x, y, width)
        platform_layout = [
            (120, 480, 160),
            (340, 400, 140),
            (560, 330, 180),
            (780, 450, 130),
            (1000, 370, 170),
            (200, 310, 150),
            (650, 280, 160),
        ]
        collectibles_vocab = theme_config["collectibles"]
        facts = theme_config["facts"]

        entities = [
            self._create_background_entity(theme),
            self._create_player_entity(theme),
            self._create_ground_entity(theme, theme_config),
        ]

        # Add platforms
        num_platforms = min(level_params["num_platforms"], len(platform_layout))
        for i in range(num_platforms):
            px, py, pw = platform_layout[i]
            entities.append({
                "name": f"Platform_{i + 1}",
                "tags": ["platform"],
                "components": {
                    "transform": {"x": px, "y": py, "scale_x": round(pw / 32.0, 4), "scale_y": 1.0},
                    "sprite": {
                        "texture": f"assets/images/{theme}/platform.png",
                        "color": theme_config["platform_color"],
                        "layer": 2
                    },
                    "collider": {"width": pw, "height": 20},
                    "platform": {"is_one_way": True}
                }
            })

        # Add collectibles (num_platforms + 2, spread across level)
        num_collectibles = min(level_params["num_platforms"] + 2, len(collectibles_vocab))
        collectible_positions = [
            (150, 360), (370, 280), (590, 250), (810, 310),
            (1020, 290), (260, 420), (480, 380)
        ]
        for i in range(num_collectibles):
            label = collectibles_vocab[i % len(collectibles_vocab)]
            cx, cy = collectible_positions[i % len(collectible_positions)]
            safe_name = label.replace("₂", "2").replace("₁", "1").replace("²", "2").replace(" ", "_")
            entities.append({
                "name": f"Collect_{safe_name}",
                "tags": ["collectible"],
                "components": {
                    "transform": {"x": cx, "y": cy},
                    "sprite": {
                        "texture": f"assets/images/{theme}/collectible_{i % 5}.png",
                        "color": theme_config["collectible_color"],
                        "layer": 4
                    },
                    "collider": {"width": 28, "height": 28, "is_trigger": True},
                    "collectible": {"point_value": 15, "label": label},
                    "text_label": {"text": label, "color": [255, 255, 200, 255], "font_size": 13, "layer": 6}
                }
            })

        # Add info signs (3 signs, spread across level floor)
        info_x_positions = [380, 680, 950]
        for i in range(min(3, len(facts))):
            entities.append({
                "name": f"InfoSign_{i + 1}",
                "tags": ["info"],
                "components": {
                    "transform": {"x": info_x_positions[i], "y": 590},
                    "sprite": {"color": [220, 180, 60, 220], "layer": 3},
                    "collider": {"width": 36, "height": 36, "is_trigger": True},
                    "text_label": {"text": "?", "color": [30, 20, 5, 255], "font_size": 20, "layer": 5},
                    "info_zone": {"text": facts[i], "repeat": False}
                }
            })

        # Add enemies for grade 5+
        enemy_x_positions = [400, 700, 950]
        num_enemies = 0 if grade_num < 5 else min(level_params["num_enemies"], 3)
        for i in range(num_enemies):
            ex = enemy_x_positions[i]
            entities.append({
                "name": f"Enemy_{i + 1}",
                "tags": ["enemy"],
                "components": {
                    "transform": {"x": ex, "y": 588},
                    "sprite": {
                        "texture": f"assets/images/{theme}/enemy.png",
                        "color": [255, 255, 255, 230],
                        "layer": 4
                    },
                    "collider": {"width": 30, "height": 32},
                    "rigid_body": {"mass": 1.0, "gravity_scale": 1.0},
                    "enemy": {
                        "patrol_min_x": float(ex - 20),
                        "patrol_max_x": float(ex + 150),
                        "speed": 70.0 + i * 15
                    }
                }
            })

        entities.append(self._create_goal_entity(theme, theme_config))

        return {
            "scene_meta": {
                "id": scene_id,
                "title": title,
                "subject": theme,
                "grade": str(grade),
                "ai_prompt": prompt
            },
            "world_settings": {
                "gravity": 980.0,
                "background_color": theme_config["background_color"]
            },
            "entities": entities
        }
    
    def _create_background_entity(self, theme: str) -> Dict:
        return {
            "name": "Background",
            "tags": ["background"],
            "components": {
                "transform": {"x": 0, "y": 0, "scale_x": 1.0, "scale_y": 1.0},
                "sprite": {
                    "texture": f"assets/images/{theme}/background.png",
                    "color": [255, 255, 255, 255],
                    "layer": 0
                }
            }
        }
    
    def _create_player_entity(self, theme: str) -> Dict:
        return {
            "name": "Player",
            "tags": ["player"],
            "components": {
                "transform": {"x": 80, "y": 520, "scale_x": 1.5, "scale_y": 1.5},
                "sprite": {
                    "texture": f"assets/images/{theme}/player.png",
                    "color": [255, 255, 255, 255],
                    "layer": 5
                },
                "rigid_body": {"mass": 1.0, "gravity_scale": 1.0},
                "collider": {"offset_x": 4, "width": 24, "height": 32},
                "player_tag": {"move_speed": 220, "jump_force": -580, "max_jumps": 2}
            }
        }
    
    def _create_ground_entity(self, theme: str, theme_config: Dict) -> Dict:
        return {
            "name": "Ground",
            "tags": ["platform"],
            "components": {
                "transform": {"x": 0, "y": 620, "scale_x": 40.0, "scale_y": 5.0},
                "sprite": {
                    "texture": f"assets/images/{theme}/platform.png",
                    "color": theme_config["platform_color"],
                    "layer": 2
                },
                "collider": {"width": 1280, "height": 100},
                "platform": {"is_one_way": False}
            }
        }
    
    def _create_goal_entity(self, theme: str, theme_config: Dict) -> Dict:
        return {
            "name": "Goal_Flag",
            "tags": ["goal"],
            "components": {
                "transform": {"x": 1160, "y": 558},
                "sprite": {
                    "texture": f"assets/images/{theme}/goal.png",
                    "color": theme_config["collectible_color"],
                    "layer": 4
                },
                "collider": {"width": 40, "height": 60, "is_trigger": True},
                "goal": {},
                "text_label": {
                    "text": "GOAL",
                    "color": [255, 255, 255, 255],
                    "layer": 6
                }
            }
        }

