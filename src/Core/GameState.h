#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <spdlog/spdlog.h>
#include "EventSystem.h"

namespace Tempest {

/**
 * @brief Game State Manager based on original game's RAM 0000 analysis
 * 
 * Manages the game's state transitions and state-specific behavior.
 * Based on the original game's gamestate variable and state machine.
 */
class GameStateManager {
public:
    // Game states based on original RAM 0000 analysis
    enum class State {
        Startup = 0x00,           // Entered briefly at game startup
        LevelStart = 0x02,        // Entered briefly at beginning of first level
        Playing = 0x04,           // Playing (including attract mode)
        PlayerDeath = 0x06,       // Entered briefly on player death
        LevelBegin = 0x08,        // Set briefly at beginning of each level
        HighScore = 0x0a,         // High-score, logo screen, AVOID SPIKES
        Unused = 0x0c,            // Unused (jump table holds 0x0000)
        LevelEnd = 0x0e,          // Entered briefly when zooming off end of level
        EnterInitials = 0x12,     // Entering initials
        LevelSelect = 0x16,       // Starting level selection screen
        ZoomingLevel = 0x18,      // Zooming new level in
        Unknown1 = 0x1a,          // Unknown state
        Unknown2 = 0x1c,          // Unknown state
        Unknown3 = 0x1e,          // Unknown state
        Descending = 0x20,        // Descending down tube at level end
        ServiceMode = 0x22,       // Non-selftest service mode display
        Explosion = 0x24          // High-score explosion
    };

    GameStateManager();
    ~GameStateManager() = default;

    // State management
    void setState(State newState);
    State getCurrentState() const { return currentState_; }
    State getPreviousState() const { return previousState_; }
    
    // State queries
    bool isPlaying() const { return currentState_ == State::Playing; }
    bool isInMenu() const;
    bool isGameOver() const;
    bool isAttractMode() const;
    
    // State transitions
    void transitionToPlaying();
    void transitionToMenu();
    void transitionToGameOver();
    void transitionToLevelSelect();
    void transitionToHighScore();
    
    // State-specific behavior
    void update(float deltaTime);
    void onStateEnter(State state);
    void onStateExit(State state);
    
    // Event handling
    void handleGameStateChanged(const GameStateChangedEvent& event);

private:
    State currentState_;
    State previousState_;
    float stateTimer_;
    
    // State-specific data
    struct StateData {
        float enterTime;
        bool initialized;
        std::string description;
    };
    
    std::unordered_map<State, StateData> stateData_;
    
    void initializeStateData();
    std::string getStateName(State state) const;
};

/**
 * @brief Game State based on original memory layout (RAM 0000-009F)
 * 
 * Represents the core game state variables as identified in the ASM analysis.
 */
struct GameState {
    // Core game state (RAM 0000-009F)
    uint8_t gamestate;        // RAM 0000: Main game state
    uint8_t timectr;          // RAM 0004: Time-based counter
    uint8_t player_control;   // RAM 0005: Player input mode (0x00=attract, 0xC0=gameplay)
    uint8_t credits;          // RAM 0006: Number of game credits (max 0x28)
    uint8_t zap_fire_shadow;  // RAM 0008: Shadow register for controls
    
    // Player data (RAM 0040-0049)
    struct {
        uint8_t score_l;      // RAM 0040: Player 1 score (lowest 2 digits)
        uint8_t score_m;      // RAM 0041: Player 1 score (middle 2 digits)
        uint8_t score_h;      // RAM 0042: Player 1 score (highest 2 digits)
        uint8_t level;        // RAM 0046: Level for Player 1
        uint8_t lives;        // RAM 0048: Lives for Player 1
    } player1;
    
    struct {
        uint8_t score_l;      // RAM 0043: Player 2 score (lowest 2 digits)
        uint8_t score_m;      // RAM 0044: Player 2 score (middle 2 digits)
        uint8_t score_h;      // RAM 0045: Player 2 score (highest 2 digits)
        uint8_t level;        // RAM 0047: Level for Player 2
        uint8_t lives;        // RAM 0049: Lives for Player 2
    } player2;
    
    uint8_t curplayer;        // RAM 003D: Current player (0=player 1, 1=player 2)
    uint8_t curlevel;         // RAM 009F: Current level - 1
    
    // Constructor with default values
    GameState() : gamestate(0), timectr(0), player_control(0), credits(0), 
                  zap_fire_shadow(0), curplayer(0), curlevel(0) {
        // Initialize player data
        player1 = {0, 0, 0, 0, 3}; // Default 3 lives
        player2 = {0, 0, 0, 0, 3}; // Default 3 lives
    }
    
    // Score management
    uint32_t getPlayer1Score() const {
        return (static_cast<uint32_t>(player1.score_h) << 16) |
               (static_cast<uint32_t>(player1.score_m) << 8) |
               static_cast<uint32_t>(player1.score_l);
    }
    
    void setPlayer1Score(uint32_t score) {
        player1.score_l = score & 0xFF;
        player1.score_m = (score >> 8) & 0xFF;
        player1.score_h = (score >> 16) & 0xFF;
    }
    
    uint32_t getPlayer2Score() const {
        return (static_cast<uint32_t>(player2.score_h) << 16) |
               (static_cast<uint32_t>(player2.score_m) << 8) |
               static_cast<uint32_t>(player2.score_l);
    }
    
    void setPlayer2Score(uint32_t score) {
        player2.score_l = score & 0xFF;
        player2.score_m = (score >> 8) & 0xFF;
        player2.score_h = (score >> 16) & 0xFF;
    }
    
    // Current player score
    uint32_t getCurrentPlayerScore() const {
        return curplayer == 0 ? getPlayer1Score() : getPlayer2Score();
    }
    
    void setCurrentPlayerScore(uint32_t score) {
        if (curplayer == 0) {
            setPlayer1Score(score);
        } else {
            setPlayer2Score(score);
        }
    }
    
    // Current player lives
    uint8_t getCurrentPlayerLives() const {
        return curplayer == 0 ? player1.lives : player2.lives;
    }
    
    void setCurrentPlayerLives(uint8_t lives) {
        if (curplayer == 0) {
            player1.lives = lives;
        } else {
            player2.lives = lives;
        }
    }
    
    // Current player level
    uint8_t getCurrentPlayerLevel() const {
        return curplayer == 0 ? player1.level : player2.level;
    }
    
    void setCurrentPlayerLevel(uint8_t level) {
        if (curplayer == 0) {
            player1.level = level;
        } else {
            player2.level = level;
        }
    }
};

} // namespace Tempest 