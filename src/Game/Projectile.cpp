#include "Projectile.h"
#include "../Core/EntityManager.h"
#include "../Core/EventSystem.h"
#include <algorithm>
#include <cmath>

namespace Tempest {

ProjectileSystem::ProjectileSystem(EntityManager& entityManager) 
    : entityManager_(entityManager)
    , currentPlayerShots(0)
    , currentEnemyShots(0) {
}

ProjectileSystem::~ProjectileSystem() {
}

void ProjectileSystem::update(float deltaTime) {
    // Update all active projectiles
    for (auto& projectileId : activeProjectiles) {
        auto* entity = entityManager_.getEntity(projectileId);
        if (entity && entity->hasComponent<ProjectileComponent>()) {
            auto* projectile = entity->getComponent<ProjectileComponent>();
            if (projectile && projectile->state == ProjectileState::Active) {
                updateProjectileMovement(*projectile, deltaTime);
                
                // Check lifetime
                projectile->currentLifetime += deltaTime;
                if (projectile->currentLifetime >= projectile->lifetime) {
                    handleProjectileDeath(*projectile);
                }
            }
        }
    }
    
    // Clean up dead projectiles
    activeProjectiles.erase(
        std::remove_if(activeProjectiles.begin(), activeProjectiles.end(),
            [this](EntityID id) {
                auto* entity = entityManager_.getEntity(id);
                if (!entity || !entity->hasComponent<ProjectileComponent>()) {
                    return true;
                }
                auto* projectile = entity->getComponent<ProjectileComponent>();
                return projectile && projectile->state == ProjectileState::Dead;
            }),
        activeProjectiles.end()
    );
}

void ProjectileSystem::spawnProjectile(ProjectileType type, uint8_t segment, float yPosition, bool isPlayer, uint8_t ownerId) {
    // Check projectile limits
    if (isPlayer && currentPlayerShots >= MAX_PLAYER_SHOTS) {
        return; // Can't fire more player shots
    }
    if (!isPlayer && currentEnemyShots >= MAX_ENEMY_SHOTS) {
        return; // Can't fire more enemy shots
    }
    
    // Create entity and add projectile component
    EntityID projectileId = entityManager_.createEntity();
    auto projectileComponent = std::make_unique<ProjectileComponent>();
    auto& projectile = *projectileComponent;
    
    // Initialize projectile
    projectile.initialize(type, segment, yPosition, isPlayer, ownerId);
    entityManager_.addComponent<ProjectileComponent>(projectileId, std::move(projectileComponent));
    activeProjectiles.push_back(projectileId);
    
    // Update shot count
    if (isPlayer) {
        currentPlayerShots++;
    } else {
        currentEnemyShots++;
    }
}

void ProjectileSystem::updateProjectileMovement(ProjectileComponent& projectile, float deltaTime) {
    if (projectile.state != ProjectileState::Active) return;
    
    // Update Y position (movement along tube depth)
    if (projectile.type == ProjectileType::SuperZapper) {
        // Super zapper is instant, no movement
        return;
    }
    
    // Move projectile along tube depth
    if (projectile.isPlayerProjectile) {
        // Player projectiles move down (increasing Y)
        projectile.yPosition += projectile.speed * deltaTime;
    } else {
        // Enemy projectiles move up (decreasing Y)
        projectile.yPosition -= projectile.speed * deltaTime;
    }
    
    // Update 3D position based on tube geometry
    // This would integrate with TubeGeometry system
    float angle = (projectile.segment * 22.5f) * 3.14159f / 180.0f; // 360° / 16 segments
    float radius = 1.0f; // Constant radius as per original
    projectile.position.x = radius * cos(angle);
    projectile.position.y = projectile.yPosition;
    projectile.position.z = radius * sin(angle);
    
    // Set velocity for collision detection
    if (projectile.isPlayerProjectile) {
        projectile.velocity = glm::vec3(0.0f, projectile.speed, 0.0f);
    } else {
        projectile.velocity = glm::vec3(0.0f, -projectile.speed, 0.0f);
    }
}

void ProjectileSystem::handleProjectileCollision(ProjectileComponent& projectile, EntityID targetEntity) {
    if (projectile.state != ProjectileState::Active) return;
    
    // Handle collision based on projectile type
    switch (projectile.type) {
        case ProjectileType::PlayerZap:
        case ProjectileType::PlayerFire:
            // Player projectile hit enemy
            // This would trigger enemy damage/death
            break;
        case ProjectileType::EnemyShot:
            // Enemy projectile hit player
            // This would trigger player damage/death
            break;
        case ProjectileType::SuperZapper:
            // Super zapper affects all enemies
            // This would clear all enemies on screen
            break;
    }
    
    // Mark projectile for destruction
    projectile.state = ProjectileState::Exploding;
}

void ProjectileSystem::handleProjectileDeath(ProjectileComponent& projectile) {
    projectile.state = ProjectileState::Dead;
    
    // Update shot count
    if (projectile.isPlayerProjectile) {
        currentPlayerShots--;
    } else {
        currentEnemyShots--;
    }
    
    // Explosion effects would go here
    // For now, just mark as dead
}

void ProjectileSystem::firePlayerZap(uint8_t segment, float yPosition) {
    spawnProjectile(ProjectileType::PlayerZap, segment, yPosition, true, 0);
}

void ProjectileSystem::firePlayerFire(uint8_t segment, float yPosition) {
    spawnProjectile(ProjectileType::PlayerFire, segment, yPosition, true, 0);
}

void ProjectileSystem::fireSuperZapper() {
    // Super zapper affects all segments - bypass normal shot limits
    for (uint8_t segment = 0; segment < 16; ++segment) {
        // Create entity and add projectile component directly
        EntityID projectileId = entityManager_.createEntity();
        auto projectileComponent = std::make_unique<ProjectileComponent>();
        auto& projectile = *projectileComponent;
        
        // Initialize projectile
        projectile.initialize(ProjectileType::SuperZapper, segment, 0.0f, true, 0);
        entityManager_.addComponent<ProjectileComponent>(projectileId, std::move(projectileComponent));
        activeProjectiles.push_back(projectileId);
        
        // Don't increment shot count for super zapper
    }
}

void ProjectileSystem::fireEnemyShot(uint8_t segment, float yPosition, uint8_t enemyId) {
    spawnProjectile(ProjectileType::EnemyShot, segment, yPosition, false, enemyId);
}

} // namespace Tempest 