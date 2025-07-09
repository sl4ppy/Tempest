#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/Game/Enemy.h"
#include "../../src/Core/EntityManager.h"
#include "../../src/Core/EventSystem.h"
#include <memory>

using namespace Tempest;

class EnemyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create entity manager and enemy system
        entityManager_ = std::make_unique<EntityManager>();
        enemySystem_ = std::make_unique<EnemySystem>(*entityManager_);
    }
    
    void TearDown() override {
        // No EventSystem shutdown needed
    }
    
    std::unique_ptr<EntityManager> entityManager_;
    std::unique_ptr<EnemySystem> enemySystem_;
};

TEST_F(EnemyTest, EnemyComponentInitialization) {
    EnemyComponent enemy;
    
    // Test default values
    EXPECT_EQ(enemy.type, EnemyType::Flipper);
    EXPECT_EQ(enemy.segment, 0);
    EXPECT_EQ(enemy.along, 0.0f);
    EXPECT_EQ(enemy.health, 1);
    EXPECT_EQ(enemy.maxHealth, 1);
    EXPECT_EQ(enemy.state, EnemyState::Spawning);
    EXPECT_EQ(enemy.movementType, MovementType::NoAdjustment);
    EXPECT_FALSE(enemy.isActive);
    
    // Test type ID
    EXPECT_EQ(enemy.getTypeId(), 2);
    EXPECT_STREQ(enemy.getTypeName(), "EnemyComponent");
}

TEST_F(EnemyTest, EnemyTypeInitialization) {
    // Test Flipper initialization
    EnemyComponent flipper;
    flipper.initialize(EnemyType::Flipper, 5, 2.0f);
    
    EXPECT_EQ(flipper.type, EnemyType::Flipper);
    EXPECT_EQ(flipper.segment, 5);
    EXPECT_EQ(flipper.along, 2.0f);
    EXPECT_EQ(flipper.health, 1);
    EXPECT_EQ(flipper.maxHealth, 1);
    EXPECT_EQ(flipper.moveSpeed, 2.0f);
    EXPECT_EQ(flipper.attackCooldown, 1.5f);
    EXPECT_TRUE(flipper.isActive);
    EXPECT_EQ(flipper.state, EnemyState::Spawning);
    
    // Test Pulsar initialization
    EnemyComponent pulsar;
    pulsar.initialize(EnemyType::Pulsar, 10, 1.5f);
    
    EXPECT_EQ(pulsar.type, EnemyType::Pulsar);
    EXPECT_EQ(pulsar.segment, 10);
    EXPECT_EQ(pulsar.along, 1.5f);
    EXPECT_EQ(pulsar.health, 2);
    EXPECT_EQ(pulsar.maxHealth, 2);
    EXPECT_EQ(pulsar.moveSpeed, 1.5f);
    EXPECT_EQ(pulsar.attackCooldown, 2.0f);
    
    // Test Tanker initialization
    EnemyComponent tanker;
    tanker.initialize(EnemyType::Tanker, 3, 0.5f);
    
    EXPECT_EQ(tanker.type, EnemyType::Tanker);
    EXPECT_EQ(tanker.health, 3);
    EXPECT_EQ(tanker.maxHealth, 3);
    EXPECT_EQ(tanker.moveSpeed, 0.8f);
    EXPECT_EQ(tanker.attackCooldown, 3.0f);
    
    // Test Spiker initialization
    EnemyComponent spiker;
    spiker.initialize(EnemyType::Spiker, 7, 3.0f);
    
    EXPECT_EQ(spiker.type, EnemyType::Spiker);
    EXPECT_EQ(spiker.health, 1);
    EXPECT_EQ(spiker.maxHealth, 1);
    EXPECT_EQ(spiker.moveSpeed, 3.0f);
    EXPECT_EQ(spiker.attackCooldown, 1.0f);
    
    // Test Fuzzball initialization
    EnemyComponent fuzzball;
    fuzzball.initialize(EnemyType::Fuzzball, 12, 1.0f);
    
    EXPECT_EQ(fuzzball.type, EnemyType::Fuzzball);
    EXPECT_EQ(fuzzball.health, 2);
    EXPECT_EQ(fuzzball.maxHealth, 2);
    EXPECT_EQ(fuzzball.moveSpeed, 1.0f);
    EXPECT_EQ(fuzzball.attackCooldown, 2.5f);
}

TEST_F(EnemyTest, EnemyMovement) {
    EnemyComponent enemy;
    enemy.initialize(EnemyType::Flipper, 0, 0.0f);
    enemy.state = EnemyState::Moving;
    
    // Test no movement
    enemy.movementType = MovementType::NoAdjustment;
    uint8_t originalSegment = enemy.segment;
    enemySystem_->updateEnemyMovement(enemy, 1.0f);
    EXPECT_EQ(enemy.segment, originalSegment);
    
    // Test clockwise movement
    enemy.movementType = MovementType::Clockwise;
    enemy.segment = 0;
    enemySystem_->updateEnemyMovement(enemy, 1.0f);
    EXPECT_EQ(enemy.segment, 1);
    
    // Test wrap-around clockwise
    enemy.segment = 15;
    enemySystem_->updateEnemyMovement(enemy, 1.0f);
    EXPECT_EQ(enemy.segment, 0);
    
    // Test counter-clockwise movement
    enemy.movementType = MovementType::CounterClockwise;
    enemy.segment = 1;
    enemySystem_->updateEnemyMovement(enemy, 1.0f);
    EXPECT_EQ(enemy.segment, 0);
    
    // Test wrap-around counter-clockwise
    enemy.segment = 0;
    enemySystem_->updateEnemyMovement(enemy, 1.0f);
    EXPECT_EQ(enemy.segment, 15);
    
    // Test position calculation
    enemy.segment = 0;
    enemy.along = 5.0f;
    enemySystem_->updateEnemyMovement(enemy, 1.0f);
    float angle = (enemy.segment * 22.5f) * 3.14159f / 180.0f;
    float radius = 1.0f;
    float expectedX = radius * std::cos(angle);
    float expectedY = enemy.along;
    float expectedZ = radius * std::sin(angle);
    EXPECT_NEAR(enemy.position.x, expectedX, 0.001f);
    EXPECT_NEAR(enemy.position.y, expectedY, 0.001f);
    EXPECT_NEAR(enemy.position.z, expectedZ, 0.001f);
}

TEST_F(EnemyTest, EnemyAIBehavior) {
    EnemyComponent enemy;
    enemy.initialize(EnemyType::Flipper, 5, 2.0f);
    enemy.state = EnemyState::Moving;
    enemy.currentCooldown = 0.0f;
    
    // Test cooldown update
    enemy.currentCooldown = 2.0f;
    enemySystem_->updateEnemyAI(enemy, 1.0f);
    EXPECT_EQ(enemy.currentCooldown, 1.0f);
    
    // Test attack cooldown reset
    enemy.currentCooldown = 0.0f;
    enemySystem_->updateEnemyAI(enemy, 1.0f);
    EXPECT_EQ(enemy.currentCooldown, enemy.attackCooldown);
}

TEST_F(EnemyTest, EnemyDeath) {
    EnemyComponent enemy;
    enemy.initialize(EnemyType::Pulsar, 3, 1.0f);
    enemy.state = EnemyState::Moving;
    enemy.isActive = true;
    
    // Test death handling
    enemySystem_->handleEnemyDeath(enemy);
    
    EXPECT_EQ(enemy.state, EnemyState::Dead);
    EXPECT_FALSE(enemy.isActive);
}

TEST_F(EnemyTest, EnemySpawnLimits) {
    // Test that we can't spawn more enemies than the maximum
    // This would require access to the private enemy counts
    // For now, test the basic spawn functionality
    
    // Spawn a Flipper
    enemySystem_->spawnEnemy(EnemyType::Flipper, 0, 0.0f);
    
    // Verify entity was created by checking entity count
    EXPECT_EQ(entityManager_->getEntityCount(), 1);
    EXPECT_EQ(entityManager_->getActiveEntityCount(), 1);
    
    // Verify the entity has an EnemyComponent
    auto entityId = 0; // First entity should have ID 0
    auto* entity = entityManager_->getEntity(entityId);
    EXPECT_NE(entity, nullptr);
    
    if (entity) {
        EXPECT_TRUE(entity->hasComponent<EnemyComponent>());
        auto* enemy = entity->getComponent<EnemyComponent>();
        EXPECT_NE(enemy, nullptr);
        if (enemy) {
            EXPECT_EQ(enemy->type, EnemyType::Flipper);
            EXPECT_TRUE(enemy->isActive);
        }
    }
}

TEST_F(EnemyTest, EnemyTypeSpecificBehavior) {
    // Test Flipper behavior
    EnemyComponent flipper;
    flipper.initialize(EnemyType::Flipper, 5, 1.0f);
    flipper.state = EnemyState::Moving;
    flipper.movementType = MovementType::Clockwise;
    
    // Flippers should change direction occasionally
    // This is probabilistic, so we test the basic structure
    enemySystem_->updateFlipperBehavior(flipper, 1.0f);
    // Direction change is random, so we just verify the function doesn't crash
    
    // Test Pulsar behavior
    EnemyComponent pulsar;
    pulsar.initialize(EnemyType::Pulsar, 8, 2.0f);
    pulsar.state = EnemyState::Moving;
    pulsar.movementType = MovementType::CounterClockwise;
    
    enemySystem_->updatePulsarBehavior(pulsar, 1.0f);
    // Verify function doesn't crash
    
    // Test Tanker behavior
    EnemyComponent tanker;
    tanker.initialize(EnemyType::Tanker, 12, 0.5f);
    tanker.state = EnemyState::Moving;
    
    enemySystem_->updateTankerBehavior(tanker, 1.0f);
    // Verify function doesn't crash
    
    // Test Spiker behavior
    EnemyComponent spiker;
    spiker.initialize(EnemyType::Spiker, 1, 3.0f);
    spiker.state = EnemyState::Moving;
    
    enemySystem_->updateSpikerBehavior(spiker, 1.0f);
    // Verify function doesn't crash
    
    // Test Fuzzball behavior
    EnemyComponent fuzzball;
    fuzzball.initialize(EnemyType::Fuzzball, 15, 1.5f);
    fuzzball.state = EnemyState::Moving;
    
    enemySystem_->updateFuzzballBehavior(fuzzball, 1.0f);
    // Verify function doesn't crash
}

TEST_F(EnemyTest, EnemySystemUpdate) {
    // Test the main update loop
    // This should handle spawning and updating all enemies
    
    // Set up initial state
    enemySystem_->update(0.1f); // Small time step
    
    // The update should handle spawn timers and enemy updates
    // Since this is mostly internal logic, we just verify it doesn't crash
    
    // Test with larger time step to trigger spawning
    enemySystem_->update(3.0f); // Should trigger spawn timer
}

TEST_F(EnemyTest, EnemyStateTransitions) {
    EnemyComponent enemy;
    enemy.initialize(EnemyType::Flipper, 0, 0.0f);
    
    // Test initial state
    EXPECT_EQ(enemy.state, EnemyState::Spawning);
    EXPECT_TRUE(enemy.isActive);
    
    // Test transition to moving
    enemy.setState(EnemyState::Moving);
    EXPECT_EQ(enemy.state, EnemyState::Moving);
    
    // Test transition to attacking
    enemy.setState(EnemyState::Attacking);
    EXPECT_EQ(enemy.state, EnemyState::Attacking);
    
    // Test transition to dying
    enemy.setState(EnemyState::Dying);
    EXPECT_EQ(enemy.state, EnemyState::Dying);
    
    // Test transition to dead
    enemy.setState(EnemyState::Dead);
    EXPECT_EQ(enemy.state, EnemyState::Dead);
    EXPECT_FALSE(enemy.isActive);
} 