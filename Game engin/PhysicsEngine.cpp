#include "PhysicsEngine.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

PhysicsEngine::PhysicsEngine() 
    : gravity(0, 400.0f), timeScale(1.0f), paused(false), collisionCooldown(0) {}

void PhysicsEngine::setTimeScale(float scale) {
    timeScale = std::max(0.1f, std::min(scale, 3.0f));
}

void PhysicsEngine::update(std::vector<std::shared_ptr<Entity>>& entities, float deltaTime) {
    if (paused) return;
    
    float scaledDelta = deltaTime * timeScale;
    collisionCooldown -= scaledDelta;
    
    if (collisionCooldown <= 0) {
        recentCollisions.clear();
        collisionCooldown = 0;
    }
    
    // Apply gravity
    for (auto& entity : entities) {
        if (entity->getPhysics().affectedByGravity && 
            entity->getPhysics().type == PhysicsType::DYNAMIC &&
            !entity->isDragging()) {
            applyGravity(entity.get());
        }
    }
    
    // Update all entities
    for (auto& entity : entities) {
        entity->update(scaledDelta);
    }
    
    // Collision detection and resolution
    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            Entity* a = entities[i].get();
            Entity* b = entities[j].get();
            
            if (a->getPhysics().type == PhysicsType::STATIC && 
                b->getPhysics().type == PhysicsType::STATIC) {
                continue;
            }
            
            if (a->isDragging() || b->isDragging()) continue;
            
            CollisionInfo info;
            bool collided = false;
            
            // Ball-Ball
            if (a->getType() == EntityType::BALL && b->getType() == EntityType::BALL) {
                collided = checkCircleCircleCollision(
                    static_cast<Ball*>(a), static_cast<Ball*>(b), info);
            }
            // Ball-Ramp
            else if (a->getType() == EntityType::BALL && b->getType() == EntityType::RAMP) {
                collided = checkCircleRampCollision(static_cast<Ball*>(a), static_cast<Ramp*>(b), info);
            }
            else if (b->getType() == EntityType::BALL && a->getType() == EntityType::RAMP) {
                collided = checkCircleRampCollision(static_cast<Ball*>(b), static_cast<Ramp*>(a), info);
            }
            // Ball-Box
            else if (a->getType() == EntityType::BALL) {
                collided = checkCircleAABBCollision(static_cast<Ball*>(a), b, info);
            }
            else if (b->getType() == EntityType::BALL) {
                collided = checkCircleAABBCollision(static_cast<Ball*>(b), a, info);
                if (collided) {
                    std::swap(info.entityA, info.entityB);
                    info.normal = info.normal * -1.0f;
                }
            }
            // Box-Box
            else {
                collided = checkAABBCollision(a, b, info);
            }
            
            if (collided) {
                info.occurred = true;
                separateEntities(info);
                resolveCollision(info);
                
                if (collisionCooldown <= 0) {
                    recentCollisions.push_back(info);
                    collisionCooldown = 0.5f;
                }
            }
        }
    }
}

void PhysicsEngine::applyGravity(Entity* entity) {
    Vector2 gravityForce = gravity * entity->getPhysics().mass;
    entity->applyForce(gravityForce);
}

bool PhysicsEngine::checkCircleCircleCollision(Ball* a, Ball* b, CollisionInfo& info) {
    Vector2 delta = b->getPosition() - a->getPosition();
    float distance = delta.length();
    float radiusSum = a->getRadius() + b->getRadius();
    
    if (distance < radiusSum && distance > 0) {
        info.entityA = a;
        info.entityB = b;
        info.normal = delta.normalized();
        info.penetration = radiusSum - distance;
        return true;
    }
    return false;
}

bool PhysicsEngine::checkCircleAABBCollision(Ball* ball, Entity* box, CollisionInfo& info) {
    AABB bbox = box->getBoundingBox();
    Vector2 ballPos = ball->getPosition();
    
    float closestX = std::max(bbox.min.x, std::min(ballPos.x, bbox.max.x));
    float closestY = std::max(bbox.min.y, std::min(ballPos.y, bbox.max.y));
    Vector2 closest(closestX, closestY);
    
    Vector2 delta = ballPos - closest;
    float distance = delta.length();
    
    if (distance < ball->getRadius()) {
        info.entityA = ball;
        info.entityB = box;
        
        if (distance > 0.001f) {
            info.normal = delta.normalized();
        } else {
            Vector2 center = bbox.center();
            Vector2 toCenter = ballPos - center;
            
            float dx = std::abs(toCenter.x);
            float dy = std::abs(toCenter.y);
            
            if (dx > dy) {
                info.normal = Vector2(toCenter.x > 0 ? 1.0f : -1.0f, 0.0f);
            } else {
                info.normal = Vector2(0.0f, toCenter.y > 0 ? 1.0f : -1.0f);
            }
        }
        
        info.penetration = ball->getRadius() - distance;
        return true;
    }
    return false;
}

bool PhysicsEngine::checkCircleRampCollision(Ball* ball, Ramp* ramp, CollisionInfo& info) {
    Vector2 ballPos = ball->getPosition();
    AABB rampBox = ramp->getBoundingBox();
    
    // Check if ball is near ramp
    if (!rampBox.contains(ballPos) && 
        ballPos.x < rampBox.min.x - ball->getRadius() &&
        ballPos.y < rampBox.min.y - ball->getRadius()) {
        return false;
    }
    
    // Ramp line from top-left to bottom-right
    Vector2 rampStart = rampBox.min;
    Vector2 rampEnd(rampBox.max.x, rampBox.max.y);
    Vector2 rampDir = (rampEnd - rampStart).normalized();
    
    // Project ball onto ramp line
    Vector2 toBall = ballPos - rampStart;
    float projection = toBall.dot(rampDir);
    projection = std::max(0.0f, std::min(projection, (rampEnd - rampStart).length()));
    
    Vector2 closestPoint = rampStart + (rampDir * projection);
    Vector2 delta = ballPos - closestPoint;
    float distance = delta.length();
    
    if (distance < ball->getRadius()) {
        info.entityA = ball;
        info.entityB = ramp;
        info.normal = delta.normalized();
        info.penetration = ball->getRadius() - distance;
        return true;
    }
    
    return false;
}

bool PhysicsEngine::checkAABBCollision(Entity* a, Entity* b, CollisionInfo& info) {
    AABB boxA = a->getBoundingBox();
    AABB boxB = b->getBoundingBox();
    
    if (!boxA.intersects(boxB)) return false;
    
    Vector2 centerA = boxA.center();
    Vector2 centerB = boxB.center();
    Vector2 delta = centerB - centerA;
    
    float overlapX = (boxA.max.x - boxA.min.x + boxB.max.x - boxB.min.x) / 2 - std::abs(delta.x);
    float overlapY = (boxA.max.y - boxA.min.y + boxB.max.y - boxB.min.y) / 2 - std::abs(delta.y);
    
    info.entityA = a;
    info.entityB = b;
    
    if (overlapX < overlapY) {
        info.normal = Vector2(delta.x > 0 ? 1.0f : -1.0f, 0.0f);
        info.penetration = overlapX;
    } else {
        info.normal = Vector2(0.0f, delta.y > 0 ? 1.0f : -1.0f);
        info.penetration = overlapY;
    }
    
    return true;
}

void PhysicsEngine::separateEntities(CollisionInfo& info) {
    if (!info.entityA || !info.entityB) return;
    
    PhysicsType typeA = info.entityA->getPhysics().type;
    PhysicsType typeB = info.entityB->getPhysics().type;
    
    Vector2 separation = info.normal * (info.penetration + 0.01f);
    
    if (typeA == PhysicsType::DYNAMIC && typeB == PhysicsType::DYNAMIC) {
        info.entityA->setPosition(info.entityA->getPosition() - (separation * 0.5f));
        info.entityB->setPosition(info.entityB->getPosition() + (separation * 0.5f));
    }
    else if (typeA == PhysicsType::DYNAMIC) {
        info.entityA->setPosition(info.entityA->getPosition() - separation);
    }
    else if (typeB == PhysicsType::DYNAMIC) {
        info.entityB->setPosition(info.entityB->getPosition() + separation);
    }
}

void PhysicsEngine::resolveCollision(CollisionInfo& info) {
    if (!info.entityA || !info.entityB) return;

    PhysicsType typeA = info.entityA->getPhysics().type;
    PhysicsType typeB = info.entityB->getPhysics().type;

    if (typeA != PhysicsType::DYNAMIC && typeB != PhysicsType::DYNAMIC) return;

    Vector2 velA = info.entityA->getVelocity();
    Vector2 velB = info.entityB->getVelocity();
    Vector2 relativeVel = velB - velA;

    float velAlongNormal = relativeVel.dot(info.normal);
    if (velAlongNormal > 0) return;

    float restitution = std::min(info.entityA->getPhysics().restitution,
                                  info.entityB->getPhysics().restitution);

    float massA = info.entityA->getPhysics().mass;
    float massB = info.entityB->getPhysics().mass;

    float impulseScalar = 0.0f;
    Vector2 impulse(0, 0);

    // Normal impulse (inelastic/elastic response)
    if (typeA == PhysicsType::DYNAMIC && typeB == PhysicsType::DYNAMIC) {
        impulseScalar = -(1 + restitution) * velAlongNormal;
        impulseScalar /= (1.0f / massA + 1.0f / massB);

        impulse = info.normal * impulseScalar;
        velA = velA - (impulse * (1.0f / massA));
        velB = velB + (impulse * (1.0f / massB));
    }
    else if (typeA == PhysicsType::DYNAMIC) {
        impulseScalar = -(1 + restitution) * velAlongNormal;
        impulse = info.normal * impulseScalar;
        velA = velA - impulse;
    }
    else if (typeB == PhysicsType::DYNAMIC) {
        impulseScalar = -(1 + restitution) * velAlongNormal;
        impulse = info.normal * impulseScalar;
        velB = velB + impulse;
    }

    // Tangential (friction) impulse — Coulomb friction model (approximate)
    Vector2 tangent = relativeVel - (info.normal * velAlongNormal);
    float tangLen = tangent.length();
    if (tangLen > 0.0001f) {
        tangent = tangent * (1.0f / tangLen); // normalize

        float jt = -relativeVel.dot(tangent);
        float invMassSum = 0.0f;
        if (typeA == PhysicsType::DYNAMIC) invMassSum += 1.0f / massA;
        if (typeB == PhysicsType::DYNAMIC) invMassSum += 1.0f / massB;
        if (invMassSum > 0.0f) jt /= invMassSum; else jt = 0.0f;

        float mu = std::sqrt(info.entityA->getPhysics().friction * info.entityB->getPhysics().friction);

        Vector2 frictionImpulse;
        if (std::abs(jt) < std::abs(impulseScalar) * mu) {
            frictionImpulse = tangent * jt;
        } else {
            frictionImpulse = tangent * (-std::abs(impulseScalar) * mu);
        }

        if (typeA == PhysicsType::DYNAMIC) velA = velA - (frictionImpulse * (1.0f / massA));
        if (typeB == PhysicsType::DYNAMIC) velB = velB + (frictionImpulse * (1.0f / massB));
    }

    // Apply final velocities
    if (typeA == PhysicsType::DYNAMIC) info.entityA->setVelocity(velA);
    if (typeB == PhysicsType::DYNAMIC) info.entityB->setVelocity(velB);

    // Small resting threshold to avoid jitter
    const float restingEpsilon = 0.5f;
    if (std::abs(info.entityA->getVelocity().dot(info.normal)) < restingEpsilon) {
        Vector2 v = info.entityA->getVelocity();
        Vector2 nComp = info.normal * v.dot(info.normal);
        v = v - nComp;
        info.entityA->setVelocity(v);
    }
    if (std::abs(info.entityB->getVelocity().dot(info.normal)) < restingEpsilon) {
        Vector2 v = info.entityB->getVelocity();
        Vector2 nComp = info.normal * v.dot(info.normal);
        v = v - nComp;
        info.entityB->setVelocity(v);
    }
}

std::string PhysicsEngine::explainGravity() const {
    std::stringstream ss;
    ss << "GRAVITY\n\n";
    ss << "Acceleration: " << gravity.y << " pixels/sec^2\n";
    ss << "Direction: Downward (like Earth)\n\n";
    ss << "All objects fall at the same rate,\n";
    ss << "regardless of their mass!";
    return ss.str();
}

std::string PhysicsEngine::explainCollision(const CollisionInfo& info) const {
    if (!info.occurred) return "No collision";
    
    std::stringstream ss;
    ss << "COLLISION DETECTED\n\n";
    ss << info.entityA->getName() << " <-> " << info.entityB->getName() << "\n\n";
    ss << "Impact normal: (" << std::fixed << std::setprecision(2) 
       << info.normal.x << ", " << info.normal.y << ")\n";
    ss << "Penetration: " << info.penetration << " pixels\n\n";
    ss << "Energy is transferred between objects!";
    return ss.str();
}

std::string PhysicsEngine::explainVelocity(const Entity* entity) const {
    if (!entity) return "No entity";
    
    Vector2 vel = entity->getVelocity();
    float speed = vel.length();
    
    std::stringstream ss;
    ss << "VELOCITY - " << entity->getName() << "\n\n";
    ss << std::fixed << std::setprecision(1);
    ss << "Speed: " << speed << " pixels/sec\n";
    ss << "Direction: (" << vel.x << ", " << vel.y << ")\n\n";
    
    if (speed < 10) ss << "Moving slowly";
    else if (speed < 100) ss << "Moving at medium speed";
    else ss << "Moving fast!";
    
    return ss.str();
}

std::string PhysicsEngine::explainEnergy(const Entity* entity) const {
    if (!entity) return "No entity";
    
    Vector2 vel = entity->getVelocity();
    float mass = entity->getPhysics().mass;
    float kineticEnergy = 0.5f * mass * vel.length() * vel.length();
    
    std::stringstream ss;
    ss << "ENERGY - " << entity->getName() << "\n\n";
    ss << "Mass: " << std::fixed << std::setprecision(2) << mass << " kg\n";
    ss << "Kinetic Energy: " << (int)kineticEnergy << " J\n\n";
    ss << "Energy = ½ × mass × velocity²";
    
    return ss.str();
}
