#include "Window.hpp"
#include <Event/EventSystem.hpp>
#include <Rendering/Renderer.hpp>
// #include <glad.h>

#include "GLFW/glfw3.h"

#include "Log/log.hpp"
#include "Rendering/DeviceManager.hpp"
#include "core.hpp"
#include "nvrhi/nvrhi.h"
#include "nvrhi/utils.h"

namespace Dimensional {

Window::Window(const WindowSettings settings)
{
    m_Settings.Title = settings.Title;
    m_Settings.Width = settings.Width;
    m_Settings.Height = settings.Height;

    initWindow(settings);
}

nvrhi::GraphicsState graphicsState;
nvrhi::Color clearColor = nvrhi::Color(0.1f, 0.1f, 0.3f, 1.0f);

void Window::BeginFrame()
{
    glfwPollEvents();
    device->UpdateWindowSize();
    device->BeginFrame();

    // CommandList->open();
    // graphicsState.framebuffer = device->GetCurrentFramebuffer();
    // nvrhi::utils::ClearColorAttachment(CommandList, device->GetCurrentFramebuffer(), 0, clearColor);
}

int cnt = 0;
void Window::EndFrame()
{

    if (cnt % 60 == 0) {
        DM_CORE_INFO("AVG FrameTime: {}", device->GetAverageFrameTimeSeconds())
    }
    cnt += 1;
    // CommandList->close();
    // device->GetDevice()->executeCommandList(CommandList);

    bool succ = device->RenderPresent();
    if (!succ) {
        DM_CORE_ERROR("Failed to Present")
    }
    bool waitSuccess = device->GetDevice()->waitForIdle();
}

void Window::initWindow(const WindowSettings& settings)
{

    DM_CORE_ASSERT(glfwInit(), "Failed to initialize GLFW")

    // DeviceCreateProps props;
    // props.enableValidation = true;
    // props.windowHeight = settings.Height;
    // props.windowWidth = settings.Width;
    // props.maxFramesInFlight = 2;
    // props.title = settings.Title;

    device = DeviceManager::Create(nvrhi::GraphicsAPI::VULKAN);

    DeviceCreationParameters params;
    params.vsyncEnabled = true;
    params.enableNvrhiValidationLayer = true;
    device->CreateWindowDeviceAndSwapChain(params, "Test Title");

    CommandList = device->GetDevice()->createCommandList();

    // DM_CORE_ASSERT(vkd.initialize(props) == true, "Failed to initialize GraphicsDevice");
    m_Window = device->GetWindow();

    // Renderer::SetWindowHints();

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
        // glViewport(0, 0, width, height);
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
