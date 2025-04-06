#include "Window.hpp"
#include <Event/EventSystem.hpp>
#include <Rendering/Renderer.hpp>
#include <glad.h>

#include "GLFW/glfw3.h"

namespace Dimensional {

Window::Window(const WindowSettings settings)
{
    glfwSwapInterval(1);
    m_Settings.Title = settings.Title;
    m_Settings.Width = settings.Width;
    m_Settings.Height = settings.Height;
    m_Settings.gApi = settings.gApi;

    initWindow(settings);
}

void Window::update()
{
    glfwSwapBuffers(m_Window);

    glfwPollEvents();
}

void Window::initWindow(const WindowSettings& settings)
{

    int glfwSuccess = glfwInit();
    DM_CORE_ASSERT(glfwSuccess, "Failed to initialize GLFW")

    Renderer::SetWindowHints();

    m_Window = glfwCreateWindow(settings.Width, settings.Height, settings.Title.c_str(), nullptr, nullptr);
    DM_CORE_ASSERT(m_Window, "Failed to create GLFW window. Terminating.");
    glfwMakeContextCurrent(m_Window);

    glfwSetWindowUserPointer(m_Window, &m_Settings);
    initCallbacks();
}

void Window::shutdown() { }

void Window::initCallbacks()
{
    glfwSetErrorCallback([](int error, const char* description) {
        DM_CORE_ERROR("GLFW Error {0}: {0}", error, description);
    });

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        WindowSettings& settings = *(WindowSettings*)glfwGetWindowUserPointer(window);
        settings.Height = height;
        settings.Width = width;
        glViewport(0, 0, width, height);
    });
}

void Window::setVsync(bool enabled)
{
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
    m_Settings.VSync = enabled;
}
}
