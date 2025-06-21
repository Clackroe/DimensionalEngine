#include "GLFW/glfw3.h"
#include "Log/log.hpp"
#include <Rendering/RenderDevice.hpp>
namespace Dimensional {

Ref<RenderDevice> RenderDevice::Create(nvrhi::GraphicsAPI api)
{
    switch (api) {
    case nvrhi::GraphicsAPI::D3D11:
    case nvrhi::GraphicsAPI::D3D12:
    case nvrhi::GraphicsAPI::VULKAN:
        return Ref<RenderDevice>(CreateVK());
        break;
    }
}

bool RenderDevice::InitializeHeadlessDevice(RenderDeviceCreateInfo& info)
{
    info.isHeadless = true;
    if (m_InstanceCreated) {
        return true;
    }

    m_InstanceCreated = CreateInstanceInternal();
    if (!m_InstanceCreated) {
        return false;
    }
    return CreateDevice();
}
bool RenderDevice::InitializeDevice(Ref<Window> window, RenderDeviceCreateInfo& info)
{
    info.isHeadless = false;
    if (m_InstanceCreated) {
        return true;
    }
    m_Window = window;

    m_InstanceCreated = CreateInstanceInternal();
    if (!m_InstanceCreated) {
        return false;
    }
    if (!CreateDevice()) {
        return false;
    }
    if (!CreateSwapChain()) {
        return false;
    }
    UpdateWindowSize();
    return true;
}
void RenderDevice::UpdateWindowSize()
{

    if (m_Window->GetWidth() != m_Width || m_Window->GetHeight() != m_Height) {
        BackBufferResizing();

        ResizeSwapChain();
        BackBufferResized();
    }
}

bool RenderDevice::BeginFrame()
{
    if (!BeginFrameInternal()) {
        return false;
    }
    return true;
}

bool RenderDevice::Present()
{
    if (!PresentInternal()) {
        return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(0));

    GetDevice()->runGarbageCollection();

    ++m_FrameIndex;
    return true;
}

void RenderDevice::Shutdown()
{
    m_SwapchainFramebuffers.clear();
    DestroyDeviceAndSwapChain();

    m_InstanceCreated = false;
}

nvrhi::IFramebuffer* RenderDevice::GetCurrentFramebuffer()
{
    return GetFramebuffer(GetCurrentBackBufferIndex());
}
nvrhi::IFramebuffer* RenderDevice::GetFramebuffer(uint32_t index)
{
    if (index < m_SwapchainFramebuffers.size())
        return m_SwapchainFramebuffers[index];

    return nullptr;
}

void RenderDevice::BackBufferResizing()
{
    m_SwapchainFramebuffers.clear();
}
void RenderDevice::BackBufferResized()
{
    uint32_t backBufferCount = GetBackBufferCount();
    m_SwapchainFramebuffers.resize(backBufferCount);
    for (uint32_t index = 0; index < backBufferCount; index++) {
        m_SwapchainFramebuffers[index] = GetDevice()->createFramebuffer(
            nvrhi::FramebufferDesc().addColorAttachment(GetBackBuffer(index)));
    }
}
}
