#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Tempest {

/**
 * @brief Tube Geometry System based on original game's 3D tube design
 * 
 * Manages the 3D tube geometry with 16 segments, perspective projection,
 * and depth-based scaling. Based on the original game's tube rendering system.
 */
class TubeGeometry {
public:
    // Tube configuration
    static constexpr uint8_t NUM_SEGMENTS = 16;
    static constexpr float TUBE_RADIUS = 1.0f;
    static constexpr float TUBE_DEPTH = 10.0f;
    static constexpr float SEGMENT_ANGLE = 2.0f * 3.14159f / NUM_SEGMENTS;
    
    // Depth levels for enemies and effects
    static constexpr float DEPTH_NEAR = 0.0f;      // Player level
    static constexpr float DEPTH_MID = 5.0f;        // Mid-level enemies
    static constexpr float DEPTH_FAR = 10.0f;       // Far enemies
    
    struct TubeSegment {
        uint8_t segmentId;           // Segment ID (0-15)
        glm::vec3 position;          // 3D position
        glm::vec3 normal;            // Surface normal
        glm::vec2 uv;                // Texture coordinates
        float depth;                 // Depth along tube
    };
    
    struct TubeVertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec4 color;
    };
    
    TubeGeometry();
    ~TubeGeometry() = default;
    
    // Initialization
    void initialize();
    void generateGeometry();
    
    // Position calculations
    glm::vec3 getSegmentPosition(uint8_t segment, float depth) const;
    glm::vec3 getSegmentNormal(uint8_t segment) const;
    float getSegmentAngle(uint8_t segment) const;
    
    // Depth and perspective
    float getDepthScale(float depth) const;
    float getPerspectiveScale(float depth) const;
    glm::mat4 getProjectionMatrix() const;
    
    // Collision detection
    bool isPointInTube(const glm::vec3& point) const;
    bool isSegmentVisible(uint8_t segment, float depth) const;
    
    // Geometry access
    const std::vector<TubeVertex>& getVertices() const { return vertices_; }
    const std::vector<uint32_t>& getIndices() const { return indices_; }
    size_t getVertexCount() const { return vertices_.size(); }
    size_t getIndexCount() const { return indices_.size(); }
    
    // Utility functions
    uint8_t getSegmentFromAngle(float angle) const;
    float getAngleFromSegment(uint8_t segment) const;
    float getDistanceBetweenSegments(uint8_t seg1, uint8_t seg2) const;
    
    // Configuration
    void setTubeRadius(float radius) { tubeRadius_ = radius; generateGeometry(); }
    void setTubeDepth(float depth) { tubeDepth_ = depth; generateGeometry(); }
    void setCameraDistance(float distance) { cameraDistance_ = distance; }
    
    float getTubeRadius() const { return tubeRadius_; }
    float getTubeDepth() const { return tubeDepth_; }
    float getCameraDistance() const { return cameraDistance_; }

private:
    // Tube parameters
    float tubeRadius_;
    float tubeDepth_;
    float cameraDistance_;
    
    // Geometry data
    std::vector<TubeVertex> vertices_;
    std::vector<uint32_t> indices_;
    
    // Cached calculations
    std::vector<TubeSegment> segments_;
    
    // Helper functions
    void generateSegments();
    void generateVertices();
    void generateIndices();
    glm::vec4 getSegmentColor(uint8_t segment, float depth) const;
};

/**
 * @brief Tube Collision System
 * 
 * Handles collision detection between entities and the tube geometry.
 */
class TubeCollisionSystem {
public:
    static bool checkTubeCollision(const glm::vec3& position, float radius);
    static bool checkSegmentCollision(uint8_t segment, float depth, float radius);
    static glm::vec3 getClosestPointOnTube(const glm::vec3& point);
    
private:
    static constexpr float TUBE_COLLISION_RADIUS = 1.2f;
    static constexpr float SEGMENT_COLLISION_THRESHOLD = 0.3f;
};

/**
 * @brief Tube Rendering System
 * 
 * Handles the rendering of the tube geometry with proper perspective and depth.
 */
class TubeRenderingSystem {
public:
    static void renderTube(const TubeGeometry& geometry);
    static void renderSegment(uint8_t segment, float depth, const glm::vec4& color);
    static void renderTubeWireframe(const TubeGeometry& geometry);
    
private:
    static constexpr float WIREFRAME_ALPHA = 0.3f;
    static constexpr glm::vec4 WIREFRAME_COLOR = {0.5f, 0.5f, 0.5f, WIREFRAME_ALPHA};
};

} // namespace Tempest 