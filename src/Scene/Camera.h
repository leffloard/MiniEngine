#pragma once

#include <glm/glm.hpp>

namespace MiniEngine {

class Camera {
public:
    glm::vec3 position() const { return m_position; }
    void      setPosition(glm::vec3 p) { m_position = p; }

    float yaw() const   { return m_yaw; }
    float pitch() const { return m_pitch; }
    void  setYaw(float y)   { m_yaw = y; }
    void  setPitch(float p) { m_pitch = p; }

    float fovDegrees() const { return m_fovDeg; }
    void  setFovDegrees(float fov) { m_fovDeg = fov; }

    glm::vec3 front() const;
    glm::vec3 right() const;
    glm::vec3 up() const { return glm::vec3(0.0f, 1.0f, 0.0f); }

    void addYaw(float delta);
    void addPitch(float delta);
    void addFov(float delta);

    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix(float aspectRatio, float nearPlane = 0.1f, float farPlane = 200.0f) const;

    float moveSpeed() const { return m_moveSpeed; }
    void  setMoveSpeed(float s) { m_moveSpeed = s; }
    float mouseSensitivity() const { return m_mouseSensitivity; }
    void  setMouseSensitivity(float s) { m_mouseSensitivity = s; }

private:
    glm::vec3 m_position = glm::vec3(0.0f, 1.2f, 4.0f);
    float     m_yaw      = -90.0f;
    float     m_pitch    = -10.0f;
    float     m_fovDeg   = 70.0f;
    float     m_moveSpeed       = 3.2f;
    float     m_mouseSensitivity = 0.07f;
};

} // namespace MiniEngine
