#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <spdlog/spdlog.h>

namespace Tempest {

/**
 * @brief Base Event class
 * 
 * All game events inherit from this base class. Events are used for
 * decoupled communication between systems.
 */
class Event {
public:
    virtual ~Event() = default;
    virtual const char* getTypeName() const = 0;
};

/**
 * @brief Event Handler function type
 */
using EventHandler = std::function<void(const Event&)>;

/**
 * @brief Event Queue for storing pending events
 */
class EventQueue {
public:
    void push(std::unique_ptr<Event> event);
    std::vector<std::unique_ptr<Event>> flush();
    size_t size() const { return events_.size(); }
    void clear() { events_.clear(); }

private:
    std::vector<std::unique_ptr<Event>> events_;
};

/**
 * @brief Event Dispatcher for routing events to handlers
 * 
 * Manages event subscriptions and dispatches events to appropriate handlers.
 * Based on the original game's event-driven architecture but with modern
 * C++ features for type safety and performance.
 */
class EventDispatcher {
public:
    EventDispatcher();
    ~EventDispatcher() = default;

    // Event subscription
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> handler);
    
    template<typename EventType>
    void unsubscribe();
    
    // Event dispatching
    void dispatch(const Event& event);
    void dispatchImmediate(const Event& event);
    
    // Queue management
    void processQueue();
    void clearQueue();
    
    // Statistics
    size_t getHandlerCount() const;
    size_t getQueueSize() const { return queue_.size(); }

private:
    std::unordered_map<std::type_index, std::vector<EventHandler>> handlers_;
    EventQueue queue_;
};

/**
 * @brief Event System singleton
 * 
 * Provides global access to the event system for the entire game.
 */
class EventSystem {
public:
    static EventSystem& getInstance();
    
    // Event dispatching
    template<typename EventType>
    void dispatch(const EventType& event);
    
    template<typename EventType>
    void dispatchImmediate(const EventType& event);
    
    // Event subscription
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> handler);
    
    template<typename EventType>
    void unsubscribe();
    
    // Queue management
    void processQueue();
    void clearQueue();
    
    // Statistics
    size_t getHandlerCount() const { return dispatcher_.getHandlerCount(); }
    size_t getQueueSize() const { return dispatcher_.getQueueSize(); }

private:
    EventSystem() = default;
    ~EventSystem() = default;
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;
    
    EventDispatcher dispatcher_;
};

// Template implementations
template<typename EventType>
void EventDispatcher::subscribe(std::function<void(const EventType&)> handler) {
    auto typeIndex = std::type_index(typeid(EventType));
    handlers_[typeIndex].push_back([handler](const Event& event) {
        handler(static_cast<const EventType&>(event));
    });
    
    spdlog::debug("Subscribed to event type: {}", typeid(EventType).name());
}

template<typename EventType>
void EventDispatcher::unsubscribe() {
    auto typeIndex = std::type_index(typeid(EventType));
    handlers_.erase(typeIndex);
    
    spdlog::debug("Unsubscribed from event type: {}", typeid(EventType).name());
}

template<typename EventType>
void EventSystem::dispatch(const EventType& event) {
    dispatcher_.dispatch(event);
}

template<typename EventType>
void EventSystem::dispatchImmediate(const EventType& event) {
    dispatcher_.dispatchImmediate(event);
}

template<typename EventType>
void EventSystem::subscribe(std::function<void(const EventType&)> handler) {
    dispatcher_.subscribe<EventType>(handler);
}

template<typename EventType>
void EventSystem::unsubscribe() {
    dispatcher_.unsubscribe<EventType>();
}

// Common event types for the game
struct GameStateChangedEvent : public Event {
    enum class State {
        Startup = 0x00,
        LevelStart = 0x02,
        Playing = 0x04,
        PlayerDeath = 0x06,
        LevelBegin = 0x08,
        HighScore = 0x0a,
        Unused = 0x0c,
        LevelEnd = 0x0e,
        EnterInitials = 0x12,
        LevelSelect = 0x16,
        ZoomingLevel = 0x18,
        Unknown1 = 0x1a,
        Unknown2 = 0x1c,
        Unknown3 = 0x1e,
        Descending = 0x20,
        ServiceMode = 0x22,
        Explosion = 0x24
    };
    
    State oldState;
    State newState;
    
    GameStateChangedEvent(State oldState, State newState) 
        : oldState(oldState), newState(newState) {}
    
    const char* getTypeName() const override { return "GameStateChangedEvent"; }
};

struct PlayerInputEvent : public Event {
    enum class InputType {
        Zap,
        Fire,
        MoveLeft,
        MoveRight,
        SuperZapper
    };
    
    InputType inputType;
    bool pressed;
    
    PlayerInputEvent(InputType inputType, bool pressed) 
        : inputType(inputType), pressed(pressed) {}
    
    const char* getTypeName() const override { return "PlayerInputEvent"; }
};

struct EnemySpawnEvent : public Event {
    enum class EnemyType {
        Flipper = 1,
        Pulsar = 2,
        Tanker = 3,
        Spiker = 4,
        Fuzzball = 5
    };
    
    EnemyType enemyType;
    uint8_t segment;
    float along; // Position along tube depth
    
    EnemySpawnEvent(EnemyType enemyType, uint8_t segment, float along)
        : enemyType(enemyType), segment(segment), along(along) {}
    
    const char* getTypeName() const override { return "EnemySpawnEvent"; }
};

struct PlayerDeathEvent : public Event {
    uint8_t livesRemaining;
    
    explicit PlayerDeathEvent(uint8_t livesRemaining) : livesRemaining(livesRemaining) {}
    
    const char* getTypeName() const override { return "PlayerDeathEvent"; }
};

struct LevelCompleteEvent : public Event {
    uint8_t levelNumber;
    uint32_t score;
    
    LevelCompleteEvent(uint8_t levelNumber, uint32_t score)
        : levelNumber(levelNumber), score(score) {}
    
    const char* getTypeName() const override { return "LevelCompleteEvent"; }
};

} // namespace Tempest 