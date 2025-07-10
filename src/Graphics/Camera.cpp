#include "Camera.h"
#include <spdlog/spdlog.h>

namespace Tempest {

Camera::Camera() 
    : m_position(0.0f, 0.0f, 3.0f), m_target(0.0f, 0.0f, 0.0f), m_up(0.0f, 1.0f, 0.0f),
      m_fov(45.0f), m_aspectRatio(16.0f/9.0f), m_nearPlane(0.1f), m_farPlane(100.0f),
      m_projectionType(ProjectionType::Perspective),
      m_left(-10.0f), m_right(10.0f), m_bottom(-10.0f), m_top(10.0f),
      m_viewMatrix(1.0f), m_projectionMatrix(1.0f),
      m_viewMatrixDirty(true), m_projectionMatrixDirty(true) {
}

void Camera::SetPosition(const glm::vec3& position) {
    m_position = position;
    m_viewMatrixDirty = true;
}

void Camera::SetTarget(const glm::vec3& target) {
    m_target = target;
    m_viewMatrixDirty = true;
}

void Camera::SetUp(const glm::vec3& up) {
    m_up = glm::normalize(up);
    m_viewMatrixDirty = true;
}

void Camera::SetFOV(float fov) {
    m_fov = fov;
    m_projectionMatrixDirty = true;
}

void Camera::SetAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
    m_projectionMatrixDirty = true;
}

void Camera::SetNearPlane(float nearPlane) {
    m_nearPlane = nearPlane;
    m_projectionMatrixDirty = true;
}

void Camera::SetFarPlane(float farPlane) {
    m_farPlane = farPlane;
    m_projectionMatrixDirty = true;
}

void Camera::SetProjectionType(ProjectionType type) {
    m_projectionType = type;
    m_projectionMatrixDirty = true;
}

void Camera::Move(const glm::vec3& offset) {
    m_position += offset;
    m_target += offset;
    m_viewMatrixDirty = true;
}

void Camera::Rotate(float yaw, float pitch) {
    // Convert to radians
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);
    
    // Calculate new direction
    glm::vec3 direction;
    direction.x = cos(yawRad) * cos(pitchRad);
    direction.y = sin(pitchRad);
    direction.z = sin(yawRad) * cos(pitchRad);
    
    m_target = m_position + glm::normalize(direction);
    m_viewMatrixDirty = true;
}

void Camera::Orbit(const glm::vec3& center, float distance, float yaw, float pitch) {
    // Convert to radians
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);
    
    // Calculate position
    m_position.x = center.x + distance * cos(yawRad) * cos(pitchRad);
    m_position.y = center.y + distance * sin(pitchRad);
    m_position.z = center.z + distance * sin(yawRad) * cos(pitchRad);
    
    m_target = center;
    m_viewMatrixDirty = true;
}

void Camera::Zoom(float factor) {
    if (m_projectionType == ProjectionType::Perspective) {
        m_fov = glm::clamp(m_fov * factor, 1.0f, 179.0f);
        m_projectionMatrixDirty = true;
    } else {
        // For orthographic, adjust the orthographic bounds
        float zoomFactor = 1.0f / factor;
        m_left *= zoomFactor;
        m_right *= zoomFactor;
        m_bottom *= zoomFactor;
        m_top *= zoomFactor;
        m_projectionMatrixDirty = true;
    }
}

glm::mat4 Camera::GetViewMatrix() const {
    if (m_viewMatrixDirty) {
        UpdateViewMatrix();
    }
    return m_viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() const {
    if (m_projectionMatrixDirty) {
        UpdateProjectionMatrix();
    }
    return m_projectionMatrix;
}

glm::mat4 Camera::GetViewProjectionMatrix() const {
    return GetProjectionMatrix() * GetViewMatrix();
}

void Camera::LookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    m_position = position;
    m_target = target;
    m_up = glm::normalize(up);
    m_viewMatrixDirty = true;
}

void Camera::SetViewport(uint32_t width, uint32_t height) {
    if (width > 0 && height > 0) {
        m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        m_projectionMatrixDirty = true;
    }
}

void Camera::UpdateAspectRatio(uint32_t width, uint32_t height) {
    SetViewport(width, height);
}

void Camera::SetDefaultPerspective() {
    m_projectionType = ProjectionType::Perspective;
    m_fov = 45.0f;
    m_aspectRatio = 16.0f / 9.0f;
    m_nearPlane = 0.1f;
    m_farPlane = 100.0f;
    m_projectionMatrixDirty = true;
}

void Camera::SetDefaultOrthographic() {
    m_projectionType = ProjectionType::Orthographic;
    m_left = -10.0f;
    m_right = 10.0f;
    m_bottom = -10.0f;
    m_top = 10.0f;
    m_nearPlane = -1.0f;
    m_farPlane = 1.0f;
    m_projectionMatrixDirty = true;
}

void Camera::SetGameCamera() {
    // Top-down view for Tempest game
    m_position = glm::vec3(0.0f, 10.0f, 0.0f);
    m_target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_up = glm::vec3(0.0f, 0.0f, -1.0f);
    m_projectionType = ProjectionType::Orthographic;
    m_left = -20.0f;
    m_right = 20.0f;
    m_bottom = -20.0f;
    m_top = 20.0f;
    m_nearPlane = -1.0f;
    m_farPlane = 1.0f;
    m_viewMatrixDirty = true;
    m_projectionMatrixDirty = true;
}

void Camera::SetDebugCamera() {
    // Free-look camera for debugging
    m_position = glm::vec3(0.0f, 5.0f, 10.0f);
    m_target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    m_projectionType = ProjectionType::Perspective;
    m_fov = 60.0f;
    m_aspectRatio = 16.0f / 9.0f;
    m_nearPlane = 0.1f;
    m_farPlane = 100.0f;
    m_viewMatrixDirty = true;
    m_projectionMatrixDirty = true;
}

// Private helper methods
void Camera::UpdateViewMatrix() const {
    m_viewMatrix = glm::lookAt(m_position, m_target, m_up);
    m_viewMatrixDirty = false;
}

void Camera::UpdateProjectionMatrix() const {
    if (m_projectionType == ProjectionType::Perspective) {
        m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
    } else {
        m_projectionMatrix = glm::ortho(m_left, m_right, m_bottom, m_top, m_nearPlane, m_farPlane);
    }
    m_projectionMatrixDirty = false;
}

void Camera::UpdateOrthographicBounds() {
    if (m_projectionType == ProjectionType::Orthographic) {
        float aspectRatio = m_aspectRatio;
        float halfHeight = 10.0f; // Default height
        float halfWidth = halfHeight * aspectRatio;
        
        m_left = -halfWidth;
        m_right = halfWidth;
        m_bottom = -halfHeight;
        m_top = halfHeight;
    }
}

// CameraController implementation
CameraController::CameraController(Camera* camera)
    : m_camera(camera), m_movementSpeed(2.5f), m_mouseSensitivity(0.1f), m_zoomSpeed(1.0f),
      m_forwardPressed(false), m_backwardPressed(false), m_leftPressed(false),
      m_rightPressed(false), m_upPressed(false), m_downPressed(false),
      m_lastX(0.0f), m_lastY(0.0f), m_firstMouse(true), m_yaw(-90.0f), m_pitch(0.0f) {
}

void CameraController::SetCamera(Camera* camera) {
    m_camera = camera;
}

void CameraController::Update(float deltaTime) {
    if (!m_camera) return;
    
    float velocity = m_movementSpeed * deltaTime;
    
    if (m_forwardPressed) {
        m_camera->Move(m_camera->GetForward() * velocity);
    }
    if (m_backwardPressed) {
        m_camera->Move(-m_camera->GetForward() * velocity);
    }
    if (m_leftPressed) {
        m_camera->Move(-m_camera->GetRight() * velocity);
    }
    if (m_rightPressed) {
        m_camera->Move(m_camera->GetRight() * velocity);
    }
    if (m_upPressed) {
        m_camera->Move(m_camera->GetUp() * velocity);
    }
    if (m_downPressed) {
        m_camera->Move(-m_camera->GetUp() * velocity);
    }
}

void CameraController::HandleMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    if (!m_camera) return;
    
    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;
    
    m_yaw += xOffset;
    m_pitch += yOffset;
    
    if (constrainPitch) {
        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
    }
    
    // Update camera target based on yaw and pitch
    glm::vec3 direction;
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    
    m_camera->SetTarget(m_camera->GetPosition() + glm::normalize(direction));
}

void CameraController::HandleMouseScroll(float yOffset) {
    if (!m_camera) return;
    
    m_camera->Zoom(1.0f + yOffset * m_zoomSpeed * 0.1f);
}

void CameraController::HandleKeyboard(float deltaTime) {
    // This method is called by Update() - keyboard state should be set externally
    // through the Set*Pressed methods
}

} // namespace Tempest 