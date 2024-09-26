#include "Core/Application.hpp"
#include "Event/EventSystem.hpp"
#include <Input/Input.hpp>

#include <GLFW/glfw3.h>
namespace Dimensional {

bool Input::isKeyDown(KeyCode key)
{
    return getInstance().m_KeyStates[key];
};
bool Input::isKeyReleased(KeyCode key)
{
    return !getInstance().m_KeyStates[key];
};

float Input::getMouseX()
{
    return getInstance().m_MouseX;
}
float Input::getMouseY()
{
    return getInstance().m_MouseY;
}

//---

void Input::keyPressed(KeyCode key)
{
    m_KeyStates[key] = true;
}
void Input::keyReleased(KeyCode key)
{
    m_KeyStates[key] = false;
}
void Input::mouseMoved(float x, float y)
{
    m_MouseX = x;
    m_MouseY = y;
}

// ----
static void mouseMovedCallback(GLFWwindow* window, double xPos, double yPos)
{
    ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);

    ImGuiIO& io = ImGui::GetIO();
    // if (!io.WantCaptureMouse) {
    Input::getInstance().mouseMoved(xPos, yPos);
    // }
}

static void keyCallBack(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_KeyCallback(window, key, scanCode, action, mods);

    // if (io.WantCaptureKeyboard) {
    //     return;
    // }

    Mode mode = -1;
    switch (action) {
    case GLFW_REPEAT:
        mode = Key::REPEAT;
        break;
    case GLFW_PRESS:
        mode = Key::PRESS;
        Input::getInstance().keyPressed(static_cast<KeyCode>(key));
        break;
    case GLFW_RELEASE:
        mode = Key::RELEASE;
        Input::getInstance().keyReleased(static_cast<KeyCode>(key));
        break;
    default:
        DM_CORE_WARN("UNKNOWN KEY MODE/ACTION");
        break;
    }

    EventSystem::RaiseEvent<KeyEvent>((KeyCode)key, (Mode)mode);
};

// ----

void Input::Init()
{
    glfwSetKeyCallback(Application::getApp().getWindowDM().getGLFWWindow(), keyCallBack);
    glfwSetCursorPosCallback(Application::getApp().getWindowDM().getGLFWWindow(), mouseMovedCallback);

    DM_CORE_INFO("Input System Initialized");
}
}
