#ifndef DM_VULKAN_DEVICE_RENDERER
#define DM_VULKAN_DEVICE_RENDERER

#include "nvrhi/vulkan.h"
#include <Rendering/RenderDevice.hpp>

#include <vulkan/vulkan_core.h>
#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif
#include <vulkan/vulkan.hpp>

namespace Dimensional {

class VulkanRenderDevice : public RenderDevice {
public:
    ~VulkanRenderDevice() override = default;
    // void Shutdown() override;

    nvrhi::IDevice* GetDevice() const override
    {
        if (m_ValidationLayer)
            return m_ValidationLayer;

        return m_NvrhiDevice;
    }

    [[nodiscard]] nvrhi::GraphicsAPI GetGraphicsAPI() const override
    {
        return nvrhi::GraphicsAPI::VULKAN;
    }

    nvrhi::ITexture* GetCurrentBackBuffer() override
    {
        return m_SwapChainImages[m_SwapChainIndex].rhiHandle;
    }
    nvrhi::ITexture* GetBackBuffer(u32 index) override
    {
        if (index < m_SwapChainImages.size())
            return m_SwapChainImages[index].rhiHandle;
        return nullptr;
    }
    u32 GetCurrentBackBufferIndex() override
    {
        return m_SwapChainIndex;
    }
    u32 GetBackBufferCount() override
    {
        return u32(m_SwapChainImages.size());
    }

    void OnWindowResize(uint32_t width, uint32_t height) override;
    void SetVSync(bool enabled) override;

    const char* GetRendererString() const override
    {
        return m_RendererString.c_str();
    }

protected:
    void ResizeSwapChain() override
    {
        if (m_VulkanDevice) {
            destroySwapChain();
            createSwapChain();
        }
    }
    bool CreateInstanceInternal() override;
    bool CreateDevice() override;
    bool CreateSwapChain() override;
    void DestroyDeviceAndSwapChain() override;

    bool PresentInternal() override;
    bool BeginFrameInternal() override;

private:
    bool IsVulkanInstanceExtensionEnabled(const char* extensionName) const
    {
        return enabledExtensions.instance.find(extensionName) != enabledExtensions.instance.end();
    }

    bool IsVulkanDeviceExtensionEnabled(const char* extensionName) const
    {
        return enabledExtensions.device.find(extensionName) != enabledExtensions.device.end();
    }

    bool IsVulkanLayerEnabled(const char* layerName) const
    {
        return enabledExtensions.layers.find(layerName) != enabledExtensions.layers.end();
    }

    void GetEnabledVulkanInstanceExtensions(std::vector<std::string>& extensions) const
    {
        for (const auto& ext : enabledExtensions.instance)
            extensions.push_back(ext);
    }

    void GetEnabledVulkanDeviceExtensions(std::vector<std::string>& extensions) const
    {
        for (const auto& ext : enabledExtensions.device)
            extensions.push_back(ext);
    }

    void GetEnabledVulkanLayers(std::vector<std::string>& layers) const
    {
        for (const auto& ext : enabledExtensions.layers)
            layers.push_back(ext);
    }

    bool createInstance();
    bool createWindowSurface();

    void installDebugCallback();

    bool findQueueFamilies(vk::PhysicalDevice physicalDevice);
    bool pickPhysicalDevice();

    bool createDevice();
    bool createSwapChain();
    void destroySwapChain();

    struct VulkanExtensionSet {
        std::unordered_set<std::string> instance;
        std::unordered_set<std::string> layers;
        std::unordered_set<std::string> device;
    };

    VulkanExtensionSet enabledExtensions = {
        // instance
        {
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME },
        // layers
        {},
        // device
        {
            VK_KHR_MAINTENANCE1_EXTENSION_NAME },
    };

    // optional extensions
    VulkanExtensionSet optionalExtensions = {
        // instance
        {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
        },
        // layers
        {},
        // device
        {
            VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
            VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
            VK_KHR_MAINTENANCE_4_EXTENSION_NAME,
            VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME,
            VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
            VK_NV_MESH_SHADER_EXTENSION_NAME,
        },
    };

    std::unordered_set<std::string> m_RayTracingExtensions = {
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
        VK_KHR_RAY_QUERY_EXTENSION_NAME,
        VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
    };

    std::string m_RendererString;

    nvrhi::vulkan::DeviceHandle m_NvrhiDevice;
    nvrhi::DeviceHandle m_ValidationLayer;

    struct SwapChainImage {
        vk::Image image;
        nvrhi::TextureHandle rhiHandle;
    };
    std::vector<SwapChainImage> m_SwapChainImages;
    u32 m_SwapChainIndex = (u32)-1;

    vk::Device m_VulkanDevice;
    vk::Instance m_VulkanInstance;

    vk::PhysicalDevice m_VulkanPhysicalDevice;
    int m_GraphicsQueueFamily = -1;
    int m_ComputeQueueFamily = -1;
    int m_TransferQueueFamily = -1;
    int m_PresentQueueFamily = -1;

    vk::Queue m_GraphicsQueue;
    vk::Queue m_ComputeQueue;
    vk::Queue m_TransferQueue;
    vk::Queue m_PresentQueue;

    vk::SurfaceFormatKHR m_SwapChainFormat;
    vk::SwapchainKHR m_SwapChain;

    vk::SurfaceKHR m_WindowSurface;

    bool m_SwapChainMutableFormatSupported = false;
    bool m_BufferDeviceAddressSupported;

    std::vector<vk::Semaphore> m_AcquireSemaphores;
    std::vector<vk::Semaphore> m_PresentSemaphores;
    uint32_t m_AcquireSemaphoreIndex = 0;

    std::queue<nvrhi::EventQueryHandle> m_FramesInFlight;
    std::vector<nvrhi::EventQueryHandle> m_QueryPool;

    vk::DebugReportCallbackEXT m_DebugReportCallback;

#if VK_HEADER_VERSION >= 301
    typedef vk::detail::DynamicLoader VulkanDynamicLoader;
#else
    typedef vk::DynamicLoader VulkanDynamicLoader;
#endif

    std::unique_ptr<VulkanDynamicLoader> m_dynamicLoader;
};
}

#endif // DM_VULKAN_DEVICE_RENDERER
