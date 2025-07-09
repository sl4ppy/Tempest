#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "../Core/Component.h"
#include "../Core/EntityManager.h"
#include "Projectile.h"
#include "Enemy.h"
#include "Player.h"

namespace Tempest {

struct CollisionComponent : public Component {
    glm::vec3 position;           // Current position
    float radius;                 // Collision radius
    bool isActive;                // Whether collision detection is active
    uint8_t collisionGroup;       // 0=player, 1=enemy, 2=projectile
    
    CollisionComponent() {
        position = glm::vec3(0.0f);
        radius = 0.1f;
        isActive = true;
        collisionGroup = 0;
    }
    
    ComponentTypeID getTypeId() const override { return 4; }
    const char* getTypeName() const override { return "CollisionComponent"; }
};

class CollisionSystem {
public:
    explicit CollisionSystem(EntityManager& entityManager);
    ~CollisionSystem();
    
    void update(float deltaTime);
    void checkCollisions();
    
    // Collision detection methods
    bool checkCollision(const glm::vec3& pos1, float radius1, const glm::vec3& pos2, float radius2);
    bool checkProjectileCollision(const ProjectileComponent& projectile, const CollisionComponent& target);
    bool checkPlayerEnemyCollision(const PlayerComponent& player, const EnemyComponent& enemy);
    
    // Collision response
    void handlePlayerEnemyCollision(EntityID playerId, EntityID enemyId);
    void handleProjectileEnemyCollision(EntityID projectileId, EntityID enemyId);
    void handleProjectilePlayerCollision(EntityID projectileId, EntityID playerId);
    
    // Utility methods
    float calculateDistance(const glm::vec3& pos1, const glm::vec3& pos2);
    bool isInSameSegment(uint8_t segment1, uint8_t segment2, float tolerance = 1.0f);
    
private:
    EntityManager& entityManager_;
    
    // Collision groups
    static constexpr uint8_t COLLISION_GROUP_PLAYER = 0;
    static constexpr uint8_t COLLISION_GROUP_ENEMY = 1;
    static constexpr uint8_t COLLISION_GROUP_PROJECTILE = 2;
    
    // Collision thresholds
    static constexpr float PLAYER_COLLISION_RADIUS = 0.15f;
    static constexpr float ENEMY_COLLISION_RADIUS = 0.12f;
    static constexpr float PROJECTILE_COLLISION_RADIUS = 0.05f;
    static constexpr float COLLISION_THRESHOLD = 0.2f;
};

} // namespace Tempest 