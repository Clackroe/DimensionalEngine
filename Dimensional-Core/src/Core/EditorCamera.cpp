#include "Input/Input.hpp"
#include "Input/KeyCodes.hpp"
#include "Log/log.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include <Core/EditorCamera.hpp>

#include <Core/Time.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Dimensional {
EditorCamera::EditorCamera(float FOV, float AR, float nearClipPlane, float farClipPlane)
    : Camera(glm::perspective(glm::radians(FOV), AR, nearClipPlane, farClipPlane))
{
    m_FOV = FOV;
    m_AspectRatio = AR;
    m_NearClipPlane = nearClipPlane;
    m_FarClipPlane = farClipPlane;

    m_LastMouseX = Input::getMouseX();
    m_LastMouseY = Input::getMouseY();

    updateViewValues();
}

void EditorCamera::Update()
{
    updateViewValues();
    if (Input::IsKeyDown(Key::Up)) {
        m_FOV -= 10 * Time::deltaTime();
    }
    if (Input::IsKeyDown(Key::Down)) {
        m_FOV += 10 * Time::deltaTime();
    }
    if (Input::IsKeyDown(Key::W)) {
        m_FocalPoint += getFwdDir() * 3.0f * Time::deltaTime() * m_Distance;
    }
    if (Input::IsKeyDown(Key::S)) {
        m_FocalPoint += -getFwdDir() * 3.0f * Time::deltaTime() * m_Distance;
    }

    if (Input::IsKeyDown(Key::A)) {
        m_FocalPoint += -getRightDir() * 3.0f * Time::deltaTime() * m_Distance;
    }
    if (Input::IsKeyDown(Key::D)) {
        m_FocalPoint += getRightDir() * 3.0f * Time::deltaTime() * m_Distance;
    }
    if (Input::IsKeyDown(Key::Space)) {
        m_FocalPoint += getUpDir() * 3.0f * Time::deltaTime() * m_Distance;
    }
    if (Input::IsKeyDown(Key::Left_control)) {
        m_FocalPoint += -getUpDir() * 3.0f * Time::deltaTime() * m_Distance;
    }

    float xpos = Input::getMouseX();
    float ypos = Input::getMouseY();
    if (Input::IsKeyDown(Key::Left_shift)) {

        float xoffset = xpos - m_LastMouseX;
        float yoffset = m_LastMouseY - ypos;

        float yawSign = getUpDir().y < 0 ? -1.0f : 1.0f;
        m_Yaw += yawSign * xoffset * 0.03f;
        m_Pitch -= yoffset * 0.03f;

        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }
    m_LastMouseX = xpos;
    m_LastMouseY = ypos;

    updateProjValues();
    updateViewValues();
}

void EditorCamera::updateProjValues()
{
    m_AspectRatio = m_ViewportW / m_ViewportH;
    m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClipPlane, m_FarClipPlane);
}

void EditorCamera::updateViewValues()
{
    m_Pos = calcPos();

    glm::quat orientation = getOrientation();
    m_ViewMtx = glm::translate(glm::mat4(1.0f), m_Pos) * glm::toMat4(orientation);
    m_ViewMtx = glm::inverse(m_ViewMtx);
}

glm::vec3 EditorCamera::getUpDir() const
{
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 EditorCamera::getRightDir() const
{
    return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}
glm::vec3 EditorCamera::getFwdDir() const
{
    return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}
glm::quat EditorCamera::getOrientation() const
{
    return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}
glm::vec3 EditorCamera::calcPos() const
{
    return m_FocalPoint - getFwdDir() * m_Distance;
}

}
