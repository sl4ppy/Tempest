#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/Game/Projectile.h"
#include "../../src/Game/Collision.h"
#include "../../src/Game/Player.h"
#include "../../src/Game/Enemy.h"
#include "../../src/Core/EntityManager.h"
#include <memory>

using namespace Tempest;

class ProjectileTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create entity manager and systems
        entityManager_ = std::make_unique<EntityManager>();
        projectileSystem_ = std::make_unique<ProjectileSystem>(*entityManager_);
        collisionSystem_ = std::make_unique<CollisionSystem>(*entityManager_);
    }
    
    void TearDown() override {
        // Clean up
    }
    
    std::unique_ptr<EntityManager> entityManager_;
    std::unique_ptr<ProjectileSystem> projectileSystem_;
    std::unique_ptr<CollisionSystem> collisionSystem_;
};

TEST_F(ProjectileTest, ProjectileComponentInitialization) {
    ProjectileComponent projectile;
    
    // Test default values
    EXPECT_EQ(projectile.type, ProjectileType::PlayerFire);
    EXPECT_EQ(projectile.segment, 0);
    EXPECT_EQ(projectile.yPosition, 0.0f);
    EXPECT_EQ(projectile.speed, 5.0f);
    EXPECT_EQ(projectile.damage, 1);
    EXPECT_EQ(projectile.state, ProjectileState::Active);
    EXPECT_EQ(projectile.lifetime, 10.0f);
    EXPECT_EQ(projectile.currentLifetime, 0.0f);
    EXPECT_TRUE(projectile.isPlayerProjectile);
    EXPECT_EQ(projectile.ownerId, 0);
    
    // Test type ID
    EXPECT_EQ(projectile.getTypeId(), 3);
    EXPECT_STREQ(projectile.getTypeName(), "ProjectileComponent");
}

TEST_F(ProjectileTest, ProjectileTypeInitialization) {
    // Test PlayerZap initialization
    ProjectileComponent zap;
    zap.initialize(ProjectileType::PlayerZap, 5, 2.0f, true, 0);
    
    EXPECT_EQ(zap.type, ProjectileType::PlayerZap);
    EXPECT_EQ(zap.segment, 5);
    EXPECT_EQ(zap.yPosition, 2.0f);
    EXPECT_EQ(zap.speed, 8.0f);
    EXPECT_EQ(zap.damage, 1);
    EXPECT_EQ(zap.lifetime, 2.0f);
    EXPECT_TRUE(zap.isPlayerProjectile);
    EXPECT_EQ(zap.ownerId, 0);
    
    // Test PlayerFire initialization
    ProjectileComponent fire;
    fire.initialize(ProjectileType::PlayerFire, 10, 1.5f, true, 0);
    
    EXPECT_EQ(fire.type, ProjectileType::PlayerFire);
    EXPECT_EQ(fire.segment, 10);
    EXPECT_EQ(fire.yPosition, 1.5f);
    EXPECT_EQ(fire.speed, 6.0f);
    EXPECT_EQ(fire.damage, 1);
    EXPECT_EQ(fire.lifetime, 8.0f);
    
    // Test EnemyShot initialization
    ProjectileComponent enemyShot;
    enemyShot.initialize(ProjectileType::EnemyShot, 3, 0.5f, false, 1);
    
    EXPECT_EQ(enemyShot.type, ProjectileType::EnemyShot);
    EXPECT_EQ(enemyShot.segment, 3);
    EXPECT_EQ(enemyShot.yPosition, 0.5f);
    EXPECT_EQ(enemyShot.speed, 4.0f);
    EXPECT_EQ(enemyShot.damage, 1);
    EXPECT_EQ(enemyShot.lifetime, 5.0f);
    EXPECT_FALSE(enemyShot.isPlayerProjectile);
    EXPECT_EQ(enemyShot.ownerId, 1);
    
    // Test SuperZapper initialization
    ProjectileComponent superZapper;
    superZapper.initialize(ProjectileType::SuperZapper, 0, 0.0f, true, 0);
    
    EXPECT_EQ(superZapper.type, ProjectileType::SuperZapper);
    EXPECT_EQ(superZapper.speed, 0.0f);
    EXPECT_EQ(superZapper.damage, 255);
    EXPECT_EQ(superZapper.lifetime, 0.1f);
}

TEST_F(ProjectileTest, ProjectileMovement) {
    ProjectileComponent projectile;
    projectile.initialize(ProjectileType::PlayerFire, 0, 0.0f, true, 0);
    projectile.state = ProjectileState::Active;
    
    // Test player projectile movement (moves down)
    float initialY = projectile.yPosition;
    projectileSystem_->updateProjectileMovement(projectile, 1.0f);
    EXPECT_GT(projectile.yPosition, initialY);
    
    // Test enemy projectile movement (moves up)
    ProjectileComponent enemyProjectile;
    enemyProjectile.initialize(ProjectileType::EnemyShot, 5, 5.0f, false, 1);
    enemyProjectile.state = ProjectileState::Active;
    
    float enemyInitialY = enemyProjectile.yPosition;
    projectileSystem_->updateProjectileMovement(enemyProjectile, 1.0f);
    EXPECT_LT(enemyProjectile.yPosition, enemyInitialY);
    
    // Test super zapper (no movement)
    ProjectileComponent superZapper;
    superZapper.initialize(ProjectileType::SuperZapper, 0, 0.0f, true, 0);
    superZapper.state = ProjectileState::Active;
    
    float zapperInitialY = superZapper.yPosition;
    projectileSystem_->updateProjectileMovement(superZapper, 1.0f);
    EXPECT_EQ(superZapper.yPosition, zapperInitialY);
}

TEST_F(ProjectileTest, ProjectileSpawning) {
    // Test player projectile spawning
    projectileSystem_->firePlayerFire(5, 1.0f);
    
    EXPECT_EQ(entityManager_->getEntityCount(), 1);
    EXPECT_EQ(entityManager_->getActiveEntityCount(), 1);
    
    auto* entity = entityManager_->getEntity(0);
    EXPECT_NE(entity, nullptr);
    
    if (entity) {
        EXPECT_TRUE(entity->hasComponent<ProjectileComponent>());
        auto* projectile = entity->getComponent<ProjectileComponent>();
        EXPECT_NE(projectile, nullptr);
        if (projectile) {
            EXPECT_EQ(projectile->type, ProjectileType::PlayerFire);
            EXPECT_EQ(projectile->segment, 5);
            EXPECT_EQ(projectile->yPosition, 1.0f);
            EXPECT_TRUE(projectile->isPlayerProjectile);
        }
    }
    
    // Test enemy projectile spawning
    projectileSystem_->fireEnemyShot(10, 3.0f, 1);
    
    EXPECT_EQ(entityManager_->getEntityCount(), 2);
    
    auto* enemyEntity = entityManager_->getEntity(1);
    EXPECT_NE(enemyEntity, nullptr);
    
    if (enemyEntity) {
        EXPECT_TRUE(enemyEntity->hasComponent<ProjectileComponent>());
        auto* enemyProjectile = enemyEntity->getComponent<ProjectileComponent>();
        EXPECT_NE(enemyProjectile, nullptr);
        if (enemyProjectile) {
            EXPECT_EQ(enemyProjectile->type, ProjectileType::EnemyShot);
            EXPECT_EQ(enemyProjectile->segment, 10);
            EXPECT_EQ(enemyProjectile->yPosition, 3.0f);
            EXPECT_FALSE(enemyProjectile->isPlayerProjectile);
            EXPECT_EQ(enemyProjectile->ownerId, 1);
        }
    }
}

TEST_F(ProjectileTest, ProjectileLifetime) {
    ProjectileComponent projectile;
    projectile.initialize(ProjectileType::PlayerFire, 0, 0.0f, true, 0);
    projectile.state = ProjectileState::Active;
    projectile.lifetime = 2.0f;
    projectile.currentLifetime = 0.0f;
    
    // Test lifetime update
    projectileSystem_->updateProjectileMovement(projectile, 1.0f);
    EXPECT_EQ(projectile.currentLifetime, 0.0f); // Movement doesn't update lifetime
    
    // Test lifetime expiration
    projectile.currentLifetime = 2.5f; // Exceed lifetime
    projectileSystem_->handleProjectileDeath(projectile);
    EXPECT_EQ(projectile.state, ProjectileState::Dead);
}

TEST_F(ProjectileTest, CollisionComponentInitialization) {
    CollisionComponent collision;
    
    // Test default values
    EXPECT_EQ(collision.position, glm::vec3(0.0f));
    EXPECT_EQ(collision.radius, 0.1f);
    EXPECT_TRUE(collision.isActive);
    EXPECT_EQ(collision.collisionGroup, 0);
    
    // Test type ID
    EXPECT_EQ(collision.getTypeId(), 4);
    EXPECT_STREQ(collision.getTypeName(), "CollisionComponent");
}

TEST_F(ProjectileTest, CollisionDetection) {
    // Test basic collision detection
    glm::vec3 pos1(0.0f, 0.0f, 0.0f);
    glm::vec3 pos2(0.1f, 0.0f, 0.0f);
    
    EXPECT_TRUE(collisionSystem_->checkCollision(pos1, 0.1f, pos2, 0.1f));
    
    // Test no collision
    glm::vec3 pos3(1.0f, 0.0f, 0.0f);
    EXPECT_FALSE(collisionSystem_->checkCollision(pos1, 0.1f, pos3, 0.1f));
}

TEST_F(ProjectileTest, CollisionSystemUpdate) {
    // Test collision system update
    // This should handle collision detection between all entities
    collisionSystem_->update(0.1f);
    
    // Since no entities exist yet, this should not crash
    // The real test would be with actual entities
}

TEST_F(ProjectileTest, WeaponSystemIntegration) {
    // Test player weapon firing
    projectileSystem_->firePlayerZap(5, 1.0f);
    projectileSystem_->firePlayerFire(10, 2.0f);
    
    EXPECT_EQ(entityManager_->getEntityCount(), 2);
    
    // Test super zapper
    projectileSystem_->fireSuperZapper();
    
    // Super zapper creates projectiles for all 16 segments
    EXPECT_EQ(entityManager_->getEntityCount(), 18); // 2 + 16 super zapper shots
}

TEST_F(ProjectileTest, ProjectileLimits) {
    // Test that we can't spawn more projectiles than the maximum
    // This would require access to the private shot counts
    // For now, test the basic spawn functionality
    
    // Spawn multiple projectiles
    for (int i = 0; i < 5; ++i) {
        projectileSystem_->firePlayerFire(i, 1.0f);
    }
    
    // Verify entities were created
    EXPECT_EQ(entityManager_->getEntityCount(), 5);
    EXPECT_EQ(entityManager_->getActiveEntityCount(), 5);
}

TEST_F(ProjectileTest, ProjectileStateTransitions) {
    ProjectileComponent projectile;
    projectile.initialize(ProjectileType::PlayerFire, 0, 0.0f, true, 0);
    
    // Test initial state
    EXPECT_EQ(projectile.state, ProjectileState::Active);
    
    // Test transition to exploding
    projectile.state = ProjectileState::Exploding;
    EXPECT_EQ(projectile.state, ProjectileState::Exploding);
    
    // Test transition to dead
    projectile.state = ProjectileState::Dead;
    EXPECT_EQ(projectile.state, ProjectileState::Dead);
}

TEST_F(ProjectileTest, DistanceCalculation) {
    glm::vec3 pos1(0.0f, 0.0f, 0.0f);
    glm::vec3 pos2(3.0f, 4.0f, 0.0f);
    
    float distance = collisionSystem_->calculateDistance(pos1, pos2);
    EXPECT_NEAR(distance, 5.0f, 0.001f); // 3-4-5 triangle
    
    // Test zero distance
    float zeroDistance = collisionSystem_->calculateDistance(pos1, pos1);
    EXPECT_NEAR(zeroDistance, 0.0f, 0.001f);
}

TEST_F(ProjectileTest, SegmentCollision) {
    // Test same segment
    EXPECT_TRUE(collisionSystem_->isInSameSegment(5, 5, 1.0f));
    
    // Test adjacent segments
    EXPECT_TRUE(collisionSystem_->isInSameSegment(5, 6, 1.0f));
    EXPECT_TRUE(collisionSystem_->isInSameSegment(15, 0, 1.0f)); // Wrap around
    
    // Test non-adjacent segments
    EXPECT_FALSE(collisionSystem_->isInSameSegment(5, 7, 1.0f));
    EXPECT_FALSE(collisionSystem_->isInSameSegment(0, 8, 1.0f));
} 