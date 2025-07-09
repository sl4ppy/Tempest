#include "TubeGeometry.h"
#include <spdlog/spdlog.h>
#include <cmath>

namespace Tempest {

TubeGeometry::TubeGeometry() 
    : tubeRadius_(TUBE_RADIUS), tubeDepth_(TUBE_DEPTH), cameraDistance_(15.0f) {
    spdlog::info("TubeGeometry created");
}

void TubeGeometry::initialize() {
    generateGeometry();
    spdlog::info("TubeGeometry initialized with {} segments", NUM_SEGMENTS);
}

void TubeGeometry::generateGeometry() {
    generateSegments();
    generateVertices();
    generateIndices();
}

void TubeGeometry::generateSegments() {
    segments_.clear();
    segments_.reserve(NUM_SEGMENTS);
    
    for (uint8_t i = 0; i < NUM_SEGMENTS; ++i) {
        TubeSegment segment;
        segment.segmentId = i;
        segment.position = getSegmentPosition(i, 0.0f);
        segment.normal = getSegmentNormal(i);
        segment.uv = glm::vec2(static_cast<float>(i) / NUM_SEGMENTS, 0.0f);
        segment.depth = 0.0f;
        segments_.push_back(segment);
    }
}

void TubeGeometry::generateVertices() {
    vertices_.clear();
    
    // Generate vertices for each segment at different depths
    for (uint8_t segment = 0; segment < NUM_SEGMENTS; ++segment) {
        for (int depthLevel = 0; depthLevel <= 10; ++depthLevel) {
            float depth = static_cast<float>(depthLevel);
            glm::vec3 position = getSegmentPosition(segment, depth);
            glm::vec3 normal = getSegmentNormal(segment);
            glm::vec2 uv = glm::vec2(static_cast<float>(segment) / NUM_SEGMENTS, depth / tubeDepth_);
            glm::vec4 color = getSegmentColor(segment, depth);
            
            TubeVertex vertex;
            vertex.position = position;
            vertex.normal = normal;
            vertex.uv = uv;
            vertex.color = color;
            vertices_.push_back(vertex);
        }
    }
}

void TubeGeometry::generateIndices() {
    indices_.clear();
    
    // Generate triangle indices for tube surface
    for (uint8_t segment = 0; segment < NUM_SEGMENTS; ++segment) {
        for (int depthLevel = 0; depthLevel < 10; ++depthLevel) {
            uint32_t baseIndex = (segment * 11 + depthLevel) * 4;
            
            // First triangle
            indices_.push_back(baseIndex);
            indices_.push_back(baseIndex + 1);
            indices_.push_back(baseIndex + 2);
            
            // Second triangle
            indices_.push_back(baseIndex + 1);
            indices_.push_back(baseIndex + 3);
            indices_.push_back(baseIndex + 2);
        }
    }
}

glm::vec3 TubeGeometry::getSegmentPosition(uint8_t segment, float depth) const {
    float angle = getSegmentAngle(segment);
    float scale = getDepthScale(depth);
    
    return glm::vec3(
        cos(angle) * tubeRadius_ * scale,
        depth,
        sin(angle) * tubeRadius_ * scale
    );
}

glm::vec3 TubeGeometry::getSegmentNormal(uint8_t segment) const {
    float angle = getSegmentAngle(segment);
    return glm::vec3(cos(angle), 0.0f, sin(angle));
}

float TubeGeometry::getSegmentAngle(uint8_t segment) const {
    return static_cast<float>(segment) * SEGMENT_ANGLE;
}

float TubeGeometry::getDepthScale(float depth) const {
    // Constant radius - no scaling with depth as per original Tempest
    return 1.0f;
}

float TubeGeometry::getPerspectiveScale(float depth) const {
    // Perspective scaling based on camera distance
    return cameraDistance_ / (cameraDistance_ + depth);
}

glm::mat4 TubeGeometry::getProjectionMatrix() const {
    // Create a perspective projection matrix for the tube
    return glm::perspective(glm::radians(45.0f), 4.0f/3.0f, 0.1f, 100.0f);
}

bool TubeGeometry::isPointInTube(const glm::vec3& point) const {
    // Check if a point is within the tube bounds
    float distanceFromCenter = sqrt(point.x * point.x + point.z * point.z);
    return distanceFromCenter <= tubeRadius_ * 1.2f && point.y >= 0.0f && point.y <= tubeDepth_;
}

bool TubeGeometry::isSegmentVisible(uint8_t segment, float depth) const {
    // Check if a segment is visible from the camera position
    glm::vec3 segmentPos = getSegmentPosition(segment, depth);
    glm::vec3 cameraPos(0.0f, -cameraDistance_, 0.0f);
    
    // Simple visibility check - in a real implementation, this would be more sophisticated
    return glm::length(segmentPos - cameraPos) < cameraDistance_ * 2.0f;
}

uint8_t TubeGeometry::getSegmentFromAngle(float angle) const {
    // Normalize angle to [0, 2π)
    while (angle < 0.0f) angle += 2.0f * 3.14159f;
    while (angle >= 2.0f * 3.14159f) angle -= 2.0f * 3.14159f;
    
    return static_cast<uint8_t>(angle / SEGMENT_ANGLE) % NUM_SEGMENTS;
}

float TubeGeometry::getAngleFromSegment(uint8_t segment) const {
    return getSegmentAngle(segment);
}

float TubeGeometry::getDistanceBetweenSegments(uint8_t seg1, uint8_t seg2) const {
    float angle1 = getSegmentAngle(seg1);
    float angle2 = getSegmentAngle(seg2);
    
    float angleDiff = abs(angle2 - angle1);
    if (angleDiff > 3.14159f) {
        angleDiff = 2.0f * 3.14159f - angleDiff;
    }
    
    return angleDiff * tubeRadius_;
}

glm::vec4 TubeGeometry::getSegmentColor(uint8_t segment, float depth) const {
    // Generate a color based on segment and depth
    float segmentFactor = static_cast<float>(segment) / NUM_SEGMENTS;
    float depthFactor = depth / tubeDepth_;
    
    return glm::vec4(
        0.3f + 0.4f * segmentFactor,
        0.2f + 0.3f * depthFactor,
        0.5f + 0.2f * (1.0f - depthFactor),
        0.8f
    );
}

// TubeCollisionSystem implementation
bool TubeCollisionSystem::checkTubeCollision(const glm::vec3& position, float radius) {
    float distanceFromCenter = sqrt(position.x * position.x + position.z * position.z);
    return distanceFromCenter <= TUBE_COLLISION_RADIUS + radius;
}

bool TubeCollisionSystem::checkSegmentCollision(uint8_t segment, float depth, float radius) {
    // Check collision with a specific segment
    float angle = static_cast<float>(segment) * (2.0f * 3.14159f / TubeGeometry::NUM_SEGMENTS);
    glm::vec3 segmentPos(cos(angle), depth, sin(angle));
    
    // For now, use a simple distance check
    // In a real implementation, this would be more sophisticated
    return true; // Placeholder
}

glm::vec3 TubeCollisionSystem::getClosestPointOnTube(const glm::vec3& point) {
    // Find the closest point on the tube surface to the given point
    float distanceFromCenter = sqrt(point.x * point.x + point.z * point.z);
    float scale = TUBE_COLLISION_RADIUS / distanceFromCenter;
    
    return glm::vec3(
        point.x * scale,
        point.y,
        point.z * scale
    );
}

// TubeRenderingSystem implementation
void TubeRenderingSystem::renderTube(const TubeGeometry& geometry) {
    // TODO: Implement tube rendering using the graphics system
    // This will be implemented when we have the graphics system ready
    spdlog::debug("Rendering tube with {} vertices", geometry.getVertexCount());
}

void TubeRenderingSystem::renderSegment(uint8_t segment, float depth, const glm::vec4& color) {
    // TODO: Implement segment rendering
    // This will be implemented when we have the graphics system ready
    spdlog::debug("Rendering segment {} at depth {} with color ({}, {}, {}, {})", 
                  segment, depth, color.r, color.g, color.b, color.a);
}

void TubeRenderingSystem::renderTubeWireframe(const TubeGeometry& geometry) {
    // TODO: Implement wireframe rendering
    // This will be implemented when we have the graphics system ready
    spdlog::debug("Rendering tube wireframe");
}

} // namespace Tempest 