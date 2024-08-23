#include "Core/Application.hpp"
#include "Event/EventSystem.hpp"
#include <Input/Input.hpp>

#include <GLFW/glfw3.h>
namespace Dimensional {

Input* Input::s_Instance = nullptr;

float Input::m_MouseX = 0.0f;
float Input::m_MouseY = 0.0f;

bool Input::IsKeyDown(KeyCode key)
{
    return glfwGetKey(Application::getApp().getWindowDM().getGLFWWindow(), (u32)key) == GLFW_PRESS;
};
bool Input::IsKeyReleased(KeyCode key)
{
    return glfwGetKey(Application::getApp().getWindowDM().getGLFWWindow(), (u32)key) == GLFW_RELEASE;
};

void Input::Init()
{
    if (s_Instance != nullptr) {
        DM_CORE_WARN("Input System already Initialized!!");
    }

    s_Instance = this;

    EventSystem::AddListener<MouseEvent>([](const Ref<MouseEvent>& e) {
        float x = e->getX();
        float y = e->getY();
        m_MouseX = x;
        m_MouseY = y;
    });

    DM_CORE_INFO("Input System Initialized");
}
}
