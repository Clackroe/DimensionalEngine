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

    inline float getDistance() const { return m_Distance; };
    inline void setDistance(float dist) { m_Distance = dist; };

    inline void setViewportDimensions(float width, float height)
    {
        m_ViewportH = height;
        m_ViewportW = width;
        updateProjValues();
    }

    const glm::mat4 getViewMtx() { return m_ViewMtx; };
    glm::mat4 getViewProj() { return getProjection() * m_ViewMtx; }

    glm::vec3 getUpDir() const;
    glm::vec3 getRightDir() const;
    glm::vec3 getFwdDir() const;
    glm::quat getOrientation() const;
    glm::vec3 calcPos() const;

private:
    void updateProjValues();
    void updateViewValues();

    float m_FOV = 45.0f, m_AspectRatio = 16.0f / 9.0f, m_NearClipPlane = 0.1f, m_FarClipPlane = 1000.0f;

    glm::mat4 m_ViewMtx;
    glm::vec3 m_Pos = { 0.0f, 0.0f, -1.0f };
    glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };
    float m_Distance = 1.0f;
    float m_Pitch = 0.0f, m_Yaw = 0.0f;

    float m_LastMouseX, m_LastMouseY;

    float m_ViewportW = 1200, m_ViewportH = 720;
};
}

#endif
