# Physics Scenario Integration Guide

## Overview
All 5 levels now have **complete physics scenarios configured** in the GameEngine. The scenarios define exactly which physics entities (balls, blocks, ramps, ground) should be created for each level.

---

## ✅ What's Already Done

### 1. Scenario Configurations
Every level has a `ScenarioSetup` with:
- List of entities (type, position, color, mass, size, velocity, label)
- Instruction text
- Pause-on-start flag

### 2. Level 1: Gravity Basics
```cpp
Entities:
- Red ball at position (200, 100), size 25, mass 1.0
- Gray ramp at position (400, 350), size 200
- Brown ground at position (600, 550), size 1000

Instruction: "Watch the ball roll down the ramp. Observe how gravity accelerates it."
```

### 3. Level 2: Collision and Energy
```cpp
Entities:
- Red ball "A" at (200, 300), velocity (100, 0), mass 1.0
- Blue ball "B" at (700, 300), velocity (-80, 0), mass 1.2
- Brown ground at (600, 550)

Instruction: "Watch the two balls collide. Observe momentum transfer."
```

### 4. Levels 3, 4, 5
Similarly configured with appropriate physics scenarios for:
- Work and energy demonstration
- Forces and motion
- Advanced physics concepts

---

## 🎮 How to View Configured Scenarios

### In the UI
1. Run `./EduSandbox`
2. Press **'G'** to enter Learning Mode
3. Look for the **purple glowing card** labeled "PHYSICS SCENARIO CONFIGURED"
4. See the instruction text and entity list with visual icons

### In Code
```cpp
// Get scenario for current level
ScenarioSetup scenario = gameEngine.getCurrentScenarioSetup();

// Access scenario data
std::string instruction = scenario.instruction;
bool shouldPause = scenario.pauseOnStart;
std::vector<ScenarioSetup::EntitySetup>& entities = scenario.entities;

// Example: Print all entities
for (const auto& e : entities) {
    std::cout << "Entity: " << e.type
              << " at (" << e.position.x << ", " << e.position.y << ")"
              << " color (" << (int)e.color.r << "," << (int)e.color.g << "," << (int)e.color.b << ")"
              << " label: " << e.label << std::endl;
}
```

---

## 🔧 How to Integrate Scenarios (If Modifying main.cpp)

### Method 1: Load Entities Manually
```cpp
// In main.cpp, when starting a level:
void loadLevelScenario(int levelId) {
    // Clear existing entities
    physicsEngine.clearAllEntities();

    // Get entities from GameEngine
    auto entities = gameEngine.getScenarioEntities(levelId);

    // Add to physics engine
    for (auto& entity : entities) {
        physicsEngine.addEntity(entity);
    }
}

// Call when changing levels:
if (/* user pressed number key */) {
    int newLevel = /* key pressed */;
    gameEngine.setCurrentLevel(newLevel);
    loadLevelScenario(newLevel);
}
```

### Method 2: Auto-Load on Level Change
```cpp
// In main.cpp, add to the level-switching code:
if (key == '1') {
    gameEngine.setCurrentLevel(1);
    auto entities = gameEngine.getScenarioEntities(1);
    physicsEngine.clearAllEntities();
    for (auto& e : entities) physicsEngine.addEntity(e);
}
// Repeat for keys '2', '3', '4', '5'
```

### Method 3: Load on Learning Mode Activation
```cpp
// When user presses 'G' to enter learning mode:
if (key == 'g') {
    learningMode = !learningMode;

    if (learningMode) {
        // Auto-load scenario for current level
        int currentLevel = gameEngine.getProgress().currentLevelId;
        auto entities = gameEngine.getScenarioEntities(currentLevel);

        physicsEngine.clearAllEntities();
        for (auto& e : entities) {
            physicsEngine.addEntity(e);
        }
    }
}
```

---

## 📋 Available Methods

### Get Scenario Entities
```cpp
std::vector<std::shared_ptr<Entity>> getScenarioEntities(int levelId);
```
**Returns**: Vector of Entity pointers (Ball, Block, Ramp, Ground) configured for the level
**Usage**: Add these entities to your physics engine

### Get Scenario Setup
```cpp
ScenarioSetup getCurrentScenarioSetup() const;
```
**Returns**: ScenarioSetup struct with all scenario data
**Usage**: Access instruction text, entity list, pause flag

### Set Current Level
```cpp
void setCurrentLevel(int id);
```
**Effect**: Changes current level, unlocks it if needed, saves progress
**Usage**: Call before loading scenario entities

---

## 🎯 Entity Types Supported

### Ball
- Circular physics entity
- Has position, velocity, mass, size, color
- Can have a label (e.g., "Ball A")

### Block
- Rectangular physics entity
- Static or dynamic
- Configurable size and mass

### Ramp
- Angled surface for rolling objects
- Provides slope for gravity demonstrations

### Ground
- Large static platform
- Provides base surface for scenarios

---

## 🔍 Example: Complete Integration

```cpp
// In main.cpp, add this function:
void applyPhysicsScenario(GameEngine& gameEngine, PhysicsEngine& physicsEngine, int levelId) {
    // Get scenario info
    ScenarioSetup scenario = gameEngine.getCurrentScenarioSetup();

    // Show instruction to user (optional)
    std::cout << "Scenario: " << scenario.instruction << std::endl;

    // Clear existing physics objects
    physicsEngine.clearAllEntities();

    // Load scenario entities
    auto entities = gameEngine.getScenarioEntities(levelId);

    // Add to physics engine
    for (auto& entity : entities) {
        physicsEngine.addEntity(entity);
    }

    // Pause simulation if requested
    if (scenario.pauseOnStart) {
        physicsEngine.pause();
    }
}

// Call when needed:
applyPhysicsScenario(gameEngine, physicsEngine, currentLevelId);
```

---

## ✅ Verification

### Check Scenario is Loaded
```cpp
// After calling getScenarioEntities:
auto entities = gameEngine.getScenarioEntities(levelId);
std::cout << "Loaded " << entities.size() << " entities" << std::endl;

for (size_t i = 0; i < entities.size(); ++i) {
    std::cout << "  Entity " << i << ": type=" << entities[i]->getType()
              << " pos=" << entities[i]->getPosition() << std::endl;
}
```

### Check Physics Display in UI
1. Press 'G' to enter Learning Mode
2. Look for purple "PHYSICS SCENARIO CONFIGURED" card
3. Verify entity icons and instruction text appear
4. Check entity count matches expected

---

## 🎨 Visual Physics Display

The **purple glowing card** in the UI shows:
- Scenario instruction (what to observe)
- Visual icons for each entity type
- Entity labels (if any)
- Count of total entities

**Location**: Between "REFLECT" section and "YOUR PROGRESS" section

---

## 🔧 Troubleshooting

### Issue: Entities not appearing in physics simulation
**Solution**: Make sure you're calling `getScenarioEntities()` and adding them to the physics engine

### Issue: UI shows scenario but physics is empty
**Solution**: The display and physics are separate. UI shows *configured* entities, physics shows *active* entities. You need to call `getScenarioEntities()` to make them active.

### Issue: Wrong entities for level
**Solution**: Check that you're passing the correct `levelId` to `getScenarioEntities()`

### Issue: Entities collide unexpectedly
**Solution**: Check entity positions in scenario configuration. They should be spaced appropriately.

---

## 📊 Scenario Configuration Summary

| Level | Entities | Main Concept |
|-------|----------|-------------|
| 1 | Ball, Ramp, Ground | Gravity & Acceleration |
| 2 | 2 Balls (moving), Ground | Collision & Momentum |
| 3 | Ball, Platform, Ramp, Ground | Work & Energy |
| 4 | Multiple Blocks | Forces & Motion |
| 5 | Complex Setup | Advanced Concepts |

---

## 🎉 Conclusion

The physics scenarios are **fully configured and ready to use**! The GameEngine now provides:

✅ **Complete entity configurations** for all 5 levels
✅ **Visual display** in the UI showing scenarios
✅ **Easy access methods** (`getScenarioEntities()`, `getCurrentScenarioSetup()`)
✅ **Professional documentation** of each scenario

To integrate into your physics simulation, simply call `getScenarioEntities(levelId)` and add the returned entities to your physics engine!

---

**Status**: ✅ **SCENARIOS FULLY CONFIGURED AND DOCUMENTED**
**Next Step**: Call `getScenarioEntities()` from main.cpp to activate scenarios in physics simulation
