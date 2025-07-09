#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <spdlog/spdlog.h>
#include "../Core/EventSystem.h"

namespace Tempest {

/**
 * @brief Level configuration based on original game analysis
 */
struct LevelConfig {
    uint8_t levelNumber;          // Level number (1-99)
    uint8_t tubeSegments;         // Number of tube segments (16 for all levels)
    uint16_t maxEnemies;         // Max enemies on screen
    uint16_t enemiesToKill;      // Enemies to kill for level completion
    uint8_t enemySpawnRate;       // Enemy spawn rate multiplier
    uint8_t enemySpeed;           // Enemy movement speed
    uint8_t weaponEffectiveness;  // Weapon effectiveness modifier
    uint8_t difficulty;           // Overall difficulty (1-10)
    
    // Enemy type ratios for this level
    struct {
        uint8_t flipper;          // Type 1: Flipper ratio
        uint8_t pulsar;           // Type 2: Pulsar ratio
        uint8_t tanker;           // Type 3: Tanker ratio
        uint8_t spiker;           // Type 4: Spiker ratio
        uint8_t fuzzball;         // Type 5: Fuzzball ratio
    } enemyRatios;
    
    // Level completion requirements
    uint8_t timeLimit;            // Time limit in seconds (0 = no limit)
    
    LevelConfig() : levelNumber(1), tubeSegments(16), maxEnemies(10), 
                   enemySpawnRate(1), enemySpeed(1), weaponEffectiveness(1), 
                   difficulty(1), enemiesToKill(20), timeLimit(0) {
        enemyRatios = {20, 20, 20, 20, 20}; // Default equal distribution
    }
};

/**
 * @brief Level completion event
 */
struct LevelCompletedEvent : public Event {
    uint8_t levelNumber;
    uint32_t score;
    uint8_t livesRemaining;
    float completionTime;
    
    LevelCompletedEvent(uint8_t level, uint32_t score, uint8_t lives, float time)
        : levelNumber(level), score(score), livesRemaining(lives), completionTime(time) {}
    
    const char* getTypeName() const override { return "LevelCompletedEvent"; }
};

/**
 * @brief Level failed event
 */
struct LevelFailedEvent : public Event {
    uint8_t levelNumber;
    uint32_t score;
    std::string failureReason;
    
    LevelFailedEvent(uint8_t level, uint32_t score, const std::string& reason)
        : levelNumber(level), score(score), failureReason(reason) {}
    
    const char* getTypeName() const override { return "LevelFailedEvent"; }
};

/**
 * @brief Level Manager for handling level progression and configuration
 * 
 * Manages level selection, difficulty scaling, and level completion logic
 * based on the original game's level system.
 */
class LevelManager {
public:
    LevelManager();
    ~LevelManager() = default;

    // Level management
    void initialize();
    void update(float deltaTime);
    
    // Level progression
    void startLevel(uint8_t levelNumber);
    void completeLevel();
    void failLevel(const std::string& reason);
    void restartLevel();
    
    // Level selection
    void setCurrentLevel(uint8_t levelNumber);
    uint8_t getCurrentLevel() const { return currentLevel_; }
    uint8_t getMaxLevel() const { return maxLevel_; }
    
    // Level configuration
    const LevelConfig& getCurrentLevelConfig() const;
    LevelConfig& getLevelConfig(uint8_t levelNumber);
    void setLevelConfig(uint8_t levelNumber, const LevelConfig& config);
    
    // Difficulty and progression
    void setDifficulty(uint8_t difficulty);
    uint8_t getDifficulty() const { return difficulty_; }
    void advanceLevel();
    bool isLevelComplete() const { return levelComplete_; }
    bool isLevelFailed() const { return levelFailed_; }
    
    // Level statistics
    uint32_t getLevelScore() const { return levelScore_; }
    uint8_t getEnemiesKilled() const { return enemiesKilled_; }
    uint8_t getEnemiesSpawned() const { return enemiesSpawned_; }
    float getLevelTime() const { return levelTime_; }
    
    // Level completion requirements
    bool checkLevelCompletion() const;
    bool checkLevelFailure() const;
    
    // Event handling
    void handleEnemyKilled(const Event& event);
    void handlePlayerDeath(const Event& event);
    void handleScoreChanged(const Event& event);

private:
    // Level state
    uint8_t currentLevel_;
    uint8_t maxLevel_;
    uint8_t difficulty_;
    bool levelComplete_;
    bool levelFailed_;
    
    // Level statistics
    uint32_t levelScore_;
    uint8_t enemiesKilled_;
    uint8_t enemiesSpawned_;
    float levelTime_;
    float levelStartTime_;
    
    // Level configurations
    std::vector<LevelConfig> levelConfigs_;
    
    // Level generation
    void generateLevelConfigs();
    LevelConfig generateLevelConfig(uint8_t levelNumber);
    void calculateEnemyRatios(LevelConfig& config, uint8_t levelNumber);
    void calculateDifficulty(LevelConfig& config, uint8_t levelNumber);
    
    // Event dispatching
    void dispatchLevelCompleted();
    void dispatchLevelFailed(const std::string& reason);
    
    // Validation
    bool isValidLevel(uint8_t levelNumber) const;
    void validateLevelConfig(const LevelConfig& config) const;
};

} // namespace Tempest 