#include "GameState.h"
#include <unordered_map>

namespace Tempest {

GameStateManager::GameStateManager() 
    : currentState_(State::Startup), previousState_(State::Startup), stateTimer_(0.0f) {
    
    initializeStateData();
    spdlog::info("GameStateManager initialized");
}

void GameStateManager::setState(State newState) {
    if (newState != currentState_) {
        previousState_ = currentState_;
        onStateExit(currentState_);
        
        currentState_ = newState;
        stateTimer_ = 0.0f;
        
        onStateEnter(currentState_);
        
        // Dispatch state change event
        GameStateChangedEvent event(static_cast<GameStateChangedEvent::State>(previousState_),
                                   static_cast<GameStateChangedEvent::State>(currentState_));
        EventSystem::getInstance().dispatch(event);
        
        spdlog::info("Game state changed from {} to {}", 
                     getStateName(previousState_), getStateName(currentState_));
    }
}

bool GameStateManager::isInMenu() const {
    return currentState_ == State::LevelSelect || 
           currentState_ == State::HighScore ||
           currentState_ == State::EnterInitials;
}

bool GameStateManager::isGameOver() const {
    return currentState_ == State::PlayerDeath;
}

bool GameStateManager::isAttractMode() const {
    auto it = stateData_.find(State::Playing);
    return currentState_ == State::Playing && it != stateData_.end() && it->second.description == "Attract Mode";
}

void GameStateManager::transitionToPlaying() {
    setState(State::Playing);
}

void GameStateManager::transitionToMenu() {
    setState(State::LevelSelect);
}

void GameStateManager::transitionToGameOver() {
    setState(State::PlayerDeath);
}

void GameStateManager::transitionToLevelSelect() {
    setState(State::LevelSelect);
}

void GameStateManager::transitionToHighScore() {
    setState(State::HighScore);
}

void GameStateManager::update(float deltaTime) {
    stateTimer_ += deltaTime;
    
    // State-specific update logic
    switch (currentState_) {
        case State::Startup:
            // Brief startup state, transition to level select
            if (stateTimer_ > 1.0f) {
                transitionToLevelSelect();
            }
            break;
            
        case State::Playing:
            // Normal gameplay - check for game over conditions
            break;
            
        case State::PlayerDeath:
            // Brief death state, transition to appropriate next state
            if (stateTimer_ > 2.0f) {
                // Check if player has lives remaining
                // For now, transition to level select
                transitionToLevelSelect();
            }
            break;
            
        case State::LevelEnd:
            // Brief level end state
            if (stateTimer_ > 1.0f) {
                transitionToPlaying();
            }
            break;
            
        default:
            // Other states don't need special update logic
            break;
    }
}

void GameStateManager::onStateEnter(State state) {
    auto& data = stateData_[state];
    data.enterTime = stateTimer_;
    data.initialized = true;
    
    spdlog::debug("Entered state: {}", getStateName(state));
}

void GameStateManager::onStateExit(State state) {
    spdlog::debug("Exited state: {}", getStateName(state));
}

void GameStateManager::handleGameStateChanged(const GameStateChangedEvent& event) {
    spdlog::debug("Game state changed event: {} -> {}", 
                  static_cast<int>(event.oldState), static_cast<int>(event.newState));
}

void GameStateManager::initializeStateData() {
    // Initialize state descriptions based on ASM analysis
    stateData_[State::Startup] = {0.0f, false, "Startup"};
    stateData_[State::LevelStart] = {0.0f, false, "Level Start"};
    stateData_[State::Playing] = {0.0f, false, "Playing"};
    stateData_[State::PlayerDeath] = {0.0f, false, "Player Death"};
    stateData_[State::LevelBegin] = {0.0f, false, "Level Begin"};
    stateData_[State::HighScore] = {0.0f, false, "High Score"};
    stateData_[State::Unused] = {0.0f, false, "Unused"};
    stateData_[State::LevelEnd] = {0.0f, false, "Level End"};
    stateData_[State::EnterInitials] = {0.0f, false, "Enter Initials"};
    stateData_[State::LevelSelect] = {0.0f, false, "Level Select"};
    stateData_[State::ZoomingLevel] = {0.0f, false, "Zooming Level"};
    stateData_[State::Unknown1] = {0.0f, false, "Unknown 1"};
    stateData_[State::Unknown2] = {0.0f, false, "Unknown 2"};
    stateData_[State::Unknown3] = {0.0f, false, "Unknown 3"};
    stateData_[State::Descending] = {0.0f, false, "Descending"};
    stateData_[State::ServiceMode] = {0.0f, false, "Service Mode"};
    stateData_[State::Explosion] = {0.0f, false, "Explosion"};
}

std::string GameStateManager::getStateName(State state) const {
    auto it = stateData_.find(state);
    return it != stateData_.end() ? it->second.description : "Unknown";
}

} // namespace Tempest 