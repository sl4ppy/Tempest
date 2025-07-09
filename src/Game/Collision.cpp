#include "Collision.h"
#include "../Core/EventSystem.h"
#include <algorithm>
#include <cmath>

namespace Tempest {

CollisionSystem::CollisionSystem(EntityManager& entityManager) 
    : entityManager_(entityManager) {
}

CollisionSystem::~CollisionSystem() {
}

void CollisionSystem::update(float deltaTime) {
    checkCollisions();
}

void CollisionSystem::checkCollisions() {
    // Get all entities with collision components
    std::vector<Entity*> collisionEntities;
    
    // This is a simplified collision check - in a real implementation,
    // you'd use spatial partitioning for efficiency
    for (EntityID id = 0; id < entityManager_.getEntityCount(); ++id) {
        auto* entity = entityManager_.getEntity(id);
        if (entity && entity->hasComponent<CollisionComponent>()) {
            collisionEntities.push_back(entity);
        }
    }
    
    // Check collisions between all entities
    for (size_t i = 0; i < collisionEntities.size(); ++i) {
        for (size_t j = i + 1; j < collisionEntities.size(); ++j) {
            auto* entity1 = collisionEntities[i];
            auto* entity2 = collisionEntities[j];
            
            auto* collision1 = entity1->getComponent<CollisionComponent>();
            auto* collision2 = entity2->getComponent<CollisionComponent>();
            
            if (!collision1 || !collision2 || !collision1->isActive || !collision2->isActive) {
                continue;
            }
            
            // Check if collision groups can interact
            if (collision1->collisionGroup == collision2->collisionGroup) {
                continue; // Same group doesn't collide
            }
            
            // Check collision
            if (checkCollision(collision1->position, collision1->radius, 
                              collision2->position, collision2->radius)) {
                
                // Handle specific collision types
                if (collision1->collisionGroup == COLLISION_GROUP_PLAYER && 
                    collision2->collisionGroup == COLLISION_GROUP_ENEMY) {
                    handlePlayerEnemyCollision(entity1->getId(), entity2->getId());
                } else if (collision1->collisionGroup == COLLISION_GROUP_ENEMY && 
                           collision2->collisionGroup == COLLISION_GROUP_PLAYER) {
                    handlePlayerEnemyCollision(entity2->getId(), entity1->getId());
                } else if (collision1->collisionGroup == COLLISION_GROUP_PROJECTILE && 
                           collision2->collisionGroup == COLLISION_GROUP_ENEMY) {
                    handleProjectileEnemyCollision(entity1->getId(), entity2->getId());
                } else if (collision1->collisionGroup == COLLISION_GROUP_ENEMY && 
                           collision2->collisionGroup == COLLISION_GROUP_PROJECTILE) {
                    handleProjectileEnemyCollision(entity2->getId(), entity1->getId());
                } else if (collision1->collisionGroup == COLLISION_GROUP_PROJECTILE && 
                           collision2->collisionGroup == COLLISION_GROUP_PLAYER) {
                    handleProjectilePlayerCollision(entity1->getId(), entity2->getId());
                } else if (collision1->collisionGroup == COLLISION_GROUP_PLAYER && 
                           collision2->collisionGroup == COLLISION_GROUP_PROJECTILE) {
                    handleProjectilePlayerCollision(entity2->getId(), entity1->getId());
                }
            }
        }
    }
}

bool CollisionSystem::checkCollision(const glm::vec3& pos1, float radius1, 
                                   const glm::vec3& pos2, float radius2) {
    float distance = calculateDistance(pos1, pos2);
    float combinedRadius = radius1 + radius2;
    return distance <= combinedRadius;
}

bool CollisionSystem::checkProjectileCollision(const ProjectileComponent& projectile, 
                                             const CollisionComponent& target) {
    // Check if projectile and target are in the same segment
    // This is a simplified check - in reality, you'd check 3D distance
    return checkCollision(projectile.position, PROJECTILE_COLLISION_RADIUS,
                         target.position, target.radius);
}

bool CollisionSystem::checkPlayerEnemyCollision(const PlayerComponent& player, 
                                              const EnemyComponent& enemy) {
    // Check if player and enemy are in the same segment and close in Y position
    if (player.segment != enemy.segment) {
        return false;
    }
    
    // Check Y position proximity (simplified)
    float yDistance = std::abs(player.segmentLerp - enemy.along);
    return yDistance <= COLLISION_THRESHOLD;
}

void CollisionSystem::handlePlayerEnemyCollision(EntityID playerId, EntityID enemyId) {
    auto* playerEntity = entityManager_.getEntity(playerId);
    auto* enemyEntity = entityManager_.getEntity(enemyId);
    
    if (!playerEntity || !enemyEntity) return;
    
    auto* player = playerEntity->getComponent<PlayerComponent>();
    auto* enemy = enemyEntity->getComponent<EnemyComponent>();
    
    if (!player || !enemy) return;
    
    // Player takes damage
    if (player->isAlive && !player->isInvulnerable) {
        player->lives--;
        player->isInvulnerable = true;
        player->invulnerabilityTimer = 3.0f; // 3 seconds of invulnerability
        
        if (player->lives <= 0) {
            player->isAlive = false;
        }
    }
    
    // Enemy takes damage (if it's a weak enemy)
    if (enemy->isActive && enemy->state == EnemyState::Moving) {
        enemy->health--;
        if (enemy->health <= 0) {
            enemy->state = EnemyState::Dying;
        }
    }
}

void CollisionSystem::handleProjectileEnemyCollision(EntityID projectileId, EntityID enemyId) {
    auto* projectileEntity = entityManager_.getEntity(projectileId);
    auto* enemyEntity = entityManager_.getEntity(enemyId);
    
    if (!projectileEntity || !enemyEntity) return;
    
    auto* projectile = projectileEntity->getComponent<ProjectileComponent>();
    auto* enemy = enemyEntity->getComponent<EnemyComponent>();
    
    if (!projectile || !enemy) return;
    
    // Only player projectiles can damage enemies
    if (!projectile->isPlayerProjectile) return;
    
    // Enemy takes damage
    if (enemy->isActive && enemy->state == EnemyState::Moving) {
        enemy->health -= projectile->damage;
        if (enemy->health <= 0) {
            enemy->state = EnemyState::Dying;
        }
    }
    
    // Projectile is destroyed
    projectile->state = ProjectileState::Exploding;
}

void CollisionSystem::handleProjectilePlayerCollision(EntityID projectileId, EntityID playerId) {
    auto* projectileEntity = entityManager_.getEntity(projectileId);
    auto* playerEntity = entityManager_.getEntity(playerId);
    
    if (!projectileEntity || !playerEntity) return;
    
    auto* projectile = projectileEntity->getComponent<ProjectileComponent>();
    auto* player = playerEntity->getComponent<PlayerComponent>();
    
    if (!projectile || !player) return;
    
    // Only enemy projectiles can damage player
    if (projectile->isPlayerProjectile) return;
    
    // Player takes damage
    if (player->isAlive && !player->isInvulnerable) {
        player->lives--;
        player->isInvulnerable = true;
        player->invulnerabilityTimer = 3.0f; // 3 seconds of invulnerability
        
        if (player->lives <= 0) {
            player->isAlive = false;
        }
    }
    
    // Projectile is destroyed
    projectile->state = ProjectileState::Exploding;
}

float CollisionSystem::calculateDistance(const glm::vec3& pos1, const glm::vec3& pos2) {
    glm::vec3 diff = pos1 - pos2;
    return glm::length(diff);
}

bool CollisionSystem::isInSameSegment(uint8_t segment1, uint8_t segment2, float tolerance) {
    // Check if segments are the same or adjacent (within tolerance)
    int diff = std::abs(static_cast<int>(segment1) - static_cast<int>(segment2));
    return diff <= tolerance || diff >= (16 - tolerance);
}

} // namespace Tempest 