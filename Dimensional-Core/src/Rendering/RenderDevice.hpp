#ifndef DM_RENDERDEVICE_HPP
#define DM_RENDERDEVICE_HPP

#include "Core/Window.hpp"
#include "nvrhi/nvrhi.h"
namespace Dimensional {

struct RenderDeviceCreateInfo {

    nvrhi::GraphicsAPI api = nvrhi::GraphicsAPI::VULKAN;

    bool enableDebugLayer = false;
    bool enableValidationLayer = false;
    uint32_t maxFramesInFlight = 2;
    uint32_t swapChainBufferCount = 3;
    nvrhi::Format swapChainFormat = nvrhi::Format::RGBA8_UNORM;

    bool isHeadless = false;
    bool vsyncEnabled = true;
    bool enableRayTracingSupport = false;
};

class RenderDevice {

public:
    static Ref<RenderDevice> Create(nvrhi::GraphicsAPI api);

    bool InitializeHeadlessDevice(RenderDeviceCreateInfo& info);
    bool InitializeDevice(Ref<Window> window, RenderDeviceCreateInfo& info);
    void UpdateWindowSize();

    bool Present();
    u32 GetFrameIndex() const { return m_FrameIndex; }
    nvrhi::IFramebuffer* GetCurrentFramebuffer();
    nvrhi::IFramebuffer* GetFramebuffer(uint32_t index);

    bool BeginFrame();

    void Shutdown();
    virtual nvrhi::IDevice* GetDevice() const = 0;

    virtual nvrhi::ITexture* GetCurrentBackBuffer() = 0;
    virtual nvrhi::ITexture* GetBackBuffer(uint32_t index) = 0;
    virtual u32 GetCurrentBackBufferIndex() = 0;
    virtual u32 GetBackBufferCount() = 0;

    virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;
    virtual void SetVSync(bool enabled) = 0;

    virtual const char* GetRendererString() const = 0;
    virtual nvrhi::GraphicsAPI GetGraphicsAPI() const = 0;

    RenderDeviceCreateInfo GetCreateInfo() const { return m_DeviceParams; }

    virtual ~RenderDevice() = default;
    RenderDevice() = default;

protected:
    Ref<Window> m_Window = nullptr;

    RenderDeviceCreateInfo m_DeviceParams;
    bool m_InstanceCreated = false;
    u32 m_FrameIndex = 0;

    std::vector<nvrhi::FramebufferHandle> m_SwapchainFramebuffers;

    void BackBufferResizing();
    void BackBufferResized();

    virtual bool CreateInstanceInternal() = 0;
    virtual bool CreateDevice() = 0;
    virtual bool CreateSwapChain() = 0;
    virtual void DestroyDeviceAndSwapChain() = 0;
    virtual void ResizeSwapChain() = 0;

    virtual bool PresentInternal() = 0;
    virtual bool BeginFrameInternal() = 0;

    u32 m_Width, m_Height;

private:
    static RenderDevice* CreateVK();
};
}

#endif // DM_RENDERDEVICE_HPP
