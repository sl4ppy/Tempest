#pragma once

#include <cstdint>
#include <vector>
#include <bitset>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <spdlog/spdlog.h>
#include "Component.h"

namespace Tempest {

// Forward declarations
class Entity;

using EntityID = uint32_t;
using ComponentTypeID = uint32_t;

/**
 * @brief Entity class representing a game object
 * 
 * Based on the original game's entity system, entities are lightweight
 * containers that can hold multiple components. Each entity has a unique ID
 * and a component mask to track which components it has.
 */
class Entity {
public:
    explicit Entity(EntityID id);
    ~Entity() = default;

    EntityID getId() const { return id_; }
    const std::bitset<64>& getComponentMask() const { return componentMask_; }
    
    template<typename T>
    bool hasComponent() const;
    
    template<typename T>
    void addComponent(std::unique_ptr<T> component);
    
    template<typename T>
    T* getComponent();
    
    template<typename T>
    void removeComponent();

private:
    EntityID id_;
    std::bitset<64> componentMask_;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components_;
};

/**
 * @brief Entity Manager for the ECS system
 * 
 * Manages the lifecycle of entities and provides efficient querying and
 * iteration capabilities. Based on the original game's entity management
 * but with modern C++ optimizations.
 */
class EntityManager {
public:
    EntityManager();
    ~EntityManager() = default;

    // Entity lifecycle
    EntityID createEntity();
    void destroyEntity(EntityID entityId);
    Entity* getEntity(EntityID entityId);
    
    // Component management
    template<typename T>
    void addComponent(EntityID entityId, std::unique_ptr<T> component);
    
    template<typename T>
    T* getComponent(EntityID entityId);
    
    template<typename T>
    void removeComponent(EntityID entityId);
    
    template<typename T>
    bool hasComponent(EntityID entityId) const;
    
    // Querying
    template<typename... Components>
    std::vector<Entity*> getEntitiesWith();
    
    // Iteration
    template<typename... Components>
    void forEach(std::function<void(Entity*, Components*...)> func);
    
    // Statistics
    size_t getEntityCount() const { return entities_.size(); }
    size_t getActiveEntityCount() const { return activeEntities_.size(); }

private:
    std::unordered_map<EntityID, std::unique_ptr<Entity>> entities_;
    std::vector<EntityID> activeEntities_;
    EntityID nextEntityId_;
    
    // Component storage by type
    std::unordered_map<ComponentTypeID, std::vector<Component*>> componentStorage_;
    
};

// Template implementations
template<typename T>
bool Entity::hasComponent() const {
    return componentMask_.test(getComponentTypeId<T>());
}

template<typename T>
void Entity::addComponent(std::unique_ptr<T> component) {
    auto typeId = getComponentTypeId<T>();
    componentMask_.set(typeId);
    components_[typeid(T)] = std::move(component);
}

template<typename T>
T* Entity::getComponent() {
    auto it = components_.find(typeid(T));
    return it != components_.end() ? static_cast<T*>(it->second.get()) : nullptr;
}

template<typename T>
void Entity::removeComponent() {
    auto typeId = getComponentTypeId<T>();
    componentMask_.reset(typeId);
    components_.erase(typeid(T));
}

template<typename T>
void EntityManager::addComponent(EntityID entityId, std::unique_ptr<T> component) {
    auto entity = getEntity(entityId);
    if (entity) {
        entity->addComponent<T>(std::move(component));
        
        // Store in component storage for efficient iteration
        auto typeId = getComponentTypeId<T>();
        componentStorage_[typeId].push_back(component.get());
    }
}

template<typename T>
T* EntityManager::getComponent(EntityID entityId) {
    auto entity = getEntity(entityId);
    return entity ? entity->getComponent<T>() : nullptr;
}

template<typename T>
void EntityManager::removeComponent(EntityID entityId) {
    auto entity = getEntity(entityId);
    if (entity) {
        entity->removeComponent<T>();
        
        // Remove from component storage
        auto typeId = getComponentTypeId<T>();
        auto& storage = componentStorage_[typeId];
        storage.erase(
            std::remove_if(storage.begin(), storage.end(),
                [entityId](Component* comp) {
                    // This is a simplified version - in practice you'd need
                    // to track which entity owns each component
                    return true; // Simplified for now
                }),
            storage.end()
        );
    }
}

template<typename T>
bool EntityManager::hasComponent(EntityID entityId) const {
    auto entity = getEntity(entityId);
    return entity ? entity->hasComponent<T>() : false;
}

} // namespace Tempest 