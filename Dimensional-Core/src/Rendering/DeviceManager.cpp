
#include <Rendering/DeviceManager.hpp>

/*
 * Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
License for glfw

Copyright (c) 2002-2006 Marcus Geelnard

Copyright (c) 2006-2019 Camilla Lowy

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would
   be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.
*/

// Modded from NVRHI Donut Framework Device Manager

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

Ref<DeviceManager> DeviceManager::Create(nvrhi::GraphicsAPI api)
{
    switch (api) {

    case nvrhi::GraphicsAPI::D3D11:
    case nvrhi::GraphicsAPI::D3D12:
    case nvrhi::GraphicsAPI::VULKAN:
        return Ref<DeviceManager>(DeviceManager::CreateVK());
        break;
    }
    return NULL;
}

void DeviceManager::Shutdown()
{

    m_SwapChainFramebuffers.clear();

    DestroyDeviceAndSwapChain();

    if (m_Window) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }

    glfwTerminate();

    m_InstanceCreated = false;
}

bool DeviceManager::CreateInstance(const InstanceParameters& params)
{
    if (m_InstanceCreated)
        return true;

    (InstanceParameters&)(m_DeviceParams) = params;

    if (!params.headlessDevice) {
#ifdef _WINDOWS
        if (!params.enablePerMonitorDPI) {
            // glfwInit enables the maximum supported level of DPI awareness unconditionally.
            // If the app doesn't need it, we have to call this function before glfwInit to override that behavior.
            SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
        }
#endif

        if (!glfwInit())
            return false;
    }

    m_InstanceCreated = CreateInstanceInternal();
    return m_InstanceCreated;
}

bool DeviceManager::CreateHeadlessDevice(const DeviceCreationParameters& params)
{
    m_DeviceParams = params;
    m_DeviceParams.headlessDevice = true;

    if (!CreateInstance(m_DeviceParams))
        return false;

    return CreateDevice();
}

bool DeviceManager::CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, const char* windowTitle)
{
    m_DeviceParams = params;
    m_DeviceParams.headlessDevice = false;
    m_RequestedVSync = params.vsyncEnabled;

    if (!CreateInstance(m_DeviceParams))
        return false;

    glfwDefaultWindowHints();

    bool foundFormat = false;
    for (const auto& info : formatInfo) {
        if (info.format == params.swapChainFormat) {
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

    assert(foundFormat);

    glfwWindowHint(GLFW_SAMPLES, params.swapChainSampleCount);
    glfwWindowHint(GLFW_REFRESH_RATE, params.refreshRate);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, params.resizeWindowWithDisplayScale);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Ignored for fullscreen

    if (params.startBorderless) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Borderless window
    }

    m_Window = glfwCreateWindow(params.backBufferWidth, params.backBufferHeight,
        windowTitle ? windowTitle : "",
        params.startFullscreen ? glfwGetPrimaryMonitor() : nullptr,
        nullptr);

    if (m_Window == nullptr) {
        return false;
    }

    if (params.startFullscreen) {
        glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0,
            m_DeviceParams.backBufferWidth, m_DeviceParams.backBufferHeight, m_DeviceParams.refreshRate);
    } else {
        int fbWidth = 0, fbHeight = 0;
        glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
        m_DeviceParams.backBufferWidth = fbWidth;
        m_DeviceParams.backBufferHeight = fbHeight;
    }

    if (windowTitle)
        m_WindowTitle = windowTitle;

    glfwSetWindowUserPointer(m_Window, this);

    // If there are multiple device managers, then this would be called by each one which isn't necessary
    // but should not hurt.

    if (!CreateDevice())
        return false;

    if (!CreateSwapChain())
        return false;

    glfwShowWindow(m_Window);

    if (m_DeviceParams.startMaximized) {
        glfwMaximizeWindow(m_Window);
    }

    // reset the back buffer size state to enforce a resize event
    m_DeviceParams.backBufferWidth = 0;
    m_DeviceParams.backBufferHeight = 0;

    UpdateWindowSize();

    return true;
}

void DeviceManager::BackBufferResizing()
{
    m_SwapChainFramebuffers.clear();
}

void DeviceManager::BackBufferResized()
{

    uint32_t backBufferCount = GetBackBufferCount();
    m_SwapChainFramebuffers.resize(backBufferCount);
    for (uint32_t index = 0; index < backBufferCount; index++) {
        m_SwapChainFramebuffers[index] = GetDevice()->createFramebuffer(
            nvrhi::FramebufferDesc().addColorAttachment(GetBackBuffer(index)));
    }
}

void DeviceManager::DisplayScaleChanged()
{
}

void DeviceManager::UpdateAverageFrameTime(double elapsedTime)
{
    m_FrameTimeSum += elapsedTime;
    m_NumberOfAccumulatedFrames += 1;

    if (m_FrameTimeSum > m_AverageTimeUpdateInterval && m_NumberOfAccumulatedFrames > 0) {
        m_AverageFrameTime = m_FrameTimeSum / double(m_NumberOfAccumulatedFrames);
        m_NumberOfAccumulatedFrames = 0;
        m_FrameTimeSum = 0.0;
    }
}

// TODO: Should Be in application
/*
void DeviceManager::RunMessageLoop()
{

    while (!glfwWindowShouldClose(m_Window)) {

        glfwPollEvents();
        UpdateWindowSize();

        bool presentSuccess = AnimateRenderPresent();

        if (!presentSuccess) {
            break;
        }
    }
    bool waitSuccess = GetDevice()->waitForIdle();
}
*/

void DeviceManager::Render()
{
    nvrhi::IFramebuffer* framebuffer = m_SwapChainFramebuffers[GetCurrentBackBufferIndex()];

    // TODO: Maybe have lamdas that can be submitted, and ran here (Take in a framebuffer)
}

bool DeviceManager::RenderPresent()
{
    double curTime = glfwGetTime();
    double elapsedTime = curTime - m_PreviousFrameTimestamp;

    if (m_windowVisible && m_windowIsInFocus) {
        if (m_PrevDPIScaleFactorX != m_DPIScaleFactorX || m_PrevDPIScaleFactorY != m_DPIScaleFactorY) {
            DisplayScaleChanged();
            m_PrevDPIScaleFactorX = m_DPIScaleFactorX;
            m_PrevDPIScaleFactorY = m_DPIScaleFactorY;
        }

        // normal rendering           : A0    R0 P0 A1 R1 P1
        // m_SkipRenderOnFirstFrame on: A0 A1 R0 P0 A2 R1 P1
        // m_SkipRenderOnFirstFrame simulates multi-threaded rendering frame indices, m_FrameIndex becomes the simulation index
        // while the local variable below becomes the render/present index, which will be different only if m_SkipRenderOnFirstFrame is set
        if (m_FrameIndex > 0 || !m_SkipRenderOnFirstFrame) {
            // if (BeginFrame()) {
            // first time entering this loop, m_FrameIndex is 1 for m_SkipRenderOnFirstFrame, 0 otherwise;
            uint32_t frameIndex = m_FrameIndex;

            if (m_SkipRenderOnFirstFrame) {
                frameIndex--;
            }

            Render(); // Should be where the rendering code is put

            bool presentSuccess = Present();
            if (!presentSuccess) {
                return false;
            }
            // }
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(0));

    GetDevice()->runGarbageCollection();

    UpdateAverageFrameTime(elapsedTime);
    m_PreviousFrameTimestamp = curTime;

    ++m_FrameIndex;
    return true;
}

nvrhi::IFramebuffer* DeviceManager::GetFramebuffer(uint32_t index)
{
    if (index < m_SwapChainFramebuffers.size())
        return m_SwapChainFramebuffers[index];

    return nullptr;
}

nvrhi::IFramebuffer* DeviceManager::GetCurrentFramebuffer()
{
    return GetFramebuffer(GetCurrentBackBufferIndex());
}

void DeviceManager::GetWindowDimensions(int& width, int& height)
{
    width = m_DeviceParams.backBufferWidth;
    height = m_DeviceParams.backBufferHeight;
}

const DeviceCreationParameters& DeviceManager::GetDeviceParams()
{
    return m_DeviceParams;
}

void DeviceManager::UpdateWindowSize()
{
    int width;
    int height;
    glfwGetWindowSize(m_Window, &width, &height);

    if (width == 0 || height == 0) {
        // window is minimized
        m_windowVisible = false;
        return;
    }

    m_windowVisible = true;

    m_windowIsInFocus = glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) == 1;

    if (int(m_DeviceParams.backBufferWidth) != width || int(m_DeviceParams.backBufferHeight) != height || (m_DeviceParams.vsyncEnabled != m_RequestedVSync && GetGraphicsAPI() == nvrhi::GraphicsAPI::VULKAN)) {
        // window is not minimized, and the size has changed

        BackBufferResizing();

        m_DeviceParams.backBufferWidth = width;
        m_DeviceParams.backBufferHeight = height;
        m_DeviceParams.vsyncEnabled = m_RequestedVSync;

        ResizeSwapChain();
        BackBufferResized();
    }

    m_DeviceParams.vsyncEnabled = m_RequestedVSync;
}

void DeviceManager::WindowPosCallback(int x, int y)
{
    if (m_DeviceParams.enablePerMonitorDPI) {
#ifdef _WINDOWS
        // Use Windows-specific implementation of DPI query because GLFW has issues:
        // glfwGetWindowMonitor(window) returns NULL for non-fullscreen applications.
        // This custom code allows us to adjust DPI scaling when a window is moved
        // between monitors with different scales.

        HWND hwnd = glfwGetWin32Window(m_Window);
        auto monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

        unsigned int dpiX;
        unsigned int dpiY;
        GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);

        m_DPIScaleFactorX = dpiX / 96.f;
        m_DPIScaleFactorY = dpiY / 96.f;
#else
        // Linux support for display scaling using GLFW.
        // This has limited utility due to the issue described above (NULL monitor),
        // and because GLFW doesn't support fractional scaling properly.
        // For example, on a system with 150% scaling it will report scale = 2.0
        // but the window will be either too small or too big, depending on 'resizeWindowWithDisplayScale'

        GLFWmonitor* monitor = glfwGetWindowMonitor(m_Window);

        // Non-fullscreen windows have NULL monitor, let's use the primary monitor in this case
        if (!monitor)
            monitor = glfwGetPrimaryMonitor();

        glfwGetMonitorContentScale(monitor, &m_DPIScaleFactorX, &m_DPIScaleFactorY);
#endif
    }

    if (m_EnableRenderDuringWindowMovement && m_SwapChainFramebuffers.size() > 0) {
        RenderPresent();
    }
}

}
