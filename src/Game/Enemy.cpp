#include "Enemy.h"
#include "../Core/EventSystem.h"
#include <random>
#include <algorithm>

namespace Tempest {

EnemySystem::EnemySystem(EntityManager& entityManager) 
    : entityManager_(entityManager)
    , spawnTimer(0.0f)
    , spawnInterval(2.0f)
    , currentLevel(1)
    , curFlippers(0), curPulsars(0), curTankers(0), curSpikers(0), curFuzzballs(0)
    , maxFlippers(8), maxPulsars(6), maxTankers(4), maxSpikers(6), maxFuzzballs(4) {
}

EnemySystem::~EnemySystem() {
}

void EnemySystem::update(float deltaTime) {
    // Update spawn timer
    spawnTimer += deltaTime;
    
    // Spawn enemies based on timer and level
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0f;
        
        // Adjust spawn interval based on level
        spawnInterval = std::max(0.5f, 3.0f - (currentLevel * 0.2f));
        
        // Determine which enemy type to spawn based on level and current counts
        EnemyType spawnType = determineSpawnType();
        if (spawnType != EnemyType::Flipper) { // Flipper is default, check if we can spawn
            uint8_t segment = std::rand() % 16; // Random segment
            float along = 0.0f; // Start at top of tube
            spawnEnemy(spawnType, segment, along);
        }
    }
    
    // Update all active enemies
    for (auto& enemyId : activeEnemies) {
        auto* entity = entityManager_.getEntity(enemyId);
        if (entity && entity->hasComponent<EnemyComponent>()) {
            auto* enemy = entity->getComponent<EnemyComponent>();
            if (enemy && enemy->isActive) {
                updateEnemyMovement(*enemy, deltaTime);
                updateEnemyAI(*enemy, deltaTime);
            }
        }
    }
    
    // Clean up dead enemies
    activeEnemies.erase(
        std::remove_if(activeEnemies.begin(), activeEnemies.end(),
            [this](EntityID id) {
                auto* entity = entityManager_.getEntity(id);
                if (!entity || !entity->hasComponent<EnemyComponent>()) {
                    return true;
                }
                auto* enemy = entity->getComponent<EnemyComponent>();
                return enemy && enemy->state == EnemyState::Dead;
            }),
        activeEnemies.end()
    );
}

void EnemySystem::spawnEnemy(EnemyType type, uint8_t segment, float along) {
    // Check if we can spawn this enemy type
    bool canSpawn = false;
    switch (type) {
        case EnemyType::Flipper:
            canSpawn = (curFlippers < maxFlippers);
            break;
        case EnemyType::Pulsar:
            canSpawn = (curPulsars < maxPulsars);
            break;
        case EnemyType::Tanker:
            canSpawn = (curTankers < maxTankers);
            break;
        case EnemyType::Spiker:
            canSpawn = (curSpikers < maxSpikers);
            break;
        case EnemyType::Fuzzball:
            canSpawn = (curFuzzballs < maxFuzzballs);
            break;
    }
    
    if (!canSpawn) return;
    
    // Create entity and add enemy component
    EntityID enemyId = entityManager_.createEntity();
    auto enemyComponent = std::make_unique<EnemyComponent>();
    auto& enemy = *enemyComponent;
    
    // Initialize enemy
    enemy.initialize(type, segment, along);
    entityManager_.addComponent<EnemyComponent>(enemyId, std::move(enemyComponent));
    activeEnemies.push_back(enemyId);
    
    // Update count
    switch (type) {
        case EnemyType::Flipper: curFlippers++; break;
        case EnemyType::Pulsar: curPulsars++; break;
        case EnemyType::Tanker: curTankers++; break;
        case EnemyType::Spiker: curSpikers++; break;
        case EnemyType::Fuzzball: curFuzzballs++; break;
    }
}

EnemyType EnemySystem::determineSpawnType() {
    // Simple spawn logic based on level and current enemy counts
    // In the original game, this was more complex with enemy_movement arrays
    
    std::vector<EnemyType> availableTypes;
    
    if (curFlippers < maxFlippers) availableTypes.push_back(EnemyType::Flipper);
    if (curPulsars < maxPulsars && currentLevel >= 2) availableTypes.push_back(EnemyType::Pulsar);
    if (curTankers < maxTankers && currentLevel >= 3) availableTypes.push_back(EnemyType::Tanker);
    if (curSpikers < maxSpikers && currentLevel >= 4) availableTypes.push_back(EnemyType::Spiker);
    if (curFuzzballs < maxFuzzballs && currentLevel >= 5) availableTypes.push_back(EnemyType::Fuzzball);
    
    if (availableTypes.empty()) return EnemyType::Flipper;
    
    // Weighted random selection
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, availableTypes.size() - 1);
    
    return availableTypes[dis(gen)];
}

void EnemySystem::updateEnemyMovement(EnemyComponent& enemy, float deltaTime) {
    if (enemy.state == EnemyState::Dead) return;
    
    // Update position along tube depth
    enemy.along += enemy.moveSpeed * deltaTime;
    
    // Handle segment movement based on movement type
    switch (enemy.movementType) {
        case MovementType::NoAdjustment:
            // No segment change
            break;
        case MovementType::Clockwise:
            // Move clockwise around tube
            enemy.segment = (enemy.segment + 1) & 0x0F; // Wrap to 0-15
            break;
        case MovementType::CounterClockwise:
            // Move counter-clockwise around tube
            enemy.segment = (enemy.segment - 1) & 0x0F; // Wrap to 0-15
            break;
    }
    
    // Update 3D position based on tube geometry
    // This would integrate with TubeGeometry system
    // For now, use simple calculation
    float angle = (enemy.segment * 22.5f) * 3.14159f / 180.0f; // 360° / 16 segments
    float radius = 1.0f; // Constant radius as per original
    enemy.position.x = radius * cos(angle);
    enemy.position.y = enemy.along;
    enemy.position.z = radius * sin(angle);
}

void EnemySystem::updateEnemyAI(EnemyComponent& enemy, float deltaTime) {
    if (enemy.state == EnemyState::Dead) return;
    
    // Update cooldown
    if (enemy.currentCooldown > 0) {
        enemy.currentCooldown -= deltaTime;
    }
    
    // Type-specific AI behavior
    switch (enemy.type) {
        case EnemyType::Flipper:
            updateFlipperBehavior(enemy, deltaTime);
            break;
        case EnemyType::Pulsar:
            updatePulsarBehavior(enemy, deltaTime);
            break;
        case EnemyType::Tanker:
            updateTankerBehavior(enemy, deltaTime);
            break;
        case EnemyType::Spiker:
            updateSpikerBehavior(enemy, deltaTime);
            break;
        case EnemyType::Fuzzball:
            updateFuzzballBehavior(enemy, deltaTime);
            break;
    }
}

void EnemySystem::updateFlipperBehavior(EnemyComponent& enemy, float deltaTime) {
    // Flippers move quickly and attack frequently
    if (enemy.currentCooldown <= 0) {
        // Attack logic would go here
        enemy.currentCooldown = enemy.attackCooldown;
    }
    
    // Flippers occasionally change movement direction
    if (std::rand() % 100 < 5) { // 5% chance per frame
        enemy.movementType = (enemy.movementType == MovementType::Clockwise) 
            ? MovementType::CounterClockwise : MovementType::Clockwise;
    }
}

void EnemySystem::updatePulsarBehavior(EnemyComponent& enemy, float deltaTime) {
    // Pulsars pulse (expand/contract) and move erratically
    if (enemy.currentCooldown <= 0) {
        // Pulse attack logic
        enemy.currentCooldown = enemy.attackCooldown;
    }
    
    // Pulsars change direction more frequently
    if (std::rand() % 100 < 10) { // 10% chance per frame
        enemy.movementType = (enemy.movementType == MovementType::Clockwise) 
            ? MovementType::CounterClockwise : MovementType::Clockwise;
    }
}

void EnemySystem::updateTankerBehavior(EnemyComponent& enemy, float deltaTime) {
    // Tankers are slow but tough
    if (enemy.currentCooldown <= 0) {
        // Tanker attack logic
        enemy.currentCooldown = enemy.attackCooldown;
    }
    
    // Tankers move more predictably
    if (std::rand() % 100 < 2) { // 2% chance per frame
        enemy.movementType = (enemy.movementType == MovementType::Clockwise) 
            ? MovementType::CounterClockwise : MovementType::Clockwise;
    }
}

void EnemySystem::updateSpikerBehavior(EnemyComponent& enemy, float deltaTime) {
    // Spikers are fast and aggressive
    if (enemy.currentCooldown <= 0) {
        // Spiker attack logic
        enemy.currentCooldown = enemy.attackCooldown;
    }
    
    // Spikers change direction frequently
    if (std::rand() % 100 < 15) { // 15% chance per frame
        enemy.movementType = (enemy.movementType == MovementType::Clockwise) 
            ? MovementType::CounterClockwise : MovementType::Clockwise;
    }
}

void EnemySystem::updateFuzzballBehavior(EnemyComponent& enemy, float deltaTime) {
    // Fuzzballs move erratically and split when hit
    if (enemy.currentCooldown <= 0) {
        // Fuzzball attack logic
        enemy.currentCooldown = enemy.attackCooldown;
    }
    
    // Fuzzballs change direction very frequently
    if (std::rand() % 100 < 20) { // 20% chance per frame
        enemy.movementType = (enemy.movementType == MovementType::Clockwise) 
            ? MovementType::CounterClockwise : MovementType::Clockwise;
    }
}

void EnemySystem::handleEnemyDeath(EnemyComponent& enemy) {
    enemy.state = EnemyState::Dying;
    
    // Update count
    switch (enemy.type) {
        case EnemyType::Flipper: curFlippers--; break;
        case EnemyType::Pulsar: curPulsars--; break;
        case EnemyType::Tanker: curTankers--; break;
        case EnemyType::Spiker: curSpikers--; break;
        case EnemyType::Fuzzball: curFuzzballs--; break;
    }
    
    // Death animation would go here
    // For now, mark as dead immediately
    enemy.state = EnemyState::Dead;
    enemy.isActive = false;
}

} // namespace Tempest 