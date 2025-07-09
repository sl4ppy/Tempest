#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "Core/GameEngine.h"
#include "Core/EntityManager.h"
#include "Core/EventSystem.h"
#include "Core/GameState.h"

using namespace Tempest;

class GameEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        spdlog::set_level(spdlog::level::debug);
    }
    
    void TearDown() override {
        // Clean up any global state
    }
};

TEST_F(GameEngineTest, EngineInitialization) {
    auto& engine = GameEngineSingleton::getInstance();
    
    EXPECT_TRUE(engine.initialize());
    EXPECT_EQ(engine.getEntityCount(), 0);
    EXPECT_EQ(engine.getFPS(), 0.0f);
    EXPECT_EQ(engine.getFrameTime(), 0.0f);
    
    engine.shutdown();
}

TEST_F(GameEngineTest, EntityManagerBasicOperations) {
    auto& engine = GameEngineSingleton::getInstance();
    engine.initialize();
    
    auto& entityManager = engine.getEntityManager();
    
    // Test entity creation
    auto entityId1 = entityManager.createEntity();
    EXPECT_EQ(entityId1, 0);
    EXPECT_EQ(entityManager.getEntityCount(), 1);
    EXPECT_EQ(entityManager.getActiveEntityCount(), 1);
    
    auto entityId2 = entityManager.createEntity();
    EXPECT_EQ(entityId2, 1);
    EXPECT_EQ(entityManager.getEntityCount(), 2);
    EXPECT_EQ(entityManager.getActiveEntityCount(), 2);
    
    // Test entity retrieval
    auto entity1 = entityManager.getEntity(entityId1);
    EXPECT_NE(entity1, nullptr);
    EXPECT_EQ(entity1->getId(), entityId1);
    
    auto entity2 = entityManager.getEntity(entityId2);
    EXPECT_NE(entity2, nullptr);
    EXPECT_EQ(entity2->getId(), entityId2);
    
    // Test entity destruction
    entityManager.destroyEntity(entityId1);
    EXPECT_EQ(entityManager.getEntityCount(), 1);
    EXPECT_EQ(entityManager.getActiveEntityCount(), 1);
    
    auto destroyedEntity = entityManager.getEntity(entityId1);
    EXPECT_EQ(destroyedEntity, nullptr);
    
    engine.shutdown();
}

TEST_F(GameEngineTest, GameStateManagerBasicOperations) {
    auto& engine = GameEngineSingleton::getInstance();
    engine.initialize();
    
    auto& gameStateManager = engine.getGameStateManager();
    
    // Test initial state
    EXPECT_EQ(gameStateManager.getCurrentState(), GameStateManager::State::Startup);
    
    // Test state transitions
    gameStateManager.transitionToPlaying();
    EXPECT_EQ(gameStateManager.getCurrentState(), GameStateManager::State::Playing);
    EXPECT_TRUE(gameStateManager.isPlaying());
    
    gameStateManager.transitionToMenu();
    EXPECT_EQ(gameStateManager.getCurrentState(), GameStateManager::State::LevelSelect);
    EXPECT_TRUE(gameStateManager.isInMenu());
    
    gameStateManager.transitionToGameOver();
    EXPECT_EQ(gameStateManager.getCurrentState(), GameStateManager::State::PlayerDeath);
    EXPECT_TRUE(gameStateManager.isGameOver());
    
    engine.shutdown();
}

TEST_F(GameEngineTest, EventSystemBasicOperations) {
    auto& engine = GameEngineSingleton::getInstance();
    engine.initialize();
    
    auto& eventSystem = engine.getEventSystem();
    
    // Test event subscription and dispatch
    bool eventReceived = false;
    GameStateChangedEvent::State receivedOldState = GameStateChangedEvent::State::Startup;
    GameStateChangedEvent::State receivedNewState = GameStateChangedEvent::State::Startup;
    
    eventSystem.subscribe<GameStateChangedEvent>(
        [&](const GameStateChangedEvent& event) {
            eventReceived = true;
            receivedOldState = event.oldState;
            receivedNewState = event.newState;
        }
    );
    
    // Dispatch an event
    GameStateChangedEvent testEvent(GameStateChangedEvent::State::Startup, 
                                   GameStateChangedEvent::State::Playing);
    eventSystem.dispatch(testEvent);
    
    // Process the event queue
    eventSystem.processQueue();
    
    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(receivedOldState, GameStateChangedEvent::State::Startup);
    EXPECT_EQ(receivedNewState, GameStateChangedEvent::State::Playing);
    
    engine.shutdown();
}

TEST_F(GameEngineTest, GameStateDataOperations) {
    auto& engine = GameEngineSingleton::getInstance();
    engine.initialize();
    
    auto& gameState = engine.getGameState();
    
    // Test score management
    gameState.setPlayer1Score(12345);
    EXPECT_EQ(gameState.getPlayer1Score(), 12345);
    
    gameState.setPlayer2Score(67890);
    EXPECT_EQ(gameState.getPlayer2Score(), 67890);
    
    // Test current player operations
    gameState.curplayer = 0;
    EXPECT_EQ(gameState.getCurrentPlayerScore(), 12345);
    EXPECT_EQ(gameState.getCurrentPlayerLives(), 3);
    
    gameState.setCurrentPlayerLives(5);
    EXPECT_EQ(gameState.getCurrentPlayerLives(), 5);
    
    gameState.curplayer = 1;
    EXPECT_EQ(gameState.getCurrentPlayerScore(), 67890);
    EXPECT_EQ(gameState.getCurrentPlayerLives(), 3);
    
    gameState.setCurrentPlayerScore(99999);
    EXPECT_EQ(gameState.getCurrentPlayerScore(), 99999);
    
    engine.shutdown();
}

TEST_F(GameEngineTest, EngineConfiguration) {
    auto& engine = GameEngineSingleton::getInstance();
    engine.initialize();
    
    // Test engine configuration
    engine.setTargetFPS(30.0f);
    engine.setMaxFrameTime(0.05f);
    
    // These would be tested during actual game loop execution
    // For now, we just verify the configuration methods don't crash
    
    engine.shutdown();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 