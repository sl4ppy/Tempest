#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/Game/LevelManager.h"
#include "../../src/Core/EventSystem.h"
#include <spdlog/spdlog.h> // Added for debug output

using namespace Tempest;
using namespace testing;

class TestLevelManager : public ::testing::Test {
protected:
    void SetUp() override {
        levelManager_.initialize();
        EventSystem::getInstance().clearQueue();
    }
    
    void TearDown() override {
        EventSystem::getInstance().clearQueue();
    }
    
    LevelManager levelManager_;
};

TEST_F(TestLevelManager, Initialization) {
    EXPECT_EQ(levelManager_.getCurrentLevel(), 1);
    EXPECT_EQ(levelManager_.getMaxLevel(), 99);
    EXPECT_EQ(levelManager_.getDifficulty(), 1);
    EXPECT_FALSE(levelManager_.isLevelComplete());
    EXPECT_FALSE(levelManager_.isLevelFailed());
    EXPECT_EQ(levelManager_.getLevelScore(), 0);
    EXPECT_EQ(levelManager_.getEnemiesKilled(), 0);
    EXPECT_EQ(levelManager_.getEnemiesSpawned(), 0);
    EXPECT_EQ(levelManager_.getLevelTime(), 0.0f);
}

TEST_F(TestLevelManager, LevelConfiguration) {
    // Test level 1 configuration
    const auto& config1 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config1.levelNumber, 1);
    EXPECT_EQ(config1.tubeSegments, 16);
    EXPECT_EQ(config1.difficulty, 1);
    EXPECT_EQ(config1.maxEnemies, 10);
    EXPECT_EQ(config1.enemiesToKill, 20);
    
    // Test level 10 configuration
    levelManager_.setCurrentLevel(10);
    const auto& config10 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config10.levelNumber, 10);
    EXPECT_EQ(config10.difficulty, 1);
    EXPECT_EQ(config10.maxEnemies, 26);
    EXPECT_EQ(config10.enemiesToKill, 60);
    
    // Test level 30 configuration
    levelManager_.setCurrentLevel(30);
    const auto& config30 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config30.levelNumber, 30);
    EXPECT_EQ(config30.difficulty, 3);
    EXPECT_EQ(config30.maxEnemies, 85);
    EXPECT_EQ(config30.enemiesToKill, 250);
}

TEST_F(TestLevelManager, EnemyRatios) {
    // Early levels: mostly Flippers and Pulsars
    levelManager_.setCurrentLevel(3);
    const auto& config3 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config3.enemyRatios.flipper, 40);
    EXPECT_EQ(config3.enemyRatios.pulsar, 40);
    EXPECT_EQ(config3.enemyRatios.tanker, 10);
    EXPECT_EQ(config3.enemyRatios.spiker, 5);
    EXPECT_EQ(config3.enemyRatios.fuzzball, 5);
    
    // Mid levels: introduce Tankers
    levelManager_.setCurrentLevel(15);
    const auto& config15 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config15.enemyRatios.flipper, 30);
    EXPECT_EQ(config15.enemyRatios.pulsar, 30);
    EXPECT_EQ(config15.enemyRatios.tanker, 25);
    EXPECT_EQ(config15.enemyRatios.spiker, 10);
    EXPECT_EQ(config15.enemyRatios.fuzzball, 5);
    
    // Late levels: all types, more Fuzzballs
    levelManager_.setCurrentLevel(50);
    const auto& config50 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config50.enemyRatios.flipper, 20);
    EXPECT_EQ(config50.enemyRatios.pulsar, 20);
    EXPECT_EQ(config50.enemyRatios.tanker, 20);
    EXPECT_EQ(config50.enemyRatios.spiker, 20);
    EXPECT_EQ(config50.enemyRatios.fuzzball, 20);
}

TEST_F(TestLevelManager, DifficultyScaling) {
    // Level 1: difficulty 1
    levelManager_.setCurrentLevel(1);
    const auto& config1 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config1.difficulty, 1);
    EXPECT_EQ(config1.enemySpeed, 1);
    EXPECT_EQ(config1.enemySpawnRate, 1);
    EXPECT_EQ(config1.weaponEffectiveness, 9);
    
    // Level 10: difficulty 1
    levelManager_.setCurrentLevel(10);
    const auto& config10 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config10.difficulty, 1);
    EXPECT_EQ(config10.enemySpeed, 1);
    EXPECT_EQ(config10.enemySpawnRate, 1);
    EXPECT_EQ(config10.weaponEffectiveness, 9);
    
    // Level 20: difficulty 2
    levelManager_.setCurrentLevel(20);
    const auto& config20 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config20.difficulty, 2);
    EXPECT_EQ(config20.enemySpeed, 3);
    EXPECT_EQ(config20.enemySpawnRate, 2);
    EXPECT_EQ(config20.weaponEffectiveness, 8);
    
    // Level 50: difficulty 5
    levelManager_.setCurrentLevel(50);
    const auto& config50 = levelManager_.getCurrentLevelConfig();
    EXPECT_EQ(config50.difficulty, 5);
    EXPECT_EQ(config50.enemySpeed, 9);
    EXPECT_EQ(config50.enemySpawnRate, 5);
    EXPECT_EQ(config50.weaponEffectiveness, 5);
}

TEST_F(TestLevelManager, LevelProgression) {
    EXPECT_EQ(levelManager_.getCurrentLevel(), 1);
    
    levelManager_.advanceLevel();
    EXPECT_EQ(levelManager_.getCurrentLevel(), 2);
    
    levelManager_.advanceLevel();
    EXPECT_EQ(levelManager_.getCurrentLevel(), 3);
    
    // Test max level
    levelManager_.setCurrentLevel(99);
    levelManager_.advanceLevel();
    EXPECT_EQ(levelManager_.getCurrentLevel(), 99); // Should not exceed max
}

TEST_F(TestLevelManager, LevelCompletion) {
    levelManager_.startLevel(1);
    EXPECT_FALSE(levelManager_.isLevelComplete());
    EXPECT_FALSE(levelManager_.isLevelFailed());
    
    // Simulate killing required enemies
    for (int i = 0; i < 20; ++i) {
        // Create a simple test event
        struct TestEvent : public Event {
            const char* getTypeName() const override { return "TestEvent"; }
        };
        levelManager_.handleEnemyKilled(TestEvent{});
    }
    
    // Update to trigger completion check
    levelManager_.update(0.1f);
    
    EXPECT_TRUE(levelManager_.isLevelComplete());
    EXPECT_FALSE(levelManager_.isLevelFailed());
}

TEST_F(TestLevelManager, LevelFailure) {
    levelManager_.startLevel(1);
    EXPECT_FALSE(levelManager_.isLevelComplete());
    EXPECT_FALSE(levelManager_.isLevelFailed());
    
    // Simulate player death
    struct TestEvent : public Event {
        const char* getTypeName() const override { return "TestEvent"; }
    };
    levelManager_.handlePlayerDeath(TestEvent{});
    
    EXPECT_FALSE(levelManager_.isLevelComplete());
    EXPECT_TRUE(levelManager_.isLevelFailed());
}

TEST_F(TestLevelManager, LevelRestart) {
    levelManager_.startLevel(5);
    EXPECT_EQ(levelManager_.getCurrentLevel(), 5);
    EXPECT_EQ(levelManager_.getLevelScore(), 0);
    EXPECT_EQ(levelManager_.getEnemiesKilled(), 0);
    EXPECT_EQ(levelManager_.getLevelTime(), 0.0f);
    
    // Simulate some progress
    struct TestEvent : public Event {
        const char* getTypeName() const override { return "TestEvent"; }
    };
    levelManager_.handleEnemyKilled(TestEvent{});
    levelManager_.update(1.0f);
    
    EXPECT_EQ(levelManager_.getEnemiesKilled(), 1);
    EXPECT_GT(levelManager_.getLevelTime(), 0.0f);
    
    // Restart level
    levelManager_.restartLevel();
    
    EXPECT_EQ(levelManager_.getCurrentLevel(), 5);
    EXPECT_EQ(levelManager_.getLevelScore(), 0);
    EXPECT_EQ(levelManager_.getEnemiesKilled(), 0);
    EXPECT_EQ(levelManager_.getLevelTime(), 0.0f);
}

TEST_F(TestLevelManager, InvalidLevelHandling) {
    // Test invalid level numbers
    levelManager_.setCurrentLevel(0); // Invalid
    EXPECT_EQ(levelManager_.getCurrentLevel(), 1); // Should remain at valid level
    
    levelManager_.setCurrentLevel(100); // Invalid
    EXPECT_EQ(levelManager_.getCurrentLevel(), 1); // Should remain at valid level
    
    // Test invalid level config access
    auto& config = levelManager_.getLevelConfig(0);
    EXPECT_EQ(config.levelNumber, 1); // Should return default config
}

TEST_F(TestLevelManager, LevelTimeTracking) {
    levelManager_.startLevel(1);
    EXPECT_EQ(levelManager_.getLevelTime(), 0.0f);
    
    levelManager_.update(1.0f);
    EXPECT_EQ(levelManager_.getLevelTime(), 1.0f);
    
    levelManager_.update(0.5f);
    EXPECT_EQ(levelManager_.getLevelTime(), 1.5f);
    
    // Time should stop when level is complete
    levelManager_.completeLevel();
    float timeBefore = levelManager_.getLevelTime();
    levelManager_.update(1.0f);
    EXPECT_EQ(levelManager_.getLevelTime(), timeBefore); // Should not change
}

TEST_F(TestLevelManager, LevelConfigurationValidation) {
    LevelConfig validConfig;
    validConfig.levelNumber = 5;
    validConfig.tubeSegments = 16;
    validConfig.difficulty = 3;
    validConfig.enemyRatios = {25, 25, 25, 15, 10};
    
    // Should not throw
    EXPECT_NO_THROW(levelManager_.setLevelConfig(5, validConfig));
    
    // Test invalid configs
    LevelConfig invalidConfig = validConfig;
    invalidConfig.levelNumber = 0;
    EXPECT_THROW(levelManager_.setLevelConfig(5, invalidConfig), std::invalid_argument);
    
    invalidConfig = validConfig;
    invalidConfig.tubeSegments = 8;
    EXPECT_THROW(levelManager_.setLevelConfig(5, invalidConfig), std::invalid_argument);
    
    invalidConfig = validConfig;
    invalidConfig.difficulty = 11;
    EXPECT_THROW(levelManager_.setLevelConfig(5, invalidConfig), std::invalid_argument);
    
    invalidConfig = validConfig;
    invalidConfig.enemyRatios = {20, 20, 20, 20, 10}; // Sum = 90, not 100
    EXPECT_THROW(levelManager_.setLevelConfig(5, invalidConfig), std::invalid_argument);
}

TEST_F(TestLevelManager, EventDispatching) {
    levelManager_.startLevel(1);
    // Simulate level completion
    levelManager_.completeLevel();
    // If no exception is thrown, the test passes
    SUCCEED();
}

TEST_F(TestLevelManager, LevelStatistics) {
    levelManager_.startLevel(1);
    
    // Simulate gameplay
    struct TestEvent : public Event {
        const char* getTypeName() const override { return "TestEvent"; }
    };
    for (int i = 0; i < 5; ++i) {
        levelManager_.handleEnemyKilled(TestEvent{});
    }
    
    levelManager_.update(2.5f);
    
    EXPECT_EQ(levelManager_.getEnemiesKilled(), 5);
    EXPECT_EQ(levelManager_.getLevelTime(), 2.5f);
    EXPECT_EQ(levelManager_.getEnemiesSpawned(), 0); // Not implemented yet
}

TEST_F(TestLevelManager, DifficultySetting) {
    levelManager_.setDifficulty(5);
    EXPECT_EQ(levelManager_.getDifficulty(), 5);
    
    // Test bounds
    levelManager_.setDifficulty(0);
    EXPECT_EQ(levelManager_.getDifficulty(), 1); // Should clamp to minimum
    
    levelManager_.setDifficulty(15);
    EXPECT_EQ(levelManager_.getDifficulty(), 10); // Should clamp to maximum
} 