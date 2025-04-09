#ifndef DEVICE_MANAGER
#define DEVICE_MANAGER

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

// Modified from Donut's DeviceManager: -- I just wanna render :( -- Now I can :)
// https://github.com/NVIDIAGameWorks/donut/blob/main/include/donut/app/DeviceManager.h

#define GLFW_INCLUDE_NONE // Do not include any OpenGL headers
#include <GLFW/glfw3.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // _WIN32
#include <GLFW/glfw3native.h>
#include <nvrhi/nvrhi.h>

#if DIMENSIONAL_VULKAN_RENDERER
#include <nvrhi/vulkan.h>
#endif

namespace Dimensional {

struct InstanceParameters {
    // ==== Instance ====
    bool enableDebugRuntime = false;
    bool enableWarningsAsErrors = false;
    bool enableGPUValidation = false; // Affects only DX12
    bool headlessDevice = false;

    bool logBufferLifetime = false;
    bool enableHeapDirectlyIndexed = false; // Allows ResourceDescriptorHeap on DX12
    bool resizeWindowWithDisplayScale = true;
    bool startBorderless = false;

    // Enables per-monitor DPI scale support.
    //
    // If set to true, the app will receive DisplayScaleChanged() events on DPI change and can read
    // the scaling factors using GetDPIScaleInfo(...). The window may be resized when DPI changes if
    // DeviceCreationParameters::resizeWindowWithDisplayScale is true.
    //
    // If set to false, the app will see DPI scaling factors being 1.0 all the time, but the OS
    // may scale the contents of the window based on DPI.
    //
    // This field is located in InstanceParameters and not DeviceCreationParameters because it is needed
    // in the CreateInstance() function to override the glfwInit() behavior.
    bool enablePerMonitorDPI = false;

    // Severity of the information log messages from the device manager, like the device name or enabled extensions.

#if DIMENSIONAL_VULKAN_RENDERER
    // Allows overriding the Vulkan library name with something custom, useful for Streamline
    std::string vulkanLibraryName;

    std::vector<std::string> requiredVulkanInstanceExtensions;
    std::vector<std::string> requiredVulkanLayers;
    std::vector<std::string> optionalVulkanInstanceExtensions;
    std::vector<std::string> optionalVulkanLayers;
#endif
};

struct DeviceCreationParameters : public InstanceParameters {

    //=== Device ===
    nvrhi::IMessageCallback* messageCallback = nullptr;

    std::vector<std::string> frameworkExtensions;

    bool startMaximized = false;
    bool startFullscreen = false;
    bool allowModeSwitch = true;

    uint32_t backBufferWidth = 1280;
    uint32_t backBufferHeight = 720;
    uint32_t refreshRate = 0;
    uint32_t swapChainBufferCount = 3;
    nvrhi::Format swapChainFormat = nvrhi::Format::SRGBA8_UNORM;
    uint32_t swapChainSampleCount = 1;
    uint32_t swapChainSampleQuality = 0;
    uint32_t maxFramesInFlight = 2;

    bool enableNvrhiValidationLayer = false;
    bool vsyncEnabled = true;
    bool enableRayTracingExtensions = false; // for vulkan
    bool enableComputeQueue = true;
    bool enableCopyQueue = true;

#if USE_DX11 || USE_DX12
    // Adapter to create the device on. Setting this to non-null overrides adapterNameSubstring.
    // If device creation fails on the specified adapter, it will *not* try any other adapters.
    IDXGIAdapter* adapter = nullptr;
#endif

    // For use in the case of multiple adapters; only effective if 'adapter' is null. If this is non-null, device creation will try to match
    // the given string against an adapter name.  If the specified string exists as a sub-string of the
    // adapter name, the device and window will be created on that adapter. Case sensitive.
    std::wstring adapterNameSubstring = L"";

#if USE_DX11 || USE_DX12
    DXGI_USAGE swapChainUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    Direct3DFeatureLevels::Type featureLevel = Direct3DFeatureLevels::L11_1;
#endif

#if USE_VK
    std::vector<std::string> requiredVulkanInstanceExtensions;
    std::vector<std::string> requiredVulkanDeviceExtensions;
    std::vector<std::string> requiredVulkanLayers;
    std::vector<std::string> optionalVulkanInstanceExtensions;
    std::vector<std::string> optionalVulkanDeviceExtensions;
    std::vector<std::string> optionalVulkanLayers;
    std::vector<size_t> ignoredVulkanValidationMessageLocations;
#endif
};

struct DeviceManager {

    static Ref<DeviceManager> Create(nvrhi::GraphicsAPI api);

    bool CreateInstance(const InstanceParameters& params);

    bool CreateHeadlessDevice(const DeviceCreationParameters& params);
    bool CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, const char* windowTitle);

    void UpdateWindowSize();

    void GetWindowDimensions(int& width, int& height);

    // returns the screen coordinate to pixel coordinate scale factor
    void GetDPIScaleInfo(float& x, float& y) const
    {
        x = m_DPIScaleFactorX;
        y = m_DPIScaleFactorY;
    }

    // TODO -- Move to Aplication / Renderer
    void Animate(double elapsedTime);
    void Render();
    void UpdateAverageFrameTime(double elapsedTime);
    bool RenderPresent();

    // == End TODO

protected:
    bool m_windowVisible = false;
    bool m_SkipRenderOnFirstFrame = false;
    bool m_windowIsInFocus = true;

    DeviceCreationParameters m_DeviceParams;
    GLFWwindow* m_Window = nullptr;
    bool m_EnableRenderDuringWindowMovement = false;

    bool m_IsNvidia = false;

    double m_PreviousFrameTimestamp = 0.0;
    // current DPI scale info (updated when window moves)
    float m_DPIScaleFactorX = 1.f;
    float m_DPIScaleFactorY = 1.f;
    float m_PrevDPIScaleFactorX = 0.f;
    float m_PrevDPIScaleFactorY = 0.f;
    bool m_RequestedVSync = false;
    bool m_InstanceCreated = false;

    double m_AverageFrameTime = 0.0;
    double m_AverageTimeUpdateInterval = 0.5;
    double m_FrameTimeSum = 0.0;
    int m_NumberOfAccumulatedFrames = 0;

    uint32_t m_FrameIndex = 0;

    std::vector<nvrhi::FramebufferHandle> m_SwapChainFramebuffers;

    void BackBufferResizing();
    void BackBufferResized();
    void DisplayScaleChanged();

    virtual bool CreateInstanceInternal() = 0;
    virtual bool CreateDevice() = 0;
    virtual bool CreateSwapChain() = 0;
    virtual void DestroyDeviceAndSwapChain() = 0;
    virtual void ResizeSwapChain() = 0;

public:
    virtual bool BeginFrame() = 0;
    virtual bool Present() = 0;

    [[nodiscard]] virtual nvrhi::IDevice* GetDevice() const = 0;
    [[nodiscard]] virtual const char* GetRendererString() const = 0;
    [[nodiscard]] virtual nvrhi::GraphicsAPI GetGraphicsAPI() const = 0;

    const DeviceCreationParameters& GetDeviceParams();
    [[nodiscard]] double GetAverageFrameTimeSeconds() const { return m_AverageFrameTime; }
    [[nodiscard]] double GetPreviousFrameTimestamp() const { return m_PreviousFrameTimestamp; }
    void SetFrameTimeUpdateInterval(double seconds) { m_AverageTimeUpdateInterval = seconds; }
    [[nodiscard]] bool IsVsyncEnabled() const { return m_DeviceParams.vsyncEnabled; }
    virtual void SetVsyncEnabled(bool enabled) { m_RequestedVSync = enabled; /* will be processed later */ }
    virtual void ReportLiveObjects() { }
    void SetEnableRenderDuringWindowMovement(bool val) { m_EnableRenderDuringWindowMovement = val; }

    void WindowPosCallback(int xpos, int ypos);

    [[nodiscard]] GLFWwindow* GetWindow() { return m_Window; }
    [[nodiscard]] uint32_t GetFrameIndex() const { return m_FrameIndex; }

    virtual nvrhi::ITexture* GetCurrentBackBuffer() = 0;
    virtual nvrhi::ITexture* GetBackBuffer(uint32_t index) = 0;
    virtual uint32_t GetCurrentBackBufferIndex() = 0;
    virtual uint32_t GetBackBufferCount() = 0;
    nvrhi::IFramebuffer* GetCurrentFramebuffer();
    nvrhi::IFramebuffer* GetFramebuffer(uint32_t index);

    virtual void Shutdown();
    virtual ~DeviceManager() = default;

    void SetWindowTitle(const char* title);
    void SetInformativeWindowTitle(const char* applicationName, bool includeFramerate = true, const char* extraInfo = nullptr);
    const char* GetWindowTitle();

    virtual bool IsVulkanInstanceExtensionEnabled(const char* extensionName) const { return false; }
    virtual bool IsVulkanDeviceExtensionEnabled(const char* extensionName) const { return false; }
    virtual bool IsVulkanLayerEnabled(const char* layerName) const { return false; }
    virtual void GetEnabledVulkanInstanceExtensions(std::vector<std::string>& extensions) const { }
    virtual void GetEnabledVulkanDeviceExtensions(std::vector<std::string>& extensions) const { }
    virtual void GetEnabledVulkanLayers(std::vector<std::string>& layers) const { }

private:
    static DeviceManager* CreateD3D11();
    static DeviceManager* CreateD3D12();
    static DeviceManager* CreateVK();

protected:
    std::string m_WindowTitle;
};

}
#endif // DEVICE_MANAGER
