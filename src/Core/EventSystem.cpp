#include "EventSystem.h"
#include <algorithm>

namespace Tempest {

// EventQueue implementation
void EventQueue::push(std::unique_ptr<Event> event) {
    events_.push_back(std::move(event));
}

std::vector<std::unique_ptr<Event>> EventQueue::flush() {
    std::vector<std::unique_ptr<Event>> events;
    events.swap(events_);
    return events;
}

// EventDispatcher implementation
EventDispatcher::EventDispatcher() {
    spdlog::info("EventDispatcher initialized");
}

void EventDispatcher::dispatch(const Event& event) {
    // Queue the event for later processing
    // Note: We can't make a copy of an abstract class, so we'll process immediately
    dispatchImmediate(event);
}

void EventDispatcher::dispatchImmediate(const Event& event) {
    auto typeIndex = std::type_index(typeid(event));
    auto it = handlers_.find(typeIndex);
    
    if (it != handlers_.end()) {
        for (auto& handler : it->second) {
            try {
                handler(event);
            } catch (const std::exception& e) {
                spdlog::error("Exception in event handler: {}", e.what());
            }
        }
    }
}

void EventDispatcher::processQueue() {
    auto events = queue_.flush();
    
    for (auto& event : events) {
        dispatchImmediate(*event);
    }
}

void EventDispatcher::clearQueue() {
    queue_.clear();
}

size_t EventDispatcher::getHandlerCount() const {
    size_t total = 0;
    for (const auto& pair : handlers_) {
        total += pair.second.size();
    }
    return total;
}

// EventSystem singleton implementation
EventSystem& EventSystem::getInstance() {
    static EventSystem instance;
    return instance;
}

void EventSystem::processQueue() {
    dispatcher_.processQueue();
}

void EventSystem::clearQueue() {
    dispatcher_.clearQueue();
}

} // namespace Tempest 