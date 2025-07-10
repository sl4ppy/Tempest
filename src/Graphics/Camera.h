#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Tempest {

enum class ProjectionType {
    Perspective,
    Orthographic
};

class Camera {
public:
    Camera();
    ~Camera() = default;
    
    // Camera setup
    void SetPosition(const glm::vec3& position);
    void SetTarget(const glm::vec3& target);
    void SetUp(const glm::vec3& up);
    void SetFOV(float fov);
    void SetAspectRatio(float aspectRatio);
    void SetNearPlane(float nearPlane);
    void SetFarPlane(float farPlane);
    void SetProjectionType(ProjectionType type);
    
    // Camera movement
    void Move(const glm::vec3& offset);
    void Rotate(float yaw, float pitch);
    void Orbit(const glm::vec3& center, float distance, float yaw, float pitch);
    void Zoom(float factor);
    
    // Matrix generation
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;
    
    // Getters
    glm::vec3 GetPosition() const { return m_position; }
    glm::vec3 GetTarget() const { return m_target; }
    glm::vec3 GetUp() const { return m_up; }
    glm::vec3 GetForward() const { return glm::normalize(m_target - m_position); }
    glm::vec3 GetRight() const { return glm::normalize(glm::cross(GetForward(), m_up)); }
    float GetFOV() const { return m_fov; }
    float GetAspectRatio() const { return m_aspectRatio; }
    float GetNearPlane() const { return m_nearPlane; }
    float GetFarPlane() const { return m_farPlane; }
    ProjectionType GetProjectionType() const { return m_projectionType; }
    
    // Utility methods
    void LookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
    void SetViewport(uint32_t width, uint32_t height);
    void UpdateAspectRatio(uint32_t width, uint32_t height);
    
    // Camera presets
    void SetDefaultPerspective();
    void SetDefaultOrthographic();
    void SetGameCamera();
    void SetDebugCamera();
    
private:
    // Camera properties
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    
    // Projection properties
    float m_fov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
    ProjectionType m_projectionType;
    
    // Orthographic properties
    float m_left, m_right, m_bottom, m_top;
    
    // Cached matrices
    mutable glm::mat4 m_viewMatrix;
    mutable glm::mat4 m_projectionMatrix;
    mutable bool m_viewMatrixDirty;
    mutable bool m_projectionMatrixDirty;
    
    // Helper methods
    void UpdateViewMatrix() const;
    void UpdateProjectionMatrix() const;
    void UpdateOrthographicBounds();
};

// Camera controller for game-specific camera behavior
class CameraController {
public:
    CameraController(Camera* camera = nullptr);
    ~CameraController() = default;
    
    void SetCamera(Camera* camera);
    void Update(float deltaTime);
    
    // Input handling
    void HandleMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    void HandleMouseScroll(float yOffset);
    void HandleKeyboard(float deltaTime);
    
    // Movement settings
    void SetMovementSpeed(float speed) { m_movementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }
    void SetZoomSpeed(float speed) { m_zoomSpeed = speed; }
    
    // Movement state
    void SetForwardPressed(bool pressed) { m_forwardPressed = pressed; }
    void SetBackwardPressed(bool pressed) { m_backwardPressed = pressed; }
    void SetLeftPressed(bool pressed) { m_leftPressed = pressed; }
    void SetRightPressed(bool pressed) { m_rightPressed = pressed; }
    void SetUpPressed(bool pressed) { m_upPressed = pressed; }
    void SetDownPressed(bool pressed) { m_downPressed = pressed; }
    
private:
    Camera* m_camera;
    
    // Movement settings
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoomSpeed;
    
    // Movement state
    bool m_forwardPressed;
    bool m_backwardPressed;
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_upPressed;
    bool m_downPressed;
    
    // Mouse state
    float m_lastX;
    float m_lastY;
    bool m_firstMouse;
    
    // Euler angles
    float m_yaw;
    float m_pitch;
};

} // namespace Tempest 