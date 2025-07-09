#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "Game/Player.h"
#include "Game/TubeGeometry.h"
#include "Core/EventSystem.h"

using namespace Tempest;

class PlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        spdlog::set_level(spdlog::level::debug);
        player_.initialize();
    }
    
    void TearDown() override {
        // Clean up
    }
    
    Player player_;
    TubeGeometry tubeGeometry_;
};

TEST_F(PlayerTest, PlayerInitialization) {
    EXPECT_TRUE(player_.isAlive());
    EXPECT_EQ(player_.getLives(), 3);
    EXPECT_EQ(player_.getSegment(), 0);
    EXPECT_FALSE(player_.isInvulnerable());
    EXPECT_EQ(player_.getZapEnergy(), 255);
    EXPECT_EQ(player_.getFireEnergy(), 255);
    EXPECT_EQ(player_.getSuperZapperUses(), 2);
}

TEST_F(PlayerTest, PlayerMovement) {
    auto& component = player_.getComponent();
    
    // Test movement to the right
    PlayerInputEvent moveRightEvent(PlayerInputEvent::InputType::MoveRight, true);
    player_.handleInput(moveRightEvent);
    
    EXPECT_EQ(component.targetSegment, 1);
    EXPECT_TRUE(component.isMoving);
    EXPECT_EQ(component.moveDirection, 1);
    
    // Test that new input is ignored while moving (no queuing)
    PlayerInputEvent moveLeftEvent(PlayerInputEvent::InputType::MoveLeft, true);
    player_.handleInput(moveLeftEvent);
    
    // Should still be moving to the right, not queued to move left
    EXPECT_EQ(component.targetSegment, 1);
    EXPECT_TRUE(component.isMoving);
    EXPECT_EQ(component.moveDirection, 1);
    
    // Complete the movement first
    component.isMoving = false;
    component.segmentLerp = 1.0f;
    component.segment = 1;
    
    // Now test left movement from segment 1
    player_.handleInput(moveLeftEvent);
    
    EXPECT_EQ(component.targetSegment, 0); // Should move from 1 to 0
    EXPECT_TRUE(component.isMoving);
    EXPECT_EQ(component.moveDirection, -1);
    
    // Complete the movement to segment 0
    component.isMoving = false;
    component.segmentLerp = 1.0f;
    component.segment = 0; // Ensure we are at segment 0
    // Now test left movement from segment 0 (should wrap to 15)
    player_.handleInput(moveLeftEvent);
    EXPECT_EQ(component.targetSegment, 15); // Wrap from 0 to 15
    EXPECT_TRUE(component.isMoving);
    EXPECT_EQ(component.moveDirection, -1);
}

TEST_F(PlayerTest, PlayerWeapons) {
    auto& component = player_.getComponent();
    
    // Test Zap weapon
    PlayerInputEvent zapEvent(PlayerInputEvent::InputType::Zap, true);
    player_.handleInput(zapEvent);
    
    EXPECT_TRUE(component.zapPressed);
    EXPECT_EQ(component.zapEnergy, 245); // 255 - 10 cost
    EXPECT_GT(component.weaponCooldown, 0.0f);
    
    // Test Fire weapon
    component.weaponCooldown = 0.0f; // Reset cooldown
    PlayerInputEvent fireEvent(PlayerInputEvent::InputType::Fire, true);
    player_.handleInput(fireEvent);
    
    EXPECT_TRUE(component.firePressed);
    EXPECT_EQ(component.fireEnergy, 235); // 245 - 20 cost
    EXPECT_GT(component.weaponCooldown, 0.0f);
    
    // Test Super Zapper
    component.weaponCooldown = 0.0f; // Reset cooldown
    PlayerInputEvent superZapperEvent(PlayerInputEvent::InputType::SuperZapper, true);
    player_.handleInput(superZapperEvent);
    
    EXPECT_TRUE(component.superZapperPressed);
    EXPECT_EQ(component.superZapperUses, 1);
    EXPECT_GT(component.weaponCooldown, 0.0f);
}

TEST_F(PlayerTest, PlayerDamage) {
    EXPECT_TRUE(player_.isAlive());
    EXPECT_EQ(player_.getLives(), 3);
    
    // Take damage
    player_.takeDamage();
    
    EXPECT_FALSE(player_.isAlive());
    EXPECT_EQ(player_.getLives(), 2);
    EXPECT_TRUE(player_.isInvulnerable());
    
    // Try to take damage while invulnerable
    player_.takeDamage();
    EXPECT_EQ(player_.getLives(), 2); // Should not lose another life
}

TEST_F(PlayerTest, PlayerRespawn) {
    player_.takeDamage();
    EXPECT_FALSE(player_.isAlive());
    
    player_.respawn();
    EXPECT_TRUE(player_.isAlive());
    EXPECT_TRUE(player_.isInvulnerable());
    EXPECT_EQ(player_.getZapEnergy(), 255);
    EXPECT_EQ(player_.getFireEnergy(), 255);
}

TEST_F(PlayerTest, PlayerAddLife) {
    EXPECT_EQ(player_.getLives(), 3);
    
    player_.addLife();
    EXPECT_EQ(player_.getLives(), 4);
}

TEST_F(PlayerTest, TubeGeometryInitialization) {
    tubeGeometry_.initialize();
    
    EXPECT_EQ(tubeGeometry_.getVertexCount(), 176); // 16 segments * 11 depth levels
    EXPECT_GT(tubeGeometry_.getIndexCount(), 0);
    EXPECT_EQ(tubeGeometry_.getTubeRadius(), 1.0f);
    EXPECT_EQ(tubeGeometry_.getTubeDepth(), 10.0f);
}

TEST_F(PlayerTest, TubeGeometryPositionCalculation) {
    tubeGeometry_.initialize();
    
    // Test segment position calculation with constant radius
    glm::vec3 pos0 = tubeGeometry_.getSegmentPosition(0, 0.0f);
    EXPECT_NEAR(pos0.x, 1.0f, 0.001f);
    EXPECT_NEAR(pos0.y, 0.0f, 0.001f);
    EXPECT_NEAR(pos0.z, 0.0f, 0.001f);
    
    // Test that radius is constant at different depths
    glm::vec3 pos0_deep = tubeGeometry_.getSegmentPosition(0, 5.0f);
    EXPECT_NEAR(pos0_deep.x, 1.0f, 0.001f); // Same radius at depth
    EXPECT_NEAR(pos0_deep.y, 5.0f, 0.001f);
    EXPECT_NEAR(pos0_deep.z, 0.0f, 0.001f);
    
    glm::vec3 pos8 = tubeGeometry_.getSegmentPosition(8, 5.0f);
    EXPECT_NEAR(pos8.x, -1.0f, 0.001f); // Constant radius
    EXPECT_NEAR(pos8.y, 5.0f, 0.001f);
    EXPECT_NEAR(pos8.z, 0.0f, 0.001f);
    
    // Test that radius doesn't change with depth
    glm::vec3 pos8_shallow = tubeGeometry_.getSegmentPosition(8, 0.0f);
    EXPECT_NEAR(pos8_shallow.x, -1.0f, 0.001f); // Same radius
    EXPECT_NEAR(pos8_shallow.y, 0.0f, 0.001f);
    EXPECT_NEAR(pos8_shallow.z, 0.0f, 0.001f);
}

TEST_F(PlayerTest, TubeGeometryCollision) {
    tubeGeometry_.initialize();
    
    // Test collision detection
    glm::vec3 pointInside(0.5f, 5.0f, 0.5f);
    EXPECT_TRUE(tubeGeometry_.isPointInTube(pointInside));
    
    glm::vec3 pointOutside(2.0f, 5.0f, 2.0f);
    EXPECT_FALSE(tubeGeometry_.isPointInTube(pointOutside));
    
    // Test segment visibility
    EXPECT_TRUE(tubeGeometry_.isSegmentVisible(0, 0.0f));
    EXPECT_TRUE(tubeGeometry_.isSegmentVisible(8, 5.0f));
}

TEST_F(PlayerTest, TubeGeometryUtilityFunctions) {
    tubeGeometry_.initialize();
    
    // Test angle calculations
    float angle0 = tubeGeometry_.getAngleFromSegment(0);
    EXPECT_NEAR(angle0, 0.0f, 0.001f);
    
    float angle8 = tubeGeometry_.getAngleFromSegment(8);
    EXPECT_NEAR(angle8, 3.14159f, 0.001f);
    
    // Test segment from angle
    uint8_t segment0 = tubeGeometry_.getSegmentFromAngle(0.0f);
    EXPECT_EQ(segment0, 0);
    
    uint8_t segment8 = tubeGeometry_.getSegmentFromAngle(3.14159f);
    EXPECT_EQ(segment8, 8);
    
    // Test distance between segments
    float distance = tubeGeometry_.getDistanceBetweenSegments(0, 8);
    EXPECT_NEAR(distance, 3.14159f, 0.001f);
}

TEST_F(PlayerTest, PlayerUpdate) {
    auto& component = player_.getComponent();
    
    // Ensure player is alive and has proper movement speed
    component.isAlive = true;
    component.moveSpeed = 5.0f;
    
    // Set up movement: segment != targetSegment
    component.segment = 0;
    component.targetSegment = 1;
    component.isMoving = true;
    component.segmentLerp = 0.5f;
    
    // Debug output
    spdlog::debug("Before update: segment={}, targetSegment={}, isMoving={}, segmentLerp={}, moveSpeed={}", 
                  component.segment, component.targetSegment, component.isMoving, component.segmentLerp, component.moveSpeed);
    
    // Update player with smaller deltaTime so movement doesn't complete
    player_.update(0.05f); // Use 0.05 instead of 0.1
    
    // Debug output
    spdlog::debug("After update: segment={}, targetSegment={}, isMoving={}, segmentLerp={}", 
                  component.segment, component.targetSegment, component.isMoving, component.segmentLerp);
    
    // Check that movement progressed (0.5 + 5.0 * 0.05 = 0.75)
    EXPECT_GT(component.segmentLerp, 0.5f);

    // Test update does not progress movement if not moving
    component.isMoving = false;
    component.segmentLerp = 0.0f;
    player_.update(0.1f);
    EXPECT_EQ(component.segmentLerp, 0.0f);

    // Test update does not progress movement if segment == targetSegment
    component.isMoving = true;
    component.segment = 2;
    component.targetSegment = 2;
    component.segmentLerp = 0.3f;
    player_.update(0.1f);
    EXPECT_EQ(component.segmentLerp, 0.3f);
} 