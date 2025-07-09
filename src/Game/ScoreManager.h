#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <spdlog/spdlog.h>
#include "../Core/EventSystem.h"

namespace Tempest {

/**
 * @brief High score entry
 */
struct HighScoreEntry {
    std::string playerName;
    uint32_t score;
    uint8_t level;
    std::string date;
    
    HighScoreEntry() : score(0), level(0) {}
    HighScoreEntry(const std::string& name, uint32_t score, uint8_t level, const std::string& date)
        : playerName(name), score(score), level(level), date(date) {}
    
    bool operator<(const HighScoreEntry& other) const {
        return score > other.score; // Higher scores first
    }
};

/**
 * @brief Score change event
 */
struct ScoreChangedEvent : public Event {
    uint32_t oldScore;
    uint32_t newScore;
    uint32_t pointsEarned;
    uint8_t playerId;
    
    ScoreChangedEvent(uint32_t oldScore, uint32_t newScore, uint32_t points, uint8_t player)
        : oldScore(oldScore), newScore(newScore), pointsEarned(points), playerId(player) {}
    
    const char* getTypeName() const override { return "ScoreChangedEvent"; }
};

/**
 * @brief High score achieved event
 */
struct HighScoreAchievedEvent : public Event {
    uint32_t score;
    uint8_t playerId;
    uint8_t rank;
    
    HighScoreAchievedEvent(uint32_t score, uint8_t player, uint8_t rank)
        : score(score), playerId(player), rank(rank) {}
    
    const char* getTypeName() const override { return "HighScoreAchievedEvent"; }
};

/**
 * @brief Score Manager for handling score tracking and high scores
 * 
 * Manages score tracking, high score system, and score persistence
 * based on the original game's RAM 0040-0045 structure.
 */
class ScoreManager {
public:
    ScoreManager();
    ~ScoreManager() = default;

    // Score management
    void initialize();
    void update(float deltaTime);
    
    // Score tracking
    void addScore(uint32_t points, uint8_t playerId = 0);
    void setScore(uint32_t score, uint8_t playerId = 0);
    uint32_t getScore(uint8_t playerId = 0) const;
    uint32_t getCurrentPlayerScore() const;
    
    // High score management
    void addHighScore(const std::string& playerName, uint32_t score, uint8_t level);
    const std::vector<HighScoreEntry>& getHighScores() const { return highScores_; }
    bool isHighScore(uint32_t score) const;
    uint8_t getHighScoreRank(uint32_t score) const;
    
    // Player management
    void setCurrentPlayer(uint8_t playerId);
    uint8_t getCurrentPlayer() const { return currentPlayer_; }
    void switchPlayer();
    
    // Score persistence
    void loadHighScores();
    void saveHighScores();
    void resetHighScores();
    
    // Score calculation
    uint32_t calculateEnemyKillScore(uint8_t enemyType, uint8_t level) const;
    uint32_t calculateLevelCompletionBonus(uint8_t level, float time) const;
    uint32_t calculateSurvivalBonus(uint8_t level, float time) const;
    
    // Event handling
    void handleEnemyKilled(const Event& event);
    void handleLevelCompleted(const Event& event);
    void handlePlayerDeath(const Event& event);
    
    // Statistics
    uint32_t getTotalScore() const;
    uint32_t getBestScore() const;
    uint8_t getBestLevel() const;
    uint32_t getGamesPlayed() const { return gamesPlayed_; }

private:
    // Score state
    uint32_t playerScores_[2];    // Scores for player 1 and 2
    uint8_t currentPlayer_;       // Current player (0 or 1)
    uint32_t gamesPlayed_;        // Total games played
    
    // High scores
    std::vector<HighScoreEntry> highScores_;
    static constexpr size_t MAX_HIGH_SCORES = 10;
    
    // Score multipliers and bonuses
    struct ScoreMultipliers {
        uint32_t enemyKillBase;       // Base score for enemy kill
        uint32_t levelCompletionBase;  // Base score for level completion
        uint32_t survivalBonusBase;    // Base survival bonus
        float difficultyMultiplier;    // Multiplier based on difficulty
    } multipliers_;
    
    // File paths
    std::string highScoreFile_;
    
    // Score calculation helpers
    uint32_t calculateDifficultyMultiplier(uint8_t level) const;
    uint32_t calculateTimeBonus(float time, uint8_t level) const;
    uint32_t calculateComboBonus(uint32_t consecutiveKills) const;
    
    // High score management
    void sortHighScores();
    void trimHighScores();
    bool isValidScore(uint32_t score) const;
    
    // File operations
    std::string getHighScoreFilePath() const;
    bool fileExists(const std::string& path) const;
    
    // Event dispatching
    void dispatchScoreChanged(uint32_t oldScore, uint32_t newScore, uint32_t points);
    void dispatchHighScoreAchieved(uint32_t score, uint8_t rank);
};

} // namespace Tempest 