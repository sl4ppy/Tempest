#pragma once

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include "../Core/Component.h"
#include "../Core/EntityManager.h"

namespace Tempest {

using EntityID = uint32_t;

enum class ProjectileType : uint8_t {
    PlayerZap = 1,      // Player's close-range zap weapon
    PlayerFire = 2,      // Player's long-range fire weapon
    EnemyShot = 3,       // Enemy projectile
    SuperZapper = 4      // Player's super zapper (clears screen)
};

enum class ProjectileState : uint8_t {
    Active = 0,
    Exploding = 1,
    Dead = 2
};

struct ProjectileComponent : public Component {
    ProjectileType type;
    uint8_t segment;              // Target segment (0-15)
    float yPosition;              // Y position along tube depth
    float speed;                  // Movement speed
    uint8_t damage;               // Damage amount
    ProjectileState state;
    float lifetime;               // Time to live
    float currentLifetime;        // Current time alive
    glm::vec3 position;           // 3D position
    glm::vec3 velocity;           // Movement velocity
    bool isPlayerProjectile;      // Whether fired by player
    uint8_t ownerId;              // ID of entity that fired this projectile
    
    ProjectileComponent() {
        type = ProjectileType::PlayerFire;
        segment = 0;
        yPosition = 0.0f;
        speed = 5.0f;
        damage = 1;
        state = ProjectileState::Active;
        lifetime = 10.0f;
        currentLifetime = 0.0f;
        position = glm::vec3(0.0f);
        velocity = glm::vec3(0.0f);
        isPlayerProjectile = true;
        ownerId = 0;
    }
    
    ComponentTypeID getTypeId() const override { return 3; }
    const char* getTypeName() const override { return "ProjectileComponent"; }
    
    // Initialize projectile based on type
    void initialize(ProjectileType projectileType, uint8_t targetSegment, float startY, bool isPlayer, uint8_t owner) {
        type = projectileType;
        segment = targetSegment;
        yPosition = startY;
        isPlayerProjectile = isPlayer;
        ownerId = owner;
        state = ProjectileState::Active;
        currentLifetime = 0.0f;
        
        // Set properties based on projectile type
        switch (type) {
            case ProjectileType::PlayerZap:
                speed = 8.0f;
                damage = 1;
                lifetime = 2.0f;
                break;
            case ProjectileType::PlayerFire:
                speed = 6.0f;
                damage = 1;
                lifetime = 8.0f;
                break;
            case ProjectileType::EnemyShot:
                speed = 4.0f;
                damage = 1;
                lifetime = 5.0f;
                break;
            case ProjectileType::SuperZapper:
                speed = 0.0f; // Instant effect
                damage = 255; // Maximum damage for uint8_t
                lifetime = 0.1f;
                break;
        }
    }
};

class ProjectileSystem {
public:
    explicit ProjectileSystem(EntityManager& entityManager);
    ~ProjectileSystem();
    
    void update(float deltaTime);
    void spawnProjectile(ProjectileType type, uint8_t segment, float yPosition, bool isPlayer, uint8_t ownerId);
    void updateProjectileMovement(ProjectileComponent& projectile, float deltaTime);
    void handleProjectileCollision(ProjectileComponent& projectile, EntityID targetEntity);
    void handleProjectileDeath(ProjectileComponent& projectile);
    
    // Weapon system integration
    void firePlayerZap(uint8_t segment, float yPosition);
    void firePlayerFire(uint8_t segment, float yPosition);
    void fireSuperZapper();
    void fireEnemyShot(uint8_t segment, float yPosition, uint8_t enemyId);
    
private:
    std::vector<EntityID> activeProjectiles;
    EntityManager& entityManager_;
    
    // Projectile limits (matching original game)
    static constexpr uint8_t MAX_PLAYER_SHOTS = 8;
    static constexpr uint8_t MAX_ENEMY_SHOTS = 16;
    
    uint8_t currentPlayerShots;
    uint8_t currentEnemyShots;
};

} // namespace Tempest 