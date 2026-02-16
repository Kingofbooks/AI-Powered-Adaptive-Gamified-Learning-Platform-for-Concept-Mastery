#ifndef EDU_SANDBOX_H
#define EDU_SANDBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <algorithm>

// ===== Math Structures =====

struct Vector2 {
    float x, y;
    
    Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
    
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    float length() const {
        return std::sqrt(x * x + y * y);
    }
    
    Vector2 normalized() const {
        float len = length();
        return len > 0 ? Vector2(x / len, y / len) : Vector2(0, 0);
    }
    
    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }
};

struct AABB {
    Vector2 min, max;
    
    AABB() : min(0, 0), max(0, 0) {}
    AABB(const Vector2& min, const Vector2& max) : min(min), max(max) {}
    
    bool contains(const Vector2& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y;
    }
    
    bool intersects(const AABB& other) const {
        return !(max.x < other.min.x || min.x > other.max.x ||
                 max.y < other.min.y || min.y > other.max.y);
    }
    
    Vector2 center() const {
        return Vector2((min.x + max.x) / 2, (min.y + max.y) / 2);
    }
};

struct Color {
    Uint8 r, g, b, a;
    
    Color(Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255)
        : r(r), g(g), b(b), a(a) {}
    
    SDL_Color toSDL() const { return {r, g, b, a}; }
    
    static Color Red()      { return Color(220, 50, 50); }
    static Color Green()    { return Color(50, 180, 50); }
    static Color Blue()     { return Color(50, 120, 220); }
    static Color Yellow()   { return Color(240, 200, 50); }
    static Color Orange()   { return Color(255, 140, 50); }
    static Color Purple()   { return Color(160, 80, 200); }
    static Color White()    { return Color(255, 255, 255); }
    static Color Black()    { return Color(20, 20, 20); }
    static Color Gray()     { return Color(150, 150, 150); }
    static Color LightGray(){ return Color(200, 200, 200); }
    static Color Brown()    { return Color(139, 90, 60); }
    static Color Background(){ return Color(245, 245, 250); }
};

// ===== Entity Types =====

enum class EntityType {
    BALL,
    BLOCK,
    GROUND,
    ARROW,
    TEXT_LABEL,
    RAMP
};

enum class PhysicsType {
    DYNAMIC,
    STATIC,
    KINEMATIC
};

struct PhysicsProperties {
    PhysicsType type;
    float mass;
    float restitution;
    float friction;
    bool affectedByGravity;
    
    PhysicsProperties() 
        : type(PhysicsType::DYNAMIC)
        , mass(1.0f)
        , restitution(0.6f)
        , friction(0.3f)
        , affectedByGravity(true) {}
};

// ===== Entity Base Class =====

class Entity {
protected:
    EntityType type;
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    PhysicsProperties physics;
    Color color;
    AABB boundingBox;
    std::string name;
    bool selected;
    bool dragging;
    
public:
    Entity(EntityType type, const Vector2& pos)
        : type(type), position(pos), velocity(0, 0), acceleration(0, 0),
          color(Color::White()), selected(false), dragging(false) {}
    
    virtual ~Entity() = default;
    
    EntityType getType() const { return type; }
    Vector2 getPosition() const { return position; }
    Vector2 getVelocity() const { return velocity; }
    Vector2 getAcceleration() const { return acceleration; }
    const PhysicsProperties& getPhysics() const { return physics; }
    Color getColor() const { return color; }
    AABB getBoundingBox() const { return boundingBox; }
    std::string getName() const { return name; }
    bool isSelected() const { return selected; }
    bool isDragging() const { return dragging; }
    
    void setPosition(const Vector2& pos) { 
        position = pos; 
        updateBoundingBox();
    }
    
    void setVelocity(const Vector2& vel) { velocity = vel; }
    void setAcceleration(const Vector2& acc) { acceleration = acc; }
    void setColor(const Color& c) { color = c; }
    void setName(const std::string& n) { name = n; }
    void setSelected(bool s) { selected = s; }
    void setDragging(bool d) { 
        dragging = d; 
        if (d) velocity = Vector2(0, 0);
    }
    void setPhysicsType(PhysicsType t) { physics.type = t; }
    void setMass(float m) { physics.mass = m; }
    void setRestitution(float r) { physics.restitution = r; }
    void setAffectedByGravity(bool a) { physics.affectedByGravity = a; }
    
    void applyForce(const Vector2& force) {
        if (physics.type == PhysicsType::DYNAMIC && physics.mass > 0) {
            acceleration = acceleration + (force * (1.0f / physics.mass));
        }
    }
    
    void update(float deltaTime) {
        if (physics.type == PhysicsType::DYNAMIC && !dragging) {
            // Integrate acceleration
            velocity = velocity + (acceleration * deltaTime);

            // Simple linear damping to simulate air resistance / rolling friction
            float dampingFactor = std::max(0.0f, 1.0f - physics.friction * 0.5f * deltaTime);
            velocity = velocity * dampingFactor;

            position = position + (velocity * deltaTime);
            updateBoundingBox();
            acceleration = Vector2(0, 0);
        }
    }
    
    virtual void updateBoundingBox() = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    
    virtual std::string getDescription() const {
        return "Entity at (" + std::to_string((int)position.x) + ", " + 
               std::to_string((int)position.y) + ")";
    }
};

// ===== Specific Entities =====

class Ball : public Entity {
private:
    float radius;
    
public:
    Ball(const Vector2& pos, float radius = 25.0f);
    float getRadius() const { return radius; }
    void setRadius(float r);
    void updateBoundingBox() override;
    void render(SDL_Renderer* renderer) override;
    std::string getDescription() const override;
};

class Block : public Entity {
private:
    float width, height;
    
public:
    Block(const Vector2& pos, float width = 60.0f, float height = 60.0f);
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    void setSize(float w, float h);
    void updateBoundingBox() override;
    void render(SDL_Renderer* renderer) override;
    std::string getDescription() const override;
};

class Ground : public Entity {
private:
    float width, thickness;
    
public:
    Ground(const Vector2& pos, float width = 800.0f, float thickness = 30.0f);
    float getWidth() const { return width; }
    float getThickness() const { return thickness; }
    void updateBoundingBox() override;
    void render(SDL_Renderer* renderer) override;
    std::string getDescription() const override;
};

class Ramp : public Entity {
private:
    float width, height;
    float angle;
    
public:
    Ramp(const Vector2& pos, float width = 150.0f, float height = 80.0f);
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getAngle() const { return angle; }
    void updateBoundingBox() override;
    void render(SDL_Renderer* renderer) override;
    std::string getDescription() const override;
};

class Arrow : public Entity {
private:
    Vector2 direction;
    float length;
    std::string label;
    
public:
    Arrow(const Vector2& pos, const Vector2& dir, const std::string& label = "Force");
    Vector2 getDirection() const { return direction; }
    float getLength() const { return length; }
    std::string getLabel() const { return label; }
    void setDirection(const Vector2& dir);
    void setLabel(const std::string& l) { label = l; }
    void updateBoundingBox() override;
    void render(SDL_Renderer* renderer) override;
    std::string getDescription() const override;
};

class TextLabel : public Entity {
private:
    std::string text;
    int fontSize;
    
public:
    TextLabel(const Vector2& pos, const std::string& text);
    std::string getText() const { return text; }
    int getFontSize() const { return fontSize; }
    void setText(const std::string& t) { text = t; }
    void setFontSize(int size) { fontSize = size; }
    void updateBoundingBox() override;
    void render(SDL_Renderer* renderer) override;
    std::string getDescription() const override;
};

#endif // EDU_SANDBOX_H
