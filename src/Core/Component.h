#pragma once

#include <cstdint>
#include <typeindex>
#include <unordered_map>

namespace Tempest {

using ComponentTypeID = uint32_t;

// Free function to get a unique component type ID for each component type
inline std::unordered_map<std::type_index, ComponentTypeID>& componentTypeRegistry() {
    static std::unordered_map<std::type_index, ComponentTypeID> registry;
    return registry;
}
inline ComponentTypeID& nextComponentTypeId() {
    static ComponentTypeID nextId = 0;
    return nextId;
}
template<typename T>
ComponentTypeID getComponentTypeId() {
    auto& registry = componentTypeRegistry();
    auto typeIndex = std::type_index(typeid(T));
    auto it = registry.find(typeIndex);
    if (it == registry.end()) {
        registry[typeIndex] = nextComponentTypeId()++;
    }
    return registry[typeIndex];
}

class Component {
public:
    virtual ~Component() = default;
    virtual ComponentTypeID getTypeId() const = 0;
    virtual const char* getTypeName() const = 0;
};

} // namespace Tempest 