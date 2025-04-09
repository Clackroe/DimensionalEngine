// #ifndef VULKAN_DEVICE
// #define VULKAN_DEVICE
//
// #include <vector>
// #include <vulkan/vulkan_core.h>
// #ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
// #define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
// #endif
// #include <vulkan/vulkan.hpp>
//
// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>
// #include <VkBootstrap.h>
// #include <nvrhi/nvrhi.h>
// #include <nvrhi/vulkan.h>
//
// namespace Dimensional {
//
// struct DeviceCreateProps {
//     u32 windowWidth = 1280;
//     std::string title = "Dimensional -- Default";
//     u32 windowHeight = 720;
//     bool enableValidation = true;
//     u32 maxFramesInFlight = 2;
//     bool vsync = false;
//     nvrhi::Format swapchainFormat = nvrhi::Format::SRGBA8_UNORM;
// };
//
// class VulkanDeviceManager {
// public:
//     bool initialize(DeviceCreateProps& props);
//     void shutdown();
//     void resize(uint32_t width, uint32_t height);
//
//     bool beginFrame();
//     bool endFrame();
//
//     nvrhi::IDevice* getNVRHIDevice() const
//     {
//         if (m_CreateProps.enableValidation) {
//             return m_ValidationDevice.Get();
//         }
//         return m_NvrhiDevice.Get();
//     }
//     GLFWwindow* getWindow() const { return m_Window; }
//     VkInstance getVkInstance() const { return m_Instance; }
//     VkDevice getVkDevice() const { return m_Device; }
//
// private:
//     bool initInstance(DeviceCreateProps& props);
//     bool initSurface();
//     bool initDevice();
//
//     bool createSwapchain(u32 width, u32 height);
//
//     bool initSwapchain(u32 width, u32 height);
//     void destroySwapchain();
//
// private:
//     DeviceCreateProps m_CreateProps;
//
//     GLFWwindow* m_Window = nullptr;
//
//     vk::Instance m_Instance = VK_NULL_HANDLE;
//     vk::PhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
//     vk::Device m_Device = VK_NULL_HANDLE;
//     VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
//     vk::SwapchainKHR m_Swapchain = VK_NULL_HANDLE;
//     vk::SurfaceFormatKHR m_SwapchainFormat {};
//
//     std::vector<VkImage> m_SwapchainImages;
//     std::vector<VkImageView> m_SwapchainImageViews;
//
//     struct SwapChainImage {
//         VkImage image;
//         nvrhi::TextureHandle nvrhiHandle;
//     };
//
//     std::vector<SwapChainImage> m_SwapChainImages;
//     u32 m_SwapchainImageIndex = 0;
//
//     std::vector<vk::Semaphore> m_AcquireSemaphores;
//     std::vector<vk::Semaphore> m_PresentSemaphores;
//     u32 m_AquireSemaphoreIndex = 0;
//     u32 m_PresentSemaphoreIndex = 0;
//
//     std::queue<nvrhi::EventQueryHandle> m_FramesInFlight;
//     std::vector<nvrhi::EventQueryHandle> m_QueryPool;
//
//     vk::Queue m_GraphicsQueue = VK_NULL_HANDLE;
//     vk::Queue m_ComputeQueue = VK_NULL_HANDLE;
//     vk::Queue m_TransferQueue = VK_NULL_HANDLE;
//     vk::Queue m_PresentQueue = VK_NULL_HANDLE;
//
//     vkb::Instance m_VkbInstance;
//     vkb::PhysicalDevice m_VkbPhysicalDevice;
//     vkb::Device m_VkbDevice;
//     vkb::Swapchain m_VkbSwapchain;
//
//     u32 m_FrameCounter = 0;
//
//     nvrhi::vulkan::DeviceHandle m_NvrhiDevice;
//     nvrhi::DeviceHandle m_ValidationDevice;
//     std::vector<nvrhi::FramebufferHandle> m_NvrhiFramebuffers;
//
// #if VK_HEADER_VERSION >= 301
//     typedef vk::detail::DynamicLoader VulkanDynamicLoader;
// #else
//     typedef vk::DynamicLoader VulkanDynamicLoader;
// #endif
//
//     Scope<VulkanDynamicLoader> m_dynamicLoader;
// };
// }
// #endif // VULKAN_DEVICE
