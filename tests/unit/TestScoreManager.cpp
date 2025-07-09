#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../src/Game/ScoreManager.h"
#include "../../src/Core/EventSystem.h"
#include <filesystem>

using namespace Tempest;
using namespace testing;

class TestScoreManager : public ::testing::Test {
protected:
    void SetUp() override {
        // Remove any existing high score file
        if (std::filesystem::exists("highscores.dat")) {
            std::filesystem::remove("highscores.dat");
        }
        
        scoreManager_.initialize();
        EventSystem::getInstance().clearQueue();
    }
    
    void TearDown() override {
        EventSystem::getInstance().clearQueue();
        
        // Clean up high score file
        if (std::filesystem::exists("highscores.dat")) {
            std::filesystem::remove("highscores.dat");
        }
    }
    
    ScoreManager scoreManager_;
};

TEST_F(TestScoreManager, Initialization) {
    EXPECT_EQ(scoreManager_.getScore(0), 0);
    EXPECT_EQ(scoreManager_.getScore(1), 0);
    EXPECT_EQ(scoreManager_.getCurrentPlayer(), 0);
    EXPECT_EQ(scoreManager_.getCurrentPlayerScore(), 0);
    EXPECT_EQ(scoreManager_.getGamesPlayed(), 0);
    EXPECT_EQ(scoreManager_.getTotalScore(), 0);
    EXPECT_EQ(scoreManager_.getBestScore(), 0);
    EXPECT_EQ(scoreManager_.getBestLevel(), 1);
}

TEST_F(TestScoreManager, ScoreTracking) {
    // Test adding score to player 0
    scoreManager_.addScore(100, 0);
    EXPECT_EQ(scoreManager_.getScore(0), 100);
    EXPECT_EQ(scoreManager_.getScore(1), 0);
    EXPECT_EQ(scoreManager_.getCurrentPlayerScore(), 100);
    
    // Test adding score to player 1
    scoreManager_.addScore(250, 1);
    EXPECT_EQ(scoreManager_.getScore(0), 100);
    EXPECT_EQ(scoreManager_.getScore(1), 250);
    
    // Test setting score
    scoreManager_.setScore(500, 0);
    EXPECT_EQ(scoreManager_.getScore(0), 500);
    EXPECT_EQ(scoreManager_.getScore(1), 250);
}

TEST_F(TestScoreManager, PlayerManagement) {
    EXPECT_EQ(scoreManager_.getCurrentPlayer(), 0);
    
    scoreManager_.setCurrentPlayer(1);
    EXPECT_EQ(scoreManager_.getCurrentPlayer(), 1);
    EXPECT_EQ(scoreManager_.getCurrentPlayerScore(), 0);
    
    scoreManager_.switchPlayer();
    EXPECT_EQ(scoreManager_.getCurrentPlayer(), 0);
    
    scoreManager_.switchPlayer();
    EXPECT_EQ(scoreManager_.getCurrentPlayer(), 1);
}

TEST_F(TestScoreManager, ScoreCalculation) {
    // Test enemy kill scores
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(1, 1), 100); // Flipper, level 1
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(2, 1), 150); // Pulsar, level 1
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(3, 1), 200); // Tanker, level 1
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(4, 1), 250); // Spiker, level 1
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(5, 1), 300); // Fuzzball, level 1
    
    // Test with higher level (difficulty multiplier)
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(1, 10), 200); // Flipper, level 10 (2x multiplier)
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(5, 15), 900); // Fuzzball, level 15 (3x multiplier)
    
    // Test level completion bonus
    EXPECT_EQ(scoreManager_.calculateLevelCompletionBonus(1, 60.0f), 2000); // Base 1000 + level 500 + time bonus 500
    EXPECT_EQ(scoreManager_.calculateLevelCompletionBonus(5, 30.0f), 4250); // Base 1000 + level 2500 + time bonus 750
    
    // Test survival bonus
    EXPECT_EQ(scoreManager_.calculateSurvivalBonus(1, 60.0f), 1200); // Base 500 + level 100 + time 600
    EXPECT_EQ(scoreManager_.calculateSurvivalBonus(10, 120.0f), 2700); // Base 500 + level 1000 + time 1200
}

TEST_F(TestScoreManager, HighScoreManagement) {
    // Test empty high score list
    EXPECT_TRUE(scoreManager_.isHighScore(100));
    EXPECT_EQ(scoreManager_.getHighScoreRank(100), 1);
    
    // Add some high scores
    scoreManager_.addHighScore("PLAYER1", 1000, 5);
    scoreManager_.addHighScore("PLAYER2", 2000, 8);
    scoreManager_.addHighScore("PLAYER3", 1500, 6);
    
    // Test high score checking
    EXPECT_TRUE(scoreManager_.isHighScore(2500));
    EXPECT_FALSE(scoreManager_.isHighScore(500));
    
    // Test ranking
    EXPECT_EQ(scoreManager_.getHighScoreRank(2500), 1);
    EXPECT_EQ(scoreManager_.getHighScoreRank(2000), 1);
    EXPECT_EQ(scoreManager_.getHighScoreRank(1500), 2);
    EXPECT_EQ(scoreManager_.getHighScoreRank(1000), 3);
    EXPECT_EQ(scoreManager_.getHighScoreRank(500), 4);
    
    // Test high score list
    const auto& highScores = scoreManager_.getHighScores();
    EXPECT_EQ(highScores.size(), 3);
    EXPECT_EQ(highScores[0].playerName, "PLAYER2");
    EXPECT_EQ(highScores[0].score, 2000);
    EXPECT_EQ(highScores[1].playerName, "PLAYER3");
    EXPECT_EQ(highScores[1].score, 1500);
    EXPECT_EQ(highScores[2].playerName, "PLAYER1");
    EXPECT_EQ(highScores[2].score, 1000);
}

TEST_F(TestScoreManager, HighScorePersistence) {
    // Add some high scores
    scoreManager_.addHighScore("TEST1", 1000, 3);
    scoreManager_.addHighScore("TEST2", 2000, 5);
    
    // Create new score manager to test loading
    ScoreManager newScoreManager;
    newScoreManager.initialize();
    
    // Check that high scores were loaded
    const auto& highScores = newScoreManager.getHighScores();
    EXPECT_EQ(highScores.size(), 2);
    EXPECT_EQ(highScores[0].playerName, "TEST2");
    EXPECT_EQ(highScores[0].score, 2000);
    EXPECT_EQ(highScores[1].playerName, "TEST1");
    EXPECT_EQ(highScores[1].score, 1000);
}

TEST_F(TestScoreManager, HighScoreLimits) {
    // Add more than MAX_HIGH_SCORES entries
    for (int i = 0; i < 15; ++i) {
        scoreManager_.addHighScore("PLAYER" + std::to_string(i), 1000 + i * 100, i + 1);
    }
    
    // Check that only MAX_HIGH_SCORES entries are kept
    const auto& highScores = scoreManager_.getHighScores();
    EXPECT_EQ(highScores.size(), 10); // MAX_HIGH_SCORES
    
    // Check that highest scores are kept
    EXPECT_EQ(highScores[0].score, 2400); // Highest score
    EXPECT_EQ(highScores[9].score, 1500); // Lowest score kept
}

TEST_F(TestScoreManager, EventHandling) {
    // Test enemy killed event
    struct TestEvent : public Event {
        const char* getTypeName() const override { return "TestEvent"; }
    };
    scoreManager_.handleEnemyKilled(TestEvent{});
    EXPECT_EQ(scoreManager_.getCurrentPlayerScore(), 100); // Default enemy kill score
    
    // Test level completed event
    scoreManager_.handleLevelCompleted(TestEvent{});
    EXPECT_EQ(scoreManager_.getCurrentPlayerScore(), 1600); // Previous 100 + level completion bonus
    
    // Test player death event
    scoreManager_.handlePlayerDeath(TestEvent{});
    EXPECT_EQ(scoreManager_.getGamesPlayed(), 1);
}

TEST_F(TestScoreManager, ScoreStatistics) {
    scoreManager_.addScore(1000, 0);
    scoreManager_.addScore(2000, 1);
    
    EXPECT_EQ(scoreManager_.getTotalScore(), 3000);
    EXPECT_EQ(scoreManager_.getBestScore(), 2000);
}

TEST_F(TestScoreManager, InvalidScoreHandling) {
    // Test invalid player ID
    scoreManager_.addScore(100, 2); // Invalid player ID
    EXPECT_EQ(scoreManager_.getScore(0), 0);
    EXPECT_EQ(scoreManager_.getScore(1), 0);
    
    scoreManager_.setScore(500, 3); // Invalid player ID
    EXPECT_EQ(scoreManager_.getScore(0), 0);
    EXPECT_EQ(scoreManager_.getScore(1), 0);
    
    // Test invalid score for high score
    scoreManager_.addHighScore("TEST", 0, 1); // Invalid score
    EXPECT_EQ(scoreManager_.getHighScores().size(), 0);
    
    scoreManager_.addHighScore("TEST", 999999999, 1); // Invalid score (too high)
    EXPECT_EQ(scoreManager_.getHighScores().size(), 0);
}

TEST_F(TestScoreManager, EventDispatching) {
    // Test score change event
    scoreManager_.addScore(100, 0);
    // If no exception is thrown, the test passes
    SUCCEED();
}

TEST_F(TestScoreManager, HighScoreAchievement) {
    // Add a high score that should trigger achievement event
    scoreManager_.addHighScore("CHAMPION", 5000, 10);
    // If no exception is thrown, the test passes
    SUCCEED();
}

TEST_F(TestScoreManager, ResetHighScores) {
    // Add some high scores
    scoreManager_.addHighScore("TEST1", 1000, 3);
    scoreManager_.addHighScore("TEST2", 2000, 5);
    
    EXPECT_EQ(scoreManager_.getHighScores().size(), 2);
    
    // Reset high scores
    scoreManager_.resetHighScores();
    
    EXPECT_EQ(scoreManager_.getHighScores().size(), 0);
    
    // Test that reset persists
    ScoreManager newScoreManager;
    newScoreManager.initialize();
    EXPECT_EQ(newScoreManager.getHighScores().size(), 0);
}

TEST_F(TestScoreManager, ScoreCalculationEdgeCases) {
    // Test edge cases for score calculations
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(0, 1), 100); // Invalid enemy type
    EXPECT_EQ(scoreManager_.calculateEnemyKillScore(6, 1), 100); // Invalid enemy type
    
    // Test time bonus edge cases
    EXPECT_EQ(scoreManager_.calculateLevelCompletionBonus(1, 0.0f), 2500); // No time bonus: base 1500 + 1000
    EXPECT_EQ(scoreManager_.calculateLevelCompletionBonus(1, 120.0f), 1500); // Maximum time penalty: base 1500 + 0
    
    // Test survival bonus edge cases
    EXPECT_EQ(scoreManager_.calculateSurvivalBonus(1, 0.0f), 600); // No time bonus
    EXPECT_EQ(scoreManager_.calculateSurvivalBonus(99, 999.0f), 500 + 9900 + 9990); // High values
}

// Note: These tests are removed because they test private methods
// The functionality is tested indirectly through the public methods 