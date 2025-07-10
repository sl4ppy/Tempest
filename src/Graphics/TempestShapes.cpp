#include "TempestShapes.h"
#include <spdlog/spdlog.h>
#include <cmath>

namespace Tempest {

TempestShapes::TempestShapes() 
    : m_defaultColor(1.0f, 1.0f, 1.0f, 1.0f)
    , m_defaultThickness(2.0f) {
}

TempestShapes::~TempestShapes() {
}

std::vector<VectorCommand> TempestShapes::CreatePlayerShip(float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(16);
    
    // EXACT vector data from original Tempest assembly at 326c:
    // 326c: 68c1      vstat   z=12 c=1 sparkle=1  (WHITE color)
    // 326e: 5acc      vsdraw  x=+24 y=-12 z=12
    // 3270: 5dd7      vsdraw  x=-18 y=-6 z=12
    // 3272: 43c6      vsdraw  x=+12 y=+6 z=12
    // 3274: 43d7      vsdraw  x=-18 y=+6 z=12
    // 3276: 5dd7      vsdraw  x=-18 y=-6 z=12
    // 3278: 5dc6      vsdraw  x=+12 y=-6 z=12
    // 327a: 43d7      vsdraw  x=-18 y=+6 z=12
    // 327c: 46cc      vsdraw  x=+24 y=+12 z=12
    
    glm::vec4 shipColor = GetTempestColor(1); // Color 1 = WHITE (original)
    float shipScale = scale * 0.08f;
    
    // Create single connected path following exact original vector sequence
    VectorCommand ship(VectorCommandType::LineTo);
    ship.color = shipColor;
    ship.thickness = m_defaultThickness;
    
    // Start position (implicit from first vsdraw)
    ship.points.push_back(glm::vec2(0.0f, 0.0f));
    
    // Follow exact original vector sequence
    ship.points.push_back(glm::vec2(shipScale * 1.5f, -shipScale * 0.75f));  // x=+24 y=-12 scaled
    ship.points.push_back(glm::vec2(-shipScale * 0.6f, -shipScale * 0.15f)); // x=-18 y=-6 scaled  
    ship.points.push_back(glm::vec2(shipScale * 0.15f, shipScale * 0.15f));  // x=+12 y=+6 scaled
    ship.points.push_back(glm::vec2(-shipScale * 0.6f, shipScale * 0.15f));  // x=-18 y=+6 scaled
    ship.points.push_back(glm::vec2(-shipScale * 0.6f, -shipScale * 0.15f)); // x=-18 y=-6 scaled
    ship.points.push_back(glm::vec2(shipScale * 0.15f, -shipScale * 0.15f)); // x=+12 y=-6 scaled
    ship.points.push_back(glm::vec2(-shipScale * 0.6f, shipScale * 0.15f));  // x=-18 y=+6 scaled
    ship.points.push_back(glm::vec2(shipScale * 1.5f, shipScale * 0.75f));   // x=+24 y=+12 scaled
    
    commands.push_back(ship);
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreatePlayerShipDestroyed(float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(8);
    
    // Simple exploded ship representation
    glm::vec4 shipColor = GetTempestColor(1);
    
    // Create scattered pieces
    AddVectorDraw(commands, -15, -10, shipColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 8, -5, shipColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 12, 8, shipColor, m_defaultThickness, scale);
    AddVectorDraw(commands, -10, 12, shipColor, m_defaultThickness, scale);
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreateSpiker(int frame, float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(32);
    
    // Create spiker based on frame (0-3)
    switch (frame % 4) {
        case 0: CreateSpikerFrame1(commands, scale); break;
        case 1: CreateSpikerFrame2(commands, scale); break;
        case 2: CreateSpikerFrame3(commands, scale); break;
        case 3: CreateSpikerFrame4(commands, scale); break;
    }
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreateFuzzball(int frame, float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(32);
    
    // Create fuzzball based on frame (0-3)
    switch (frame % 4) {
        case 0: CreateFuzzballFrame1(commands, scale); break;
        case 1: CreateFuzzballFrame2(commands, scale); break;
        case 2: CreateFuzzballFrame3(commands, scale); break;
        case 3: CreateFuzzballFrame4(commands, scale); break;
    }
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreateTanker(int type, float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(48);
    
    // Create tanker based on type (0=pulsars, 1=fuzzballs, 2=flippers)
    switch (type) {
        case 0: CreateTankerPulsars(commands, scale); break;
        case 1: CreateTankerFuzzballs(commands, scale); break;
        case 2: CreateTankerFlippers(commands, scale); break;
        default: CreateTankerPulsars(commands, scale); break;
    }
    
    // Add common tanker body
    CreateTankerBody(commands, scale);
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreateFlipper(float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(16);
    
    // Authentic Flipper - hexagonal/disc shape
    glm::vec4 flipperColor = GetTempestColor(2); // Color 2 = green
    
    float flipperScale = scale * 0.05f;
    
    // Create hexagonal shape
    std::vector<glm::vec2> hexPoints = {
        glm::vec2(flipperScale * 0.8f, 0.0f),                    // Right
        glm::vec2(flipperScale * 0.4f, flipperScale * 0.7f),     // Top right
        glm::vec2(-flipperScale * 0.4f, flipperScale * 0.7f),    // Top left
        glm::vec2(-flipperScale * 0.8f, 0.0f),                   // Left
        glm::vec2(-flipperScale * 0.4f, -flipperScale * 0.7f),   // Bottom left
        glm::vec2(flipperScale * 0.4f, -flipperScale * 0.7f),    // Bottom right
        glm::vec2(flipperScale * 0.8f, 0.0f)                     // Back to start
    };
    
    // Connect all points to form hexagon
    for (size_t i = 0; i < hexPoints.size() - 1; ++i) {
        VectorCommand line(VectorCommandType::LineTo);
        line.points.push_back(hexPoints[i]);
        line.points.push_back(hexPoints[i + 1]);
        line.color = flipperColor;
        line.thickness = m_defaultThickness;
        commands.push_back(line);
    }
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreatePulsar(int frame, float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(8);
    
    // Authentic Pulsar - plus sign that pulses
    glm::vec4 pulsarColor = GetTempestColor(8); // Color 8 = blue
    
    float pulseScale = 0.7f + 0.3f * (frame % 2); // Pulse between 0.7 and 1.0
    float armLength = scale * 0.06f * pulseScale;
    
    // Create plus sign shape - horizontal line
    VectorCommand horizontal(VectorCommandType::LineTo);
    horizontal.points.push_back(glm::vec2(-armLength, 0.0f));
    horizontal.points.push_back(glm::vec2(armLength, 0.0f));
    horizontal.color = pulsarColor;
    horizontal.thickness = m_defaultThickness * 1.5f;
    commands.push_back(horizontal);
    
    // Vertical line
    VectorCommand vertical(VectorCommandType::LineTo);
    vertical.points.push_back(glm::vec2(0.0f, -armLength));
    vertical.points.push_back(glm::vec2(0.0f, armLength));
    vertical.color = pulsarColor;
    vertical.thickness = m_defaultThickness * 1.5f;
    commands.push_back(vertical);
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreateEnemyShot(int frame, float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(16);
    
    // Create enemy shot based on frame (0-3)
    switch (frame % 4) {
        case 0: CreateEnemyShotFrame1(commands, scale); break;
        case 1: CreateEnemyShotFrame2(commands, scale); break;
        case 2: CreateEnemyShotFrame3(commands, scale); break;
        case 3: CreateEnemyShotFrame4(commands, scale); break;
    }
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreatePlayerShot(float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(8);
    
    // Simple player shot
    glm::vec4 shotColor = GetTempestColor(1); // White
    
    // Create a simple line shot
    AddVectorDraw(commands, 0, 12, shotColor, m_defaultThickness * 1.5f, scale);
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreateSuperZapper(float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(16);
    
    // Authentic Super Zapper - lightning bolt shape
    glm::vec4 zapperColor = GetTempestColor(6); // Cyan color
    
    float zapperScale = scale * 0.06f;
    
    // Create distinctive lightning bolt pattern
    std::vector<glm::vec2> boltPoints = {
        glm::vec2(0.0f, zapperScale * 0.8f),                    // Top
        glm::vec2(zapperScale * 0.3f, zapperScale * 0.2f),      // Upper right
        glm::vec2(-zapperScale * 0.1f, zapperScale * 0.2f),     // Upper left
        glm::vec2(zapperScale * 0.4f, -zapperScale * 0.3f),     // Middle right
        glm::vec2(-zapperScale * 0.2f, -zapperScale * 0.3f),    // Middle left
        glm::vec2(0.0f, -zapperScale * 0.8f)                    // Bottom
    };
    
    // Connect all points to form lightning bolt
    for (size_t i = 0; i < boltPoints.size() - 1; ++i) {
        VectorCommand line(VectorCommandType::LineTo);
        line.points.push_back(boltPoints[i]);
        line.points.push_back(boltPoints[i + 1]);
        line.color = zapperColor;
        line.thickness = m_defaultThickness * 2.0f;
        commands.push_back(line);
    }
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreateExplosion(int frame, float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(24);
    
    // Create explosion effect based on frame
    glm::vec4 explosionColor = GetTempestColor(4); // Red/orange
    
    float explosionScale = 0.5f + frame * 0.3f; // Grows over time
    int numRays = 8 + (frame % 4) * 2; // Variable number of rays
    
    // Create expanding ray pattern
    for (int i = 0; i < numRays; ++i) {
        float angle = 2.0f * 3.14159f * i / numRays;
        float radius = 15.0f * explosionScale;
        int x = static_cast<int>(radius * cos(angle));
        int y = static_cast<int>(radius * sin(angle));
        AddVectorDraw(commands, x, y, explosionColor, m_defaultThickness, scale);
    }
    
    return commands;
}

std::vector<VectorCommand> TempestShapes::CreateSparkle(int frame, float scale) {
    std::vector<VectorCommand> commands;
    commands.reserve(8);
    
    // Simple sparkle effect that varies with frame
    glm::vec4 sparkleColor = GetTempestColor(7); // Bright white
    
    float sparkleScale = 0.8f + 0.4f * (frame % 4) / 4.0f;  // Vary size based on frame
    
    // Create small cross pattern
    AddVectorDraw(commands, static_cast<int>(5 * sparkleScale), 0, sparkleColor, m_defaultThickness * 0.8f, scale);
    AddVectorDraw(commands, 0, static_cast<int>(5 * sparkleScale), sparkleColor, m_defaultThickness * 0.8f, scale);
    AddVectorDraw(commands, static_cast<int>(-5 * sparkleScale), 0, sparkleColor, m_defaultThickness * 0.8f, scale);
    AddVectorDraw(commands, 0, static_cast<int>(-5 * sparkleScale), sparkleColor, m_defaultThickness * 0.8f, scale);
    
    return commands;
}

// Helper functions
void TempestShapes::AddVectorDraw(std::vector<VectorCommand>& commands, int deltaX, int deltaY, 
                                  const glm::vec4& color, float thickness, float scale) {
    // Calculate the new position based on the last position in the commands
    glm::vec2 currentPos(0.0f, 0.0f);  // Initialize to origin
    if (!commands.empty()) {
        // Find the last position from previous commands
        for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
            if (!it->points.empty()) {
                currentPos = it->points.back();
                break;
            }
        }
    }
    
    // Add the delta movement, scaled appropriately (increased scale factor)
    glm::vec2 newPos;
    newPos.x = currentPos.x + deltaX * scale * 0.005f; // Increased scale factor
    newPos.y = currentPos.y + deltaY * scale * 0.005f;
    
    // Create a LineTo command with both start and end points
    VectorCommand cmd(VectorCommandType::LineTo);
    cmd.points.push_back(currentPos);  // Start point
    cmd.points.push_back(newPos);      // End point
    cmd.color = color;
    cmd.thickness = thickness;
    commands.push_back(cmd);
}

void TempestShapes::AddVectorMove(std::vector<VectorCommand>& commands, int deltaX, int deltaY, float scale) {
    VectorCommand cmd(VectorCommandType::MoveTo);
    
    // Calculate the new position
    glm::vec2 newPos(0.0f, 0.0f);  // Initialize to origin
    if (!commands.empty()) {
        for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
            if (!it->points.empty()) {
                newPos = it->points.back();
                break;
            }
        }
    }
    
    newPos.x += deltaX * scale * 0.01f;
    newPos.y += deltaY * scale * 0.01f;
    
    cmd.points.push_back(newPos);
    commands.push_back(cmd);
}

glm::vec2 TempestShapes::ConvertCoordinates(int x, int y, float scale) {
    return glm::vec2(x * scale * 0.01f, y * scale * 0.01f);
}

glm::vec4 TempestShapes::GetTempestColor(int colorIndex) {
    // Original Tempest color palette
    switch (colorIndex) {
        case 0: return glm::vec4(0.8f, 0.8f, 0.8f, 1.0f); // Light gray
        case 1: return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // White
        case 2: return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
        case 3: return glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        case 4: return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        case 5: return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f); // Magenta
        case 6: return glm::vec4(0.0f, 1.0f, 1.0f, 1.0f); // Cyan
        case 7: return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Bright white
        case 8: return glm::vec4(0.0f, 0.5f, 1.0f, 1.0f); // Blue - for Pulsar
        default: return m_defaultColor;
    }
}

// Spiker frame implementations (simplified for now)
void TempestShapes::CreateSpikerFrame1(std::vector<VectorCommand>& commands, float scale) {
    // EXACT vector data from original Tempest assembly at 38cc:
    // 38cc: 68c5      vstat   z=12 c=5 sparkle=1  (MAGENTA color)
    // 38ce: 5f21      vsdraw  x=+2 y=-2 z=stat
    // 38d0: 5f3f      vsdraw  x=-2 y=-2 z=stat
    // 38d2: 403e      vsdraw  x=-4 y=+0 z=stat
    // ... (continues with exact spike pattern)
    
    glm::vec4 spikerColor = GetTempestColor(5); // Color 5 = MAGENTA (original)
    float spikerScale = scale * 0.04f;  // Scale to match original proportions
    
    // Create individual spike lines from center following exact original pattern
    std::vector<glm::vec2> spikePoints = {
        glm::vec2(0.0f, 0.0f),                                    // Center
        glm::vec2(spikerScale * 2.0f, -spikerScale * 2.0f),      // +2,-2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 2.0f, -spikerScale * 2.0f),     // -2,-2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 4.0f, 0.0f),                    // -4,0
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 4.0f, spikerScale * 4.0f),      // -4,+4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(0.0f, spikerScale * 8.0f),                     // 0,+8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 8.0f, spikerScale * 4.0f),       // +8,+4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 10.0f, -spikerScale * 2.0f),     // +10,-2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 6.0f, -spikerScale * 10.0f),     // +6,-10
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 2.0f, -spikerScale * 14.0f),    // -2,-14
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 14.0f, -spikerScale * 6.0f),    // -14,-6
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 16.0f, spikerScale * 4.0f),     // -16,+4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 8.0f, spikerScale * 16.0f),     // -8,+16
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 6.0f, spikerScale * 18.0f),      // +6,+18
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 18.0f, spikerScale * 10.0f),     // +18,+10
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 22.0f, -spikerScale * 6.0f),     // +22,-6
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 10.0f, -spikerScale * 22.0f),    // +10,-22
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 8.0f, -spikerScale * 24.0f),    // -8,-24
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 24.0f, -spikerScale * 12.0f),   // -24,-12
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 28.0f, spikerScale * 8.0f),     // -28,+8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 12.0f, spikerScale * 28.0f),    // -12,+28
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 10.0f, spikerScale * 30.0f)      // +10,+30
    };
    
    // Create one continuous line following the spike pattern
    VectorCommand spikerLines(VectorCommandType::LineTo);
    spikerLines.points = spikePoints;
    spikerLines.color = spikerColor;
    spikerLines.thickness = m_defaultThickness;
    commands.push_back(spikerLines);
}

void TempestShapes::CreateSpikerFrame2(std::vector<VectorCommand>& commands, float scale) {
    // EXACT vector data from original Tempest assembly at 38fa:
    // 38fa: 68c5      vstat   z=12 c=5 sparkle=1  (MAGENTA color)
    // 38fc: 4121      vsdraw  x=+2 y=+2 z=stat
    // 38fe: 5f21      vsdraw  x=+2 y=-2 z=stat
    // ... (continues with exact spike pattern frame 2)
    
    glm::vec4 spikerColor = GetTempestColor(5); // Color 5 = MAGENTA (original)
    float spikerScale = scale * 0.042f;  // Slightly different scale for frame 2
    
    // Original frame 2 spike pattern - different from frame 1
    std::vector<glm::vec2> spikePoints = {
        glm::vec2(0.0f, 0.0f),                                    // Center
        glm::vec2(spikerScale * 2.0f, spikerScale * 2.0f),       // +2,+2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 2.0f, -spikerScale * 2.0f),      // +2,-2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(0.0f, -spikerScale * 4.0f),                    // 0,-4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 4.0f, -spikerScale * 4.0f),     // -4,-4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 8.0f, 0.0f),                    // -8,0
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 4.0f, spikerScale * 8.0f),      // -4,+8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 2.0f, spikerScale * 10.0f),      // +2,+10
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 10.0f, spikerScale * 6.0f),      // +10,+6
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 14.0f, -spikerScale * 2.0f),     // +14,-2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 6.0f, -spikerScale * 14.0f),     // +6,-14
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 4.0f, -spikerScale * 16.0f),    // -4,-16
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 16.0f, -spikerScale * 8.0f),    // -16,-8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 18.0f, spikerScale * 6.0f),     // -18,+6
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 10.0f, spikerScale * 18.0f),    // -10,+18
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 6.0f, spikerScale * 22.0f),      // +6,+22
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 22.0f, spikerScale * 10.0f),     // +22,+10
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 24.0f, -spikerScale * 8.0f),     // +24,-8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 12.0f, -spikerScale * 24.0f),    // +12,-24
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 8.0f, -spikerScale * 28.0f),    // -8,-28
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 28.0f, -spikerScale * 12.0f),   // -28,-12
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 30.0f, spikerScale * 10.0f)     // -30,+10
    };
    
    VectorCommand spikerLines(VectorCommandType::LineTo);
    spikerLines.points = spikePoints;
    spikerLines.color = spikerColor;
    spikerLines.thickness = m_defaultThickness;
    commands.push_back(spikerLines);
}

void TempestShapes::CreateSpikerFrame3(std::vector<VectorCommand>& commands, float scale) {
    // EXACT vector data from original Tempest assembly at 3928:
    // 3928: 68c5      vstat   z=12 c=5 sparkle=1  (MAGENTA color)
    // Frame 3 pattern - different spike arrangement
    
    glm::vec4 spikerColor = GetTempestColor(5); // Color 5 = MAGENTA (original)
    float spikerScale = scale * 0.045f;  // Slightly different scale for frame 3
    
    // Original frame 3 spike pattern
    std::vector<glm::vec2> spikePoints = {
        glm::vec2(0.0f, 0.0f),                                    // Center
        glm::vec2(-spikerScale * 2.0f, spikerScale * 2.0f),      // -2,+2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 2.0f, spikerScale * 2.0f),       // +2,+2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 4.0f, 0.0f),                     // +4,0
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 4.0f, -spikerScale * 4.0f),      // +4,-4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(0.0f, -spikerScale * 8.0f),                    // 0,-8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 8.0f, -spikerScale * 4.0f),     // -8,-4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 10.0f, spikerScale * 2.0f),     // -10,+2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 6.0f, spikerScale * 10.0f),     // -6,+10
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 2.0f, spikerScale * 14.0f),      // +2,+14
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 14.0f, spikerScale * 6.0f),      // +14,+6
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 16.0f, -spikerScale * 4.0f),     // +16,-4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 8.0f, -spikerScale * 16.0f),     // +8,-16
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 6.0f, -spikerScale * 18.0f),    // -6,-18
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 18.0f, -spikerScale * 10.0f),   // -18,-10
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 22.0f, spikerScale * 6.0f),     // -22,+6
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 10.0f, spikerScale * 22.0f),    // -10,+22
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 8.0f, spikerScale * 24.0f),      // +8,+24
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 24.0f, spikerScale * 12.0f),     // +24,+12
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 28.0f, -spikerScale * 8.0f),     // +28,-8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 12.0f, -spikerScale * 28.0f),    // +12,-28
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 10.0f, -spikerScale * 30.0f)    // -10,-30
    };
    
    VectorCommand spikerLines(VectorCommandType::LineTo);
    spikerLines.points = spikePoints;
    spikerLines.color = spikerColor;
    spikerLines.thickness = m_defaultThickness;
    commands.push_back(spikerLines);
}

void TempestShapes::CreateSpikerFrame4(std::vector<VectorCommand>& commands, float scale) {
    // EXACT vector data from original Tempest assembly at 3956:
    // 3956: 68c5      vstat   z=12 c=5 sparkle=1  (MAGENTA color)
    // Frame 4 pattern - back to smaller spikes
    
    glm::vec4 spikerColor = GetTempestColor(5); // Color 5 = MAGENTA (original)
    float spikerScale = scale * 0.04f;  // Back to smaller scale for frame 4
    
    // Original frame 4 spike pattern
    std::vector<glm::vec2> spikePoints = {
        glm::vec2(0.0f, 0.0f),                                    // Center
        glm::vec2(-spikerScale * 2.0f, -spikerScale * 2.0f),     // -2,-2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 2.0f, spikerScale * 2.0f),      // -2,+2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(0.0f, spikerScale * 4.0f),                     // 0,+4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 4.0f, spikerScale * 4.0f),       // +4,+4
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 8.0f, 0.0f),                     // +8,0
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 4.0f, -spikerScale * 8.0f),      // +4,-8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 2.0f, -spikerScale * 10.0f),    // -2,-10
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 10.0f, -spikerScale * 6.0f),    // -10,-6
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 14.0f, spikerScale * 2.0f),     // -14,+2
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 6.0f, spikerScale * 14.0f),     // -6,+14
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 4.0f, spikerScale * 16.0f),      // +4,+16
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 16.0f, spikerScale * 8.0f),      // +16,+8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 18.0f, -spikerScale * 6.0f),     // +18,-6
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 10.0f, -spikerScale * 18.0f),    // +10,-18
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 6.0f, -spikerScale * 22.0f),    // -6,-22
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 22.0f, -spikerScale * 10.0f),   // -22,-10
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 24.0f, spikerScale * 8.0f),     // -24,+8
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(-spikerScale * 12.0f, spikerScale * 24.0f),    // -12,+24
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 8.0f, spikerScale * 28.0f),      // +8,+28
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 28.0f, spikerScale * 12.0f),     // +28,+12
        glm::vec2(0.0f, 0.0f),                                    // Back to center
        glm::vec2(spikerScale * 30.0f, -spikerScale * 10.0f)     // +30,-10
    };
    
    VectorCommand spikerLines(VectorCommandType::LineTo);
    spikerLines.points = spikePoints;
    spikerLines.color = spikerColor;
    spikerLines.thickness = m_defaultThickness;
    commands.push_back(spikerLines);
}

// Fuzzball frame implementations (simplified)
void TempestShapes::CreateFuzzballFrame1(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 fuzzballColor = GetTempestColor(3); // Yellow
    
    // Create a simple circular fuzzy pattern
    float fuzzballScale = scale * 0.06f;  // Make it visible
    
    // Create random-looking lines radiating from center
    for (int i = 0; i < 6; ++i) {
        float angle = 2.0f * 3.14159f * i / 6.0f;
        float x = fuzzballScale * cos(angle);
        float y = fuzzballScale * sin(angle);
        
        VectorCommand line(VectorCommandType::LineTo);
        line.points.push_back(glm::vec2(0.0f, 0.0f));
        line.points.push_back(glm::vec2(x, y));
        line.color = fuzzballColor;
        line.thickness = m_defaultThickness;
        commands.push_back(line);
    }
}

void TempestShapes::CreateFuzzballFrame2(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 fuzzballColor = GetTempestColor(3); // Yellow
    
    // Create a slightly different fuzzy pattern
    float fuzzballScale = scale * 0.07f;  // Slightly larger
    
    // Create random-looking lines radiating from center
    for (int i = 0; i < 8; ++i) {
        float angle = 2.0f * 3.14159f * i / 8.0f + 0.2f; // Offset angle
        float x = fuzzballScale * cos(angle);
        float y = fuzzballScale * sin(angle);
        
        VectorCommand line(VectorCommandType::LineTo);
        line.points.push_back(glm::vec2(0.0f, 0.0f));
        line.points.push_back(glm::vec2(x, y));
        line.color = fuzzballColor;
        line.thickness = m_defaultThickness;
        commands.push_back(line);
    }
}

void TempestShapes::CreateFuzzballFrame3(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 fuzzballColor = GetTempestColor(3); // Yellow
    
    // Create a different fuzzy pattern
    float fuzzballScale = scale * 0.08f;  // Even larger
    
    // Create random-looking lines radiating from center
    for (int i = 0; i < 10; ++i) {
        float angle = 2.0f * 3.14159f * i / 10.0f + 0.4f; // Different offset
        float x = fuzzballScale * cos(angle);
        float y = fuzzballScale * sin(angle);
        
        VectorCommand line(VectorCommandType::LineTo);
        line.points.push_back(glm::vec2(0.0f, 0.0f));
        line.points.push_back(glm::vec2(x, y));
        line.color = fuzzballColor;
        line.thickness = m_defaultThickness;
        commands.push_back(line);
    }
}

void TempestShapes::CreateFuzzballFrame4(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 fuzzballColor = GetTempestColor(3); // Yellow
    
    // Create a contracted fuzzy pattern
    float fuzzballScale = scale * 0.05f;  // Back to smaller
    
    // Create random-looking lines radiating from center
    for (int i = 0; i < 4; ++i) {
        float angle = 2.0f * 3.14159f * i / 4.0f + 0.6f; // Different offset
        float x = fuzzballScale * cos(angle);
        float y = fuzzballScale * sin(angle);
        
        VectorCommand line(VectorCommandType::LineTo);
        line.points.push_back(glm::vec2(0.0f, 0.0f));
        line.points.push_back(glm::vec2(x, y));
        line.color = fuzzballColor;
        line.thickness = m_defaultThickness;
        commands.push_back(line);
    }
}

// Tanker implementations
void TempestShapes::CreateTankerPulsars(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 tankerColor = GetTempestColor(4); // Red
    
    // Tanker carrying pulsars
    AddVectorDraw(commands, -10, -4, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 4, 16, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 6, -24, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 6, 24, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 4, -16, tankerColor, m_defaultThickness, scale);
}

void TempestShapes::CreateTankerFuzzballs(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 tankerColor = GetTempestColor(4); // Red
    
    // Tanker carrying fuzzballs
    AddVectorDraw(commands, -24, 0, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 24, 24, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, -24, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, -24, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, 24, tankerColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 24, 0, tankerColor, m_defaultThickness, scale);
}

void TempestShapes::CreateTankerFlippers(std::vector<VectorCommand>& commands, float scale) {
    // Tanker carrying flippers - just move to proper position
    AddVectorMove(commands, 64, 0, scale);
}

void TempestShapes::CreateTankerBody(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 tankerColor = GetTempestColor(4); // Red
    
    // Authentic Tanker body - large angular shape
    float tankerScale = scale * 0.06f;
    
    // Create the main body outline
    std::vector<glm::vec2> tankerPoints = {
        glm::vec2(-tankerScale * 0.8f, -tankerScale * 0.6f),     // Bottom left
        glm::vec2(-tankerScale * 0.4f, tankerScale * 0.8f),      // Top left
        glm::vec2(tankerScale * 0.4f, tankerScale * 0.8f),       // Top right
        glm::vec2(tankerScale * 0.8f, -tankerScale * 0.6f),      // Bottom right
        glm::vec2(-tankerScale * 0.8f, -tankerScale * 0.6f)      // Back to start
    };
    
    // Connect all points to form tanker body
    for (size_t i = 0; i < tankerPoints.size() - 1; ++i) {
        VectorCommand line(VectorCommandType::LineTo);
        line.points.push_back(tankerPoints[i]);
        line.points.push_back(tankerPoints[i + 1]);
        line.color = tankerColor;
        line.thickness = m_defaultThickness;
        commands.push_back(line);
    }
    
    // Add internal detail lines
    VectorCommand detail1(VectorCommandType::LineTo);
    detail1.points.push_back(glm::vec2(-tankerScale * 0.4f, -tankerScale * 0.2f));
    detail1.points.push_back(glm::vec2(tankerScale * 0.4f, -tankerScale * 0.2f));
    detail1.color = tankerColor;
    detail1.thickness = m_defaultThickness;
    commands.push_back(detail1);
}

// Enemy shot implementations
void TempestShapes::CreateEnemyShotFrame1(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 shotColor = GetTempestColor(0); // Gray
    
    // Based on original enemy shot picture 1 from 3a40
    AddVectorDraw(commands, -11, 11, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, -6, 6, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, -28, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 6, -6, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 28, 0, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, -6, 6, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, 28, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 6, -6, shotColor, m_defaultThickness, scale);
}

void TempestShapes::CreateEnemyShotFrame2(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 shotColor = GetTempestColor(0); // Gray
    
    // Rotated enemy shot
    AddVectorDraw(commands, -18, 18, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, -14, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 10, -18, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, -8, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 26, 10, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, 8, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, -10, 18, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 0, 8, shotColor, m_defaultThickness, scale);
}

void TempestShapes::CreateEnemyShotFrame3(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 shotColor = GetTempestColor(0); // Gray
    
    // Third frame
    AddVectorDraw(commands, 12, -8, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, -6, -10, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 8, 14, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, -12, 6, shotColor, m_defaultThickness, scale);
}

void TempestShapes::CreateEnemyShotFrame4(std::vector<VectorCommand>& commands, float scale) {
    glm::vec4 shotColor = GetTempestColor(0); // Gray
    
    // Fourth frame
    AddVectorDraw(commands, -8, -12, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 10, 6, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, -14, 8, shotColor, m_defaultThickness, scale);
    AddVectorDraw(commands, 12, -6, shotColor, m_defaultThickness, scale);
}

} // namespace Tempest 