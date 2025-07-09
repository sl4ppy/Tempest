#include "EntityManager.h"
#include <algorithm>
#include <functional>

namespace Tempest {

// Entity implementation
Entity::Entity(EntityID id) : id_(id) {
    spdlog::debug("Created entity with ID: {}", id);
}

// EntityManager implementation
EntityManager::EntityManager() : nextEntityId_(0) {
    spdlog::info("EntityManager initialized");
}

EntityID EntityManager::createEntity() {
    EntityID id = nextEntityId_++;
    auto entity = std::make_unique<Entity>(id);
    entities_[id] = std::move(entity);
    activeEntities_.push_back(id);
    
    spdlog::debug("Created entity with ID: {}", id);
    return id;
}

void EntityManager::destroyEntity(EntityID entityId) {
    auto it = entities_.find(entityId);
    if (it != entities_.end()) {
        // Remove from active entities list
        auto activeIt = std::find(activeEntities_.begin(), activeEntities_.end(), entityId);
        if (activeIt != activeEntities_.end()) {
            activeEntities_.erase(activeIt);
        }
        
        // Remove from entities map
        entities_.erase(it);
        
        spdlog::debug("Destroyed entity with ID: {}", entityId);
    } else {
        spdlog::warn("Attempted to destroy non-existent entity with ID: {}", entityId);
    }
}

Entity* EntityManager::getEntity(EntityID entityId) {
    auto it = entities_.find(entityId);
    return it != entities_.end() ? it->second.get() : nullptr;
}

} // namespace Tempest 