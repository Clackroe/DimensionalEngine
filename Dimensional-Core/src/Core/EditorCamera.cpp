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
    float deltaTime = Time::deltaTime();

    handleMovement(deltaTime);
    handleRotation(deltaTime);

    updateViewValues();
}

void EditorCamera::handleMovement(float deltaTime)
{
    float cameraSpeed = 10.0f * deltaTime; // Adjust speed as necessary

    glm::vec3 forward = getFwdDir();
    glm::vec3 right = getRightDir();
    glm::vec3 up = getUpDir();

    if (Input::IsKeyDown(Key::W)) {
        m_Position += forward * cameraSpeed;
    }
    if (Input::IsKeyDown(Key::S)) {
        m_Position -= forward * cameraSpeed;
    }
    if (Input::IsKeyDown(Key::A)) {
        m_Position -= right * cameraSpeed;
    }
    if (Input::IsKeyDown(Key::D)) {
        m_Position += right * cameraSpeed;
    }
    if (Input::IsKeyDown(Key::Q)) {
        m_Position -= up * cameraSpeed;
    }
    if (Input::IsKeyDown(Key::E)) {
        m_Position += up * cameraSpeed;
    }
}

void EditorCamera::handleRotation(float deltaTime)
{
    float sensitivity = 0.7f;

    float xPos, yPos;
    xPos = Input::getMouseX();
    yPos = Input::getMouseY();

    if (Input::IsKeyDown(Key::Left_shift)) {

        float deltaX = xPos - m_LastMouseX;
        float deltaY = yPos - m_LastMouseY;

        m_Yaw -= deltaX * sensitivity;
        m_Pitch -= deltaY * sensitivity;

        m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

        glm::quat pitchQuat = glm::angleAxis(glm::radians(m_Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yawQuat = glm::angleAxis(glm::radians(m_Yaw), glm::vec3(0.0f, 1.0f, 0.0f));

        m_Orientation = yawQuat * pitchQuat;
    }

    m_LastMouseX = xPos;
    m_LastMouseY = yPos;
}

void EditorCamera::updateProjValues()
{
    m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClipPlane, m_FarClipPlane);
}

void EditorCamera::updateViewValues()
{
    m_ViewMtx = glm::lookAt(m_Position, m_Position + getFwdDir(), getUpDir());
}

}
