#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include "../Core/Component.h"
#include "../Core/EntityManager.h"

namespace Tempest {

using EntityID = uint32_t;

enum class EnemyType : uint8_t {
    Flipper = 1,
    Pulsar = 2,
    Tanker = 3,
    Spiker = 4,
    Fuzzball = 5
};

enum class EnemyState : uint8_t {
    Spawning = 0,
    Moving = 1,
    Attacking = 2,
    Dying = 3,
    Dead = 4
};

enum class MovementType : uint8_t {
    NoAdjustment = 0,
    Clockwise = 8,
    CounterClockwise = 0xC
};

struct EnemyComponent : public Component {
    EnemyType type;
    uint8_t segment;              // 0-15 segment position
    float along;                  // Position along tube depth (0.0 = top, increasing = deeper)
    uint8_t health;
    uint8_t maxHealth;
    EnemyState state;
    MovementType movementType;
    float moveSpeed;
    float attackCooldown;
    float currentCooldown;
    glm::vec3 position;           // Calculated 3D position
    glm::vec3 velocity;           // Movement velocity
    bool isActive;                // Whether enemy is active in game
    
    EnemyComponent() {
        type = EnemyType::Flipper;
        segment = 0;
        along = 0.0f;
        health = 1;
        maxHealth = 1;
        state = EnemyState::Spawning;
        movementType = MovementType::NoAdjustment;
        moveSpeed = 1.0f;
        attackCooldown = 2.0f;
        currentCooldown = 0.0f;
        position = glm::vec3(0.0f);
        velocity = glm::vec3(0.0f);
        isActive = false;
    }
    
    ComponentTypeID getTypeId() const override { return 2; }
    const char* getTypeName() const override { return "EnemyComponent"; }
    
    // Initialize enemy based on type
    void initialize(EnemyType enemyType, uint8_t startSegment, float startAlong) {
        type = enemyType;
        segment = startSegment;
        along = startAlong;
        isActive = true;
        state = EnemyState::Spawning;
        
        // Set properties based on enemy type
        switch (type) {
            case EnemyType::Flipper:
                health = maxHealth = 1;
                moveSpeed = 2.0f;
                attackCooldown = 1.5f;
                break;
            case EnemyType::Pulsar:
                health = maxHealth = 2;
                moveSpeed = 1.5f;
                attackCooldown = 2.0f;
                break;
            case EnemyType::Tanker:
                health = maxHealth = 3;
                moveSpeed = 0.8f;
                attackCooldown = 3.0f;
                break;
            case EnemyType::Spiker:
                health = maxHealth = 1;
                moveSpeed = 3.0f;
                attackCooldown = 1.0f;
                break;
            case EnemyType::Fuzzball:
                health = maxHealth = 2;
                moveSpeed = 1.0f;
                attackCooldown = 2.5f;
                break;
        }
    }

    // Add setter for state to handle isActive
    void setState(EnemyState newState) {
        state = newState;
        if (state == EnemyState::Dead) {
            isActive = false;
        }
    }
};

class EnemySystem {
public:
    explicit EnemySystem(EntityManager& entityManager);
    ~EnemySystem();
    
    void update(float deltaTime);
    void spawnEnemy(EnemyType type, uint8_t segment, float along);
    void updateEnemyMovement(EnemyComponent& enemy, float deltaTime);
    void updateEnemyAI(EnemyComponent& enemy, float deltaTime);
    void handleEnemyDeath(EnemyComponent& enemy);
    
    // Enemy type-specific behaviors
    void updateFlipperBehavior(EnemyComponent& enemy, float deltaTime);
    void updatePulsarBehavior(EnemyComponent& enemy, float deltaTime);
    void updateTankerBehavior(EnemyComponent& enemy, float deltaTime);
    void updateSpikerBehavior(EnemyComponent& enemy, float deltaTime);
    void updateFuzzballBehavior(EnemyComponent& enemy, float deltaTime);
    
private:
    EnemyType determineSpawnType();
    std::vector<EntityID> activeEnemies;
    EntityManager& entityManager_;
    float spawnTimer;
    float spawnInterval;
    uint8_t currentLevel;
    
    // Enemy counts per type (matching original RAM structure)
    uint8_t curFlippers;
    uint8_t curPulsars;
    uint8_t curTankers;
    uint8_t curSpikers;
    uint8_t curFuzzballs;
    
    // Max enemies per type
    uint8_t maxFlippers;
    uint8_t maxPulsars;
    uint8_t maxTankers;
    uint8_t maxSpikers;
    uint8_t maxFuzzballs;
};

} // namespace Tempest 