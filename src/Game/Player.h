#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include "../Core/Component.h"
#include "../Core/EventSystem.h"

namespace Tempest {

/**
 * @brief Player component based on original game's RAM 0200-0202 analysis
 * 
 * Represents the player ship (claw) with position, movement, and weapon systems.
 * Based on the original game's player entity structure.
 */
class PlayerComponent : public Component {
public:
    // Player position on tube (0-15 segments)
    uint8_t segment;           // Current segment position
    uint8_t targetSegment;     // Target segment for movement
    float segmentLerp;         // Interpolation for smooth movement
    
    // Player state
    uint8_t lives;             // Remaining lives
    bool isAlive;              // Current alive status
    bool isInvulnerable;       // Invulnerability after death
    float invulnerabilityTimer; // Time remaining invulnerable
    
    // Weapon systems
    uint8_t zapEnergy;         // Zap weapon energy (0-255)
    uint8_t fireEnergy;        // Fire weapon energy (0-255)
    uint8_t superZapperUses;   // Super Zapper uses remaining (0-2)
    float weaponCooldown;      // Weapon cooldown timer
    
    // Movement
    float moveSpeed;           // Movement speed around tube
    float acceleration;        // Movement acceleration
    float deceleration;        // Movement deceleration when stopping
    float maxSpeed;            // Maximum movement speed
    float currentSpeed;        // Current movement speed
    bool isMoving;             // Currently moving
    int8_t moveDirection;      // -1 for left, 0 for none, 1 for right
    float momentum;            // Movement momentum for smooth stops
    
    // Continuous position (floating point segment position)
    float continuousSegment;   // Continuous segment position (0.0-16.0)
    bool allowContinuousMovement; // Allow movement while already moving
    
    // Movement smoothing
    float smoothingFactor;     // Movement smoothing factor (0.0-1.0)
    float lastFramePosition;   // Last frame's position for smooth interpolation
    
    // Input state
    bool zapPressed;           // Zap button pressed
    bool firePressed;          // Fire button pressed
    bool superZapperPressed;   // Super Zapper button pressed
    
    PlayerComponent() : segment(0), targetSegment(0), segmentLerp(0.0f),
                       lives(3), isAlive(true), isInvulnerable(false), invulnerabilityTimer(0.0f),
                       zapEnergy(255), fireEnergy(255), superZapperUses(2), weaponCooldown(0.0f),
                       moveSpeed(5.0f), acceleration(15.0f), deceleration(20.0f), maxSpeed(8.0f), currentSpeed(0.0f),
                       isMoving(false), moveDirection(0), momentum(0.0f),
                       continuousSegment(0.0f), allowContinuousMovement(true),
                       smoothingFactor(0.8f), lastFramePosition(0.0f),
                       zapPressed(false), firePressed(false), superZapperPressed(false) {}
    
    ComponentTypeID getTypeId() const override { return 1; }
    const char* getTypeName() const override { return "PlayerComponent"; }
};

/**
 * @brief Player movement system
 * 
 * Handles player movement around the 16-segment tube based on input.
 */
class PlayerMovementSystem {
public:
    static void update(PlayerComponent& player, float deltaTime);
    static void handleInput(PlayerComponent& player, const PlayerInputEvent& event);
    static constexpr uint8_t NUM_SEGMENTS = 16;
    static constexpr float MOVE_SPEED = 5.0f;
    static constexpr float INVULNERABILITY_DURATION = 3.0f;
};

/**
 * @brief Player weapon system
 * 
 * Handles player weapons: Zap (close range), Fire (long range), and Super Zapper.
 */
class PlayerWeaponSystem {
public:
    static void update(PlayerComponent& player, float deltaTime);
    static void handleInput(PlayerComponent& player, const PlayerInputEvent& event);
    static constexpr float ZAP_COOLDOWN = 0.1f;
    static constexpr float FIRE_COOLDOWN = 0.2f;
    static constexpr float SUPER_ZAPPER_COOLDOWN = 0.5f;
    static constexpr uint8_t ZAP_ENERGY_COST = 10;
    static constexpr uint8_t FIRE_ENERGY_COST = 20;
};

/**
 * @brief Player collision system
 * 
 * Handles player collision detection and response.
 */
class PlayerCollisionSystem {
public:
    static void update(PlayerComponent& player, float deltaTime);
    static bool checkCollision(const PlayerComponent& player, const glm::vec3& enemyPosition);
    
private:
    static constexpr float PLAYER_RADIUS = 0.1f;
    static constexpr float COLLISION_THRESHOLD = 0.2f;
};

/**
 * @brief Player class that manages all player systems
 */
class Player {
public:
    Player();
    ~Player() = default;

    void initialize();
    void update(float deltaTime);
    void render();
    void shutdown();
    
    // Input handling
    void handleInput(const PlayerInputEvent& event);
    
    // State queries
    bool isAlive() const { return component_.isAlive; }
    uint8_t getLives() const { return component_.lives; }
    uint8_t getSegment() const { return component_.segment; }
    float getContinuousSegment() const { return component_.continuousSegment; }
    bool isInvulnerable() const { return component_.isInvulnerable; }
    bool isMoving() const { return component_.isMoving; }
    int8_t getMoveDirection() const { return component_.moveDirection; }
    
    // Position queries
    glm::vec3 getPosition() const;
    glm::vec3 getPosition(float depth) const;
    float getSegmentAngle() const;
    
    // Weapon queries
    uint8_t getZapEnergy() const { return component_.zapEnergy; }
    uint8_t getFireEnergy() const { return component_.fireEnergy; }
    uint8_t getSuperZapperUses() const { return component_.superZapperUses; }
    
    // Actions
    void takeDamage();
    void addLife();
    void respawn();
    
    // Component access
    PlayerComponent& getComponent() { return component_; }
    const PlayerComponent& getComponent() const { return component_; }

private:
    PlayerComponent component_;
    
    // Systems
    PlayerMovementSystem movementSystem_;
    PlayerWeaponSystem weaponSystem_;
    PlayerCollisionSystem collisionSystem_;
    
    // Event handling
    void onPlayerDeath();
    void onPlayerRespawn();
};

} // namespace Tempest 