#include "Scene/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>

namespace MiniEngine {

glm::vec3 Camera::front() const {
    glm::vec3 f;
    const float yawR   = glm::radians(m_yaw);
    const float pitchR = glm::radians(m_pitch);
    f.x = std::cos(yawR) * std::cos(pitchR);
    f.y = std::sin(pitchR);
    f.z = std::sin(yawR) * std::cos(pitchR);
    return glm::normalize(f);
}

glm::vec3 Camera::right() const {
    return glm::normalize(glm::cross(front(), up()));
}

void Camera::addYaw(float delta) {
    m_yaw += delta;
}

void Camera::addPitch(float delta) {
    m_pitch += delta;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
}

void Camera::addFov(float delta) {
    m_fovDeg -= delta;
    m_fovDeg = std::clamp(m_fovDeg, 25.0f, 90.0f);
}

glm::mat4 Camera::viewMatrix() const {
    return glm::lookAt(m_position, m_position + front(), up());
}

glm::mat4 Camera::projectionMatrix(float aspectRatio, float nearPlane, float farPlane) const {
    return glm::perspective(glm::radians(m_fovDeg), aspectRatio, nearPlane, farPlane);
}

} // namespace MiniEngine
