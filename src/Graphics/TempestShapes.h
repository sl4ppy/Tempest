#pragma once

#include "VectorCommand.h"
#include <vector>
#include <glm/glm.hpp>

namespace Tempest {

/**
 * @brief TempestShapes class for generating classic Tempest vector graphics
 * 
 * This class recreates the original Tempest shapes from the 1981 arcade game
 * using the modern VectorCommand system. All shapes are based on the original
 * vsdraw commands from the Tempest.asm disassembly.
 */
class TempestShapes {
public:
    TempestShapes();
    ~TempestShapes();
    
    // Player ship shapes
    std::vector<VectorCommand> CreatePlayerShip(float scale = 1.0f);
    std::vector<VectorCommand> CreatePlayerShipDestroyed(float scale = 1.0f);
    
    // Enemy shapes
    std::vector<VectorCommand> CreateSpiker(int frame, float scale = 1.0f);      // frame 0-3
    std::vector<VectorCommand> CreateFuzzball(int frame, float scale = 1.0f);    // frame 0-3
    std::vector<VectorCommand> CreateTanker(int type, float scale = 1.0f);       // type 0=pulsars, 1=fuzzballs, 2=flippers
    std::vector<VectorCommand> CreateFlipper(float scale = 1.0f);
    std::vector<VectorCommand> CreatePulsar(int frame, float scale = 1.0f);      // frame 0-1
    
    // Projectile shapes
    std::vector<VectorCommand> CreateEnemyShot(int frame, float scale = 1.0f);   // frame 0-3
    std::vector<VectorCommand> CreatePlayerShot(float scale = 1.0f);
    std::vector<VectorCommand> CreateSuperZapper(float scale = 1.0f);
    
    // Explosion effects
    std::vector<VectorCommand> CreateExplosion(int frame, float scale = 1.0f);   // frame 0-7
    std::vector<VectorCommand> CreateSparkle(int frame, float scale = 1.0f);     // frame 0-3
    
    // Utility functions
    void SetDefaultColor(const glm::vec4& color) { m_defaultColor = color; }
    void SetDefaultThickness(float thickness) { m_defaultThickness = thickness; }
    
private:
    glm::vec4 m_defaultColor;
    float m_defaultThickness;
    
    // Helper functions for converting original vsdraw coordinates
    void AddVectorDraw(std::vector<VectorCommand>& commands, int deltaX, int deltaY, 
                       const glm::vec4& color, float thickness, float scale);
    void AddVectorMove(std::vector<VectorCommand>& commands, int deltaX, int deltaY, float scale);
    void SetVectorColor(std::vector<VectorCommand>& commands, int colorIndex);
    
    // Original coordinate system conversion
    glm::vec2 ConvertCoordinates(int x, int y, float scale);
    glm::vec4 GetTempestColor(int colorIndex);
    
    // Shape generation helpers
    void CreateSpikerFrame1(std::vector<VectorCommand>& commands, float scale);
    void CreateSpikerFrame2(std::vector<VectorCommand>& commands, float scale);
    void CreateSpikerFrame3(std::vector<VectorCommand>& commands, float scale);
    void CreateSpikerFrame4(std::vector<VectorCommand>& commands, float scale);
    
    void CreateFuzzballFrame1(std::vector<VectorCommand>& commands, float scale);
    void CreateFuzzballFrame2(std::vector<VectorCommand>& commands, float scale);
    void CreateFuzzballFrame3(std::vector<VectorCommand>& commands, float scale);
    void CreateFuzzballFrame4(std::vector<VectorCommand>& commands, float scale);
    
    void CreateTankerPulsars(std::vector<VectorCommand>& commands, float scale);
    void CreateTankerFuzzballs(std::vector<VectorCommand>& commands, float scale);
    void CreateTankerFlippers(std::vector<VectorCommand>& commands, float scale);
    void CreateTankerBody(std::vector<VectorCommand>& commands, float scale);
    
    void CreateEnemyShotFrame1(std::vector<VectorCommand>& commands, float scale);
    void CreateEnemyShotFrame2(std::vector<VectorCommand>& commands, float scale);
    void CreateEnemyShotFrame3(std::vector<VectorCommand>& commands, float scale);
    void CreateEnemyShotFrame4(std::vector<VectorCommand>& commands, float scale);
};

} // namespace Tempest 