#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include "EduSandbox.h"
#include <vector>
#include <memory>
#include <string>

struct CollisionInfo {
    Entity* entityA;
    Entity* entityB;
    Vector2 normal;
    float penetration;
    bool occurred;
    float timestamp;
    
    CollisionInfo() : entityA(nullptr), entityB(nullptr), 
                      normal(0, 0), penetration(0), occurred(false), timestamp(0) {}
};

class PhysicsEngine {
private:
    Vector2 gravity;
    float timeScale;
    bool paused;
    std::vector<CollisionInfo> recentCollisions;
    float collisionCooldown;
    
    bool checkCircleCircleCollision(Ball* a, Ball* b, CollisionInfo& info);
    bool checkCircleAABBCollision(Ball* ball, Entity* box, CollisionInfo& info);
    bool checkCircleRampCollision(Ball* ball, Ramp* ramp, CollisionInfo& info);
    bool checkAABBCollision(Entity* a, Entity* b, CollisionInfo& info);
    
    void resolveCollision(CollisionInfo& info);
    void separateEntities(CollisionInfo& info);
    
public:
    PhysicsEngine();
    
    void setGravity(const Vector2& g) { gravity = g; }
    Vector2 getGravity() const { return gravity; }
    void setTimeScale(float scale);
    float getTimeScale() const { return timeScale; }
    void pause() { paused = true; }
    void resume() { paused = false; }
    void togglePause() { paused = !paused; }
    bool isPaused() const { return paused; }
    
    void update(std::vector<std::shared_ptr<Entity>>& entities, float deltaTime);
    void applyGravity(Entity* entity);
    
    const std::vector<CollisionInfo>& getRecentCollisions() const { 
        return recentCollisions; 
    }
    
    std::string explainGravity() const;
    std::string explainCollision(const CollisionInfo& info) const;
    std::string explainVelocity(const Entity* entity) const;
    std::string explainEnergy(const Entity* entity) const;
};

#endif // PHYSICS_ENGINE_H
