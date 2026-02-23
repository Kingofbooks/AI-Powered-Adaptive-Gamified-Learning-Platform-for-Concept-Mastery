from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from typing import Optional, Dict, Any
import json
import os
from dotenv import load_dotenv
from teacher_ai_agent import TeacherAIAgent

# Load environment variables
load_dotenv()

# Initialize FastAPI app
app = FastAPI(
    title="EduEngine AI Agent API",
    description="AI-powered agent that generates EduEngine scene JSON for educational platformer games",
    version="2.0.0"
)

# Add CORS middleware
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Initialize AI Agent
ai_agent = TeacherAIAgent()

# Pydantic models for request/response
class SceneGenerationRequest(BaseModel):
    """Request to generate an EduEngine scene"""
    prompt: str
    grade: Optional[str] = None
    subject: Optional[str] = None

class HealthResponse(BaseModel):
    status: str
    message: str

# Routes
@app.get("/health", response_model=HealthResponse)
async def health_check():
    """Check API health"""
    return {
        "status": "healthy",
        "message": "EduEngine AI Agent API is running"
    }

@app.post("/generate-scene")
async def generate_scene(request: SceneGenerationRequest):
    """
    🎮 **Generate EduEngine Scene JSON**
    
    Creates a complete interactive 2D educational platformer scene matching the EduEngine specification.
    
    **What it does:**
    - Analyzes the educational prompt to detect subject and grade level
    - Generates a complete scene JSON with platforms, collectibles, enemies, and educational content
    - Returns valid EduEngine format ready to load in the game engine
    
    **Parameters:**
    - `prompt` (required): Natural language description (e.g., "teach photosynthesis for grade 5", "space adventure for year 3")
    - `grade` (optional): Override detected grade level (1-12)
    - `subject` (optional): Override detected subject
    
    **Example Request:**
    ```json
    {
        "prompt": "create a photosynthesis game for grade 5 students"
    }
    ```
    
    **Example Response:**
    ```json
    {
        "success": true,
        "data": {
            "scene_meta": {
                "id": "photosynthesis_grade5",
                "title": "Photosynthesis — Grade 5",
                "subject": "photosynthesis",
                "grade": "5",
                "ai_prompt": "..."
            },
            "world_settings": {...},
            "entities": [...]
        }
    }
    ```
    """
    try:
        # Generate the scene
        scene = ai_agent.generate_scene(request.prompt)
        
        return {
            "success": True,
            "data": scene,
            "message": f"Successfully generated EduEngine scene for: {request.prompt}"
        }
    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=f"Error generating scene: {str(e)}"
        )

@app.post("/generate-scene-file")
async def generate_scene_file(request: SceneGenerationRequest):
    """
    💾 **Generate and Save EduEngine Scene File**
    
    Generates a scene and saves it as a JSON file in the data/scenes directory.
    
    **Parameters:**
    - `prompt` (required): Natural language description of the scene
    
    **Response:**
    Returns the file path where the scene was saved.
    """
    try:
        # Generate the scene
        scene = ai_agent.generate_scene(request.prompt)
        
        # Ensure data/scenes directory exists
        scenes_dir = "data/scenes"
        os.makedirs(scenes_dir, exist_ok=True)
        
        # Generate filename from scene ID
        scene_id = scene.get("scene_meta", {}).get("id", "scene")
        filename = f"{scene_id}.json"
        filepath = os.path.join(scenes_dir, filename)
        
        # Save the scene
        with open(filepath, 'w') as f:
            json.dump(scene, f, indent=2)
        
        return {
            "success": True,
            "filepath": filepath,
            "scene_id": scene_id,
            "data": scene,
            "message": f"Scene saved to {filepath}"
        }
    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=f"Error generating/saving scene: {str(e)}"
        )

@app.get("/themes")
async def list_themes():
    """
    📋 **List Available Educational Themes**
    
    Returns all available educational themes that the AI can generate scenes for.
    """
    from teacher_ai_agent import THEME_CONFIG
    
    themes = {}
    for theme_name, config in THEME_CONFIG.items():
        themes[theme_name] = {
            "slug": config["slug"],
            "collectibles": config["collectibles"],
            "keywords": config["keywords"]
        }
    
    return {
        "success": True,
        "themes": themes,
        "total_themes": len(themes)
    }

@app.get("/theme/{theme_name}")
async def get_theme_details(theme_name: str):
    """
    🎨 **Get Theme Details**
    
    Returns detailed information about a specific educational theme.
    
    **Parameters:**
    - `theme_name`: Name of the theme (e.g., "photosynthesis", "space", "biology")
    """
    from teacher_ai_agent import THEME_CONFIG
    
    if theme_name not in THEME_CONFIG:
        raise HTTPException(
            status_code=404,
            detail=f"Theme '{theme_name}' not found"
        )
    
    config = THEME_CONFIG[theme_name]
    
    return {
        "success": True,
        "theme": theme_name,
        "details": {
            "slug": config["slug"],
            "background_color": config["background_color"],
            "platform_color": config["platform_color"],
            "collectible_color": config["collectible_color"],
            "collectibles": config["collectibles"],
            "facts": config["facts"],
            "keywords": config["keywords"]
        }
    }

@app.post("/validate-scene")
async def validate_scene(scene: Dict[str, Any]):
    """
    ✅ **Validate EduEngine Scene JSON**
    
    Validates that a scene JSON matches the EduEngine specification.
    Returns detailed validation results.
    
    **Parameters:**
    - `scene`: The scene JSON object to validate
    """
    try:
        errors = []
        warnings = []
        
        # Check top-level structure
        if "scene_meta" not in scene:
            errors.append("Missing 'scene_meta'")
        if "world_settings" not in scene:
            errors.append("Missing 'world_settings'")
        if "entities" not in scene:
            errors.append("Missing 'entities'")
        
        # Check scene_meta
        if "scene_meta" in scene:
            meta = scene["scene_meta"]
            required_meta_fields = ["id", "title", "subject", "grade", "ai_prompt"]
            for field in required_meta_fields:
                if field not in meta:
                    errors.append(f"Missing scene_meta.{field}")
        
        # Check entities
        if "entities" in scene:
            entities = scene["entities"]
            
            # Count entity types
            backgrounds = [e for e in entities if e.get("tags") == ["background"]]
            players = [e for e in entities if e.get("tags") == ["player"]]
            goals = [e for e in entities if e.get("tags") == ["goal"]]
            grounds = [e for e in entities if "platform" in e.get("tags", []) and e.get("name") == "Ground"]
            
            if len(backgrounds) != 1:
                errors.append(f"Must have exactly 1 Background, found {len(backgrounds)}")
            if len(players) != 1:
                errors.append(f"Must have exactly 1 Player, found {len(players)}")
            if len(goals) != 1:
                errors.append(f"Must have exactly 1 Goal, found {len(goals)}")
            if len(grounds) != 1:
                errors.append(f"Must have exactly 1 Ground, found {len(grounds)}")
            
            # Check for entity bounds
            for entity in entities:
                if "components" in entity and "transform" in entity["components"]:
                    transform = entity["components"]["transform"]
                    x = transform.get("x", 0)
                    y = transform.get("y", 0)
                    if x < 0 or x > 1280:
                        warnings.append(f"{entity.get('name', 'Unknown')} x={x} is outside world bounds [0-1280]")
                    if y < 0 or y > 720:
                        warnings.append(f"{entity.get('name', 'Unknown')} y={y} is outside world bounds [0-720]")
        
        return {
            "success": len(errors) == 0,
            "valid": len(errors) == 0,
            "errors": errors,
            "warnings": warnings,
            "error_count": len(errors),
            "warning_count": len(warnings)
        }
    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=f"Error validating scene: {str(e)}"
        )

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8001)
