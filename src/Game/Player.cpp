#include "Player.h"
#include "TubeGeometry.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <cmath>

namespace Tempest {

// PlayerMovementSystem implementation
void PlayerMovementSystem::update(PlayerComponent& player, float deltaTime) {
    if (!player.isAlive) return;
    
    // Store last frame position for smooth interpolation
    player.lastFramePosition = player.continuousSegment;
    
    // Handle continuous movement
    if (player.moveDirection != 0) {
        // Accelerate in the direction of movement
        player.currentSpeed = std::min(player.maxSpeed, player.currentSpeed + player.acceleration * deltaTime);
        player.momentum = player.currentSpeed * player.moveDirection;
    } else {
        // Decelerate when not moving
        float deceleration = player.deceleration * deltaTime;
        if (std::abs(player.momentum) > deceleration) {
            player.momentum -= std::copysign(deceleration, player.momentum);
        } else {
            player.momentum = 0.0f;
            player.currentSpeed = 0.0f;
        }
    }
    
    // Apply momentum to continuous segment position
    if (std::abs(player.momentum) > 0.001f) {
        player.continuousSegment += player.momentum * deltaTime;
        
        // Wrap around the tube (0-16 segments)
        if (player.continuousSegment < 0.0f) {
            player.continuousSegment += NUM_SEGMENTS;
        } else if (player.continuousSegment >= NUM_SEGMENTS) {
            player.continuousSegment -= NUM_SEGMENTS;
        }
        
        player.isMoving = true;
    } else {
        player.isMoving = false;
    }
    
    // Update discrete segment position (for collision detection)
    player.segment = static_cast<uint8_t>(player.continuousSegment) % NUM_SEGMENTS;
    
    // Calculate interpolation factor within current segment
    player.segmentLerp = player.continuousSegment - std::floor(player.continuousSegment);
    
    // Update invulnerability timer
    if (player.isInvulnerable) {
        player.invulnerabilityTimer -= deltaTime;
        if (player.invulnerabilityTimer <= 0.0f) {
            player.isInvulnerable = false;
            player.invulnerabilityTimer = 0.0f;
        }
    }
}

void PlayerMovementSystem::handleInput(PlayerComponent& player, const PlayerInputEvent& event) {
    if (!player.isAlive) return;
    
    switch (event.inputType) {
        case PlayerInputEvent::InputType::MoveLeft:
            if (event.pressed) {
                player.moveDirection = -1;
                spdlog::debug("Player started moving left");
            } else if (player.moveDirection == -1) {
                player.moveDirection = 0;
                spdlog::debug("Player stopped moving left");
            }
            break;
            
        case PlayerInputEvent::InputType::MoveRight:
            if (event.pressed) {
                player.moveDirection = 1;
                spdlog::debug("Player started moving right");
            } else if (player.moveDirection == 1) {
                player.moveDirection = 0;
                spdlog::debug("Player stopped moving right");
            }
            break;
    }
}

// PlayerWeaponSystem implementation
void PlayerWeaponSystem::update(PlayerComponent& player, float deltaTime) {
    if (!player.isAlive) return;
    
    // Update weapon cooldown
    if (player.weaponCooldown > 0.0f) {
        player.weaponCooldown -= deltaTime;
        player.weaponCooldown = std::max(0.0f, player.weaponCooldown);
    }
    
    // Regenerate weapon energy
    if (player.zapEnergy < 255) {
        player.zapEnergy = std::min(255, player.zapEnergy + static_cast<uint8_t>(50 * deltaTime));
    }
    
    if (player.fireEnergy < 255) {
        player.fireEnergy = std::min(255, player.fireEnergy + static_cast<uint8_t>(30 * deltaTime));
    }
}

void PlayerWeaponSystem::handleInput(PlayerComponent& player, const PlayerInputEvent& event) {
    if (!player.isAlive || player.weaponCooldown > 0.0f) return;
    
    switch (event.inputType) {
        case PlayerInputEvent::InputType::Zap:
            if (event.pressed && player.zapEnergy >= ZAP_ENERGY_COST) {
                player.zapPressed = true;
                player.zapEnergy -= ZAP_ENERGY_COST;
                player.weaponCooldown = ZAP_COOLDOWN;
                
                // Dispatch zap event
                // TODO: Create and dispatch weapon fire event
                spdlog::debug("Player fired Zap weapon");
            } else if (!event.pressed) {
                player.zapPressed = false;
            }
            break;
            
        case PlayerInputEvent::InputType::Fire:
            if (event.pressed && player.fireEnergy >= FIRE_ENERGY_COST) {
                player.firePressed = true;
                player.fireEnergy -= FIRE_ENERGY_COST;
                player.weaponCooldown = FIRE_COOLDOWN;
                
                // Dispatch fire event
                // TODO: Create and dispatch weapon fire event
                spdlog::debug("Player fired Fire weapon");
            } else if (!event.pressed) {
                player.firePressed = false;
            }
            break;
            
        case PlayerInputEvent::InputType::SuperZapper:
            if (event.pressed && player.superZapperUses > 0) {
                player.superZapperPressed = true;
                player.superZapperUses--;
                player.weaponCooldown = SUPER_ZAPPER_COOLDOWN;
                
                // Dispatch super zapper event
                // TODO: Create and dispatch super zapper event
                spdlog::debug("Player used Super Zapper");
            } else if (!event.pressed) {
                player.superZapperPressed = false;
            }
            break;
    }
}

// PlayerCollisionSystem implementation
void PlayerCollisionSystem::update(PlayerComponent& player, float deltaTime) {
    // Collision detection will be handled by the main game loop
    // This system mainly handles collision response
}

bool PlayerCollisionSystem::checkCollision(const PlayerComponent& player, const glm::vec3& enemyPosition) {
    // Enhanced collision detection using continuous segment position
    
    // Calculate player position in 3D space using continuous segment
    float angle = (player.continuousSegment / 16.0f) * 2.0f * 3.14159f;
    glm::vec3 playerPos(cos(angle) * PLAYER_RADIUS, 0.0f, sin(angle) * PLAYER_RADIUS);
    
    // Calculate distance between player and enemy
    float distance = glm::length(enemyPosition - playerPos);
    
    // Check if collision occurs
    bool collision = distance < COLLISION_THRESHOLD;
    
    if (collision) {
        spdlog::debug("Player collision detected at segment {:.2f}, distance: {:.3f}", 
                     player.continuousSegment, distance);
    }
    
    return collision;
}

// Player class implementation
Player::Player() {
    spdlog::info("Player created");
}

void Player::initialize() {
    component_ = PlayerComponent();
    spdlog::info("Player initialized at segment {}", component_.segment);
}

void Player::update(float deltaTime) {
    if (!component_.isAlive) return;
    
    // Update all systems
    PlayerMovementSystem::update(component_, deltaTime);
    PlayerWeaponSystem::update(component_, deltaTime);
    PlayerCollisionSystem::update(component_, deltaTime);
}

void Player::render() {
    // TODO: Implement player rendering
    // This will be handled by the graphics system
}

void Player::shutdown() {
    spdlog::info("Player shutdown");
}

void Player::handleInput(const PlayerInputEvent& event) {
    PlayerMovementSystem::handleInput(component_, event);
    PlayerWeaponSystem::handleInput(component_, event);
}

void Player::takeDamage() {
    if (component_.isInvulnerable) return;
    
    component_.lives--;
    component_.isAlive = false;
    component_.isInvulnerable = true;
    component_.invulnerabilityTimer = PlayerMovementSystem::INVULNERABILITY_DURATION;
    
    onPlayerDeath();
    
    spdlog::warn("Player took damage! Lives remaining: {}", component_.lives);
}

void Player::addLife() {
    component_.lives++;
    spdlog::info("Player gained a life! Total lives: {}", component_.lives);
}

void Player::respawn() {
    if (component_.lives > 0) {
        component_.isAlive = true;
        component_.isInvulnerable = true;
        component_.invulnerabilityTimer = PlayerMovementSystem::INVULNERABILITY_DURATION;
        
        // Reset weapon energy
        component_.zapEnergy = 255;
        component_.fireEnergy = 255;
        
        // Reset movement state
        component_.moveDirection = 0;
        component_.currentSpeed = 0.0f;
        component_.momentum = 0.0f;
        component_.isMoving = false;
        
        onPlayerRespawn();
        
        spdlog::info("Player respawned at segment {}", component_.segment);
    }
}

glm::vec3 Player::getPosition() const {
    return getPosition(TubeGeometry::DEPTH_NEAR);
}

glm::vec3 Player::getPosition(float depth) const {
    float angle = getSegmentAngle();
    float radius = TubeGeometry::TUBE_RADIUS;
    
    // Apply depth scaling for perspective
    float scale = 1.0f - (depth / TubeGeometry::TUBE_DEPTH) * 0.5f;
    radius *= scale;
    
    return glm::vec3(
        cos(angle) * radius,
        -depth,  // Negative Y for depth into screen
        sin(angle) * radius
    );
}

float Player::getSegmentAngle() const {
    return (component_.continuousSegment / PlayerMovementSystem::NUM_SEGMENTS) * 2.0f * 3.14159f;
}

void Player::onPlayerDeath() {
    // Dispatch player death event
    PlayerDeathEvent event(component_.lives);
    EventSystem::getInstance().dispatch(event);
    
    spdlog::info("Player death event dispatched");
}

void Player::onPlayerRespawn() {
    // Dispatch player respawn event
    // TODO: Create PlayerRespawnEvent and dispatch it
    spdlog::info("Player respawn event dispatched");
}

} // namespace Tempest 