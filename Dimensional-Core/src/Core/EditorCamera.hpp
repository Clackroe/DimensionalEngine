#ifndef DM_EDITORCAM_H
#define DM_EDITORCAM_H
#include <Core/Camera.hpp>
#include <core.hpp>

namespace Dimensional {
class DMCORE_API EditorCamera : public Camera {
public:
    EditorCamera() = default;
    EditorCamera(float FOV, float AR, float nearClipPlane, float farClipPlane);

    void Update();

    inline float getFOV() const { return m_FOV; }
    inline void setFOV(float fov)
    {
        m_FOV = fov;
        updateProjValues();
    }

    void setViewportDimensions(float width, float height)
    {
        m_ViewportW = width;
        m_ViewportH = height;
        m_AspectRatio = width / height;
        updateProjValues();
    }

    const glm::mat4& getViewMtx() { return m_ViewMtx; }
    glm::mat4 getViewProj() { return getProjection() * m_ViewMtx; }

    glm::vec3 getUpDir() const { return m_Orientation * glm::vec3(0.0f, 1.0f, 0.0f); }
    glm::vec3 getRightDir() const { return m_Orientation * glm::vec3(1.0f, 0.0f, 0.0f); }
    glm::vec3 getFwdDir() const { return m_Orientation * glm::vec3(0.0f, 0.0f, -1.0f); }

    void setPosition(const glm::vec3& position)
    {
        m_Position = position;
        updateViewValues();
    }
    void setRotation(const glm::quat& rotation)
    {
        m_Orientation = rotation;
        updateViewValues();
    }
    void setRotationEuler(const glm::vec3& eulerAngles);

    const glm::vec3& getPosition() const { return m_Position; }
    const glm::quat& getRotation() const { return m_Orientation; }

private:
    void updateProjValues();
    void updateViewValues();

    void handleMovement(float deltaTime);
    void handleRotation(float deltaTime);

    float m_FOV = 45.0f, m_AspectRatio = 16.0f / 9.0f, m_NearClipPlane = 0.1f, m_FarClipPlane = 1000.0f;

    glm::mat4 m_ViewMtx;
    glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 5.0f);
    glm::quat m_Orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

    float m_Pitch = 0.0f, m_Yaw = 0.0f, m_Roll = 0.0f;

    float m_LastMouseX, m_LastMouseY;

    float m_ViewportW = 1200, m_ViewportH = 720;
};
}

#endif
