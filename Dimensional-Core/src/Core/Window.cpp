#include "Window.hpp"
#include <Event/EventSystem.hpp>
#include <Rendering/Renderer.hpp>
#include <vulkan/vulkan_core.h>
// #include <glad.h>

#include "GLFW/glfw3.h"

#include "Log/log.hpp"
#include "Rendering/DeviceManager.hpp"
#include "Rendering/Vulkan/VulkanDevice.hpp"
#include "core.hpp"
#include "nvrhi/nvrhi.h"
#include "nvrhi/utils.h"

namespace Dimensional {

static const struct
{
    nvrhi::Format format;
    uint32_t redBits;
    uint32_t greenBits;
    uint32_t blueBits;
    uint32_t alphaBits;
    uint32_t depthBits;
    uint32_t stencilBits;
} formatInfo[] = {
    {
        nvrhi::Format::UNKNOWN,
        0,
        0,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::R8_UINT,
        8,
        0,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RG8_UINT,
        8,
        8,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RG8_UNORM,
        8,
        8,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::R16_UINT,
        16,
        0,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::R16_UNORM,
        16,
        0,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::R16_FLOAT,
        16,
        0,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RGBA8_UNORM,
        8,
        8,
        8,
        8,
        0,
        0,
    },
    {
        nvrhi::Format::RGBA8_SNORM,
        8,
        8,
        8,
        8,
        0,
        0,
    },
    {
        nvrhi::Format::BGRA8_UNORM,
        8,
        8,
        8,
        8,
        0,
        0,
    },
    {
        nvrhi::Format::SRGBA8_UNORM,
        8,
        8,
        8,
        8,
        0,
        0,
    },
    {
        nvrhi::Format::SBGRA8_UNORM,
        8,
        8,
        8,
        8,
        0,
        0,
    },
    {
        nvrhi::Format::R10G10B10A2_UNORM,
        10,
        10,
        10,
        2,
        0,
        0,
    },
    {
        nvrhi::Format::R11G11B10_FLOAT,
        11,
        11,
        10,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RG16_UINT,
        16,
        16,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RG16_FLOAT,
        16,
        16,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::R32_UINT,
        32,
        0,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::R32_FLOAT,
        32,
        0,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RGBA16_FLOAT,
        16,
        16,
        16,
        16,
        0,
        0,
    },
    {
        nvrhi::Format::RGBA16_UNORM,
        16,
        16,
        16,
        16,
        0,
        0,
    },
    {
        nvrhi::Format::RGBA16_SNORM,
        16,
        16,
        16,
        16,
        0,
        0,
    },
    {
        nvrhi::Format::RG32_UINT,
        32,
        32,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RG32_FLOAT,
        32,
        32,
        0,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RGB32_UINT,
        32,
        32,
        32,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RGB32_FLOAT,
        32,
        32,
        32,
        0,
        0,
        0,
    },
    {
        nvrhi::Format::RGBA32_UINT,
        32,
        32,
        32,
        32,
        0,
        0,
    },
    {
        nvrhi::Format::RGBA32_FLOAT,
        32,
        32,
        32,
        32,
        0,
        0,
    },
};

static bool s_GLFWInitialized = false;

void Window::Initialize()
{
    DM_CORE_ASSERT(glfwInit(), "Failed to initialize GLFW!");
    s_GLFWInitialized = true;
}
void Window::Shutdown()
{
    if (s_GLFWInitialized) {
        glfwTerminate();
        s_GLFWInitialized = false;
    }
}

Ref<Window> Window::Create(const WindowSettings settings)
{
    Ref<Window> win = Ref<Window>(new Window);
    win->m_Settings = settings;
    if (!win->CreateWindow()) {
        DM_CORE_WARN("Failed to create Window: {}", settings.Title);
    }
    return win;
}

Window::~Window()
{
    if (m_Window) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
}

void Window::PollEvents()
{
    glfwPollEvents();
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);
    m_Settings.Width = (u32)width;
    m_Settings.Height = (u32)height;
}

void Window::EndFrame() { }

bool Window::IsVisible() const
{
    if (!m_Window)
        return false;
    return glfwGetWindowAttrib(m_Window, GLFW_VISIBLE) == GLFW_TRUE;
}
bool Window::IsInFocus() const
{
    if (!m_Window)
        return false;
    return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) == GLFW_TRUE;
}
bool Window::IsIconified() const
{
    if (!m_Window)
        return false;
    return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) == GLFW_TRUE;
}
bool Window::ShouldClose() const
{
    if (!m_Window)
        return true;
    return glfwWindowShouldClose(m_Window);
}

void Window::SetVSync(bool enabled)
{
    if (!m_Window)
        return;

    glfwSwapInterval(enabled ? 1 : 0);
    m_Settings.VSync = enabled;
}

void Window::SetSize(u32 width, u32 height)
{
    if (!m_Window)
        return;

    glfwSetWindowSize(m_Window, width, height);
    m_Settings.Width = width;
    m_Settings.Height = height;
}
void Window::SetTitle(const std::string& title)
{
    if (!m_Window)
        return;

    glfwSetWindowTitle(m_Window, title.c_str());
    m_Settings.Title = title;
}
void Window::SetFullscreen(bool fullscreen)
{
    if (!m_Window)
        return;

    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwSetWindowMonitor(m_Window, monitor, 0, 0,
            mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(m_Window, nullptr, 100, 100,
            m_Settings.Width,
            m_Settings.Height, 0);
    }

    m_Settings.startFullScreen = fullscreen;
}

void Window::SetBorderless(bool borderless)
{
    if (!m_Window)
        return;

    glfwSetWindowAttrib(m_Window, GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);
    m_Settings.startBorderless = borderless;
}
void Window::SetVisible(bool visible)
{
    if (!m_Window)
        return;

    if (visible) {
        glfwShowWindow(m_Window);
    } else {
        glfwHideWindow(m_Window);
    }
    m_Settings.visible = visible;
}

void Window::Close()
{
    if (!m_Window)
        return;
    glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
}

// Monitor/Display info
void Window::GetFramebufferSize(int& width, int& height) const
{
    if (!m_Window) {
        width = height = 0;
        return;
    }
    glfwGetFramebufferSize(m_Window, &width, &height);
}
void Window::GetMonitorInfo(int& width, int& height, int& refreshRate) const
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        width = height = refreshRate = 0;
        return;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
    refreshRate = mode->refreshRate;
}

// ===== PRIVATE =====
bool Window::CreateWindow()
{
    if (!s_GLFWInitialized) {
        Initialize();
    }

    ApplyWindowHints();

    m_Window = glfwCreateWindow(m_Settings.Width, m_Settings.Height, m_Settings.Title.c_str(), m_Settings.startFullScreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    glfwSetWindowUserPointer(m_Window, this);

    if (!m_Window) {
        DM_CORE_ERROR("Failed to create Window")
        return false;
    }

    if (m_Settings.startFullScreen) {
        SetFullscreen(true);
    } else {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
        m_Settings.Width = fbWidth;
        m_Settings.Height = fbHeight;
    }
    return true;
}

void Window::SetFormatHints(nvrhi::Format format)
{
    bool foundFormat = false;
    for (const auto& info : formatInfo) {
        if (info.format == format) {
            glfwWindowHint(GLFW_RED_BITS, info.redBits);
            glfwWindowHint(GLFW_GREEN_BITS, info.greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, info.blueBits);
            glfwWindowHint(GLFW_ALPHA_BITS, info.alphaBits);
            glfwWindowHint(GLFW_DEPTH_BITS, info.depthBits);
            glfwWindowHint(GLFW_STENCIL_BITS, info.stencilBits);
            foundFormat = true;
            break;
        }
    }
    DM_CORE_ASSERT(foundFormat, "Failed to find suitible format for window!!")
}

void Window::ApplyWindowHints()
{
    glfwDefaultWindowHints();

    glfwWindowHint(GLFW_SAMPLES, m_Settings.swapChainSamples);
    glfwWindowHint(GLFW_REFRESH_RATE, m_Settings.refreshRate);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_VISIBLE, m_Settings.visible ? GLFW_TRUE : GLFW_FALSE);
    if (m_Settings.startBorderless) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }
    glfwWindowHint(GLFW_RESIZABLE, m_Settings.resizeable ? GLFW_TRUE : GLFW_FALSE);
}

const char** Window::GetRequiredExtensions_VK(u32& count)
{
    return glfwGetRequiredInstanceExtensions(&count);
};

}
