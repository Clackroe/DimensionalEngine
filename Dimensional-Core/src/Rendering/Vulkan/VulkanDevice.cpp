// #include "Rendering/Vulkan/VulkanDevice.hpp"
// #include "Log/log.hpp"
// #include "VkBootstrap.h"
// #include "nvrhi/nvrhi.h"
// #include <unordered_set>
// #include <vector>
// #include <vulkan/vulkan_enums.hpp>
//
// VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
//
// #include <nvrhi/validation.h>
// #include <nvrhi/vulkan.h>
// #include <vulkan/vulkan_core.h>
//
// std::vector<const char*> DeviceExtensions = {
//     "VK_KHR_swapchain",
//     VK_KHR_MAINTENANCE1_EXTENSION_NAME
// };
//
// std::vector<const char*> InstanceExtensions = { VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME };
//
// struct NVRHIMessageCallback : public nvrhi::IMessageCallback {
//     virtual void message(nvrhi::MessageSeverity severity, const char* messageText) override
//     {
//         switch (severity) {
//         case nvrhi::MessageSeverity::Info:
//             DM_CORE_INFO("NVRHI: {}", messageText);
//             break;
//         case nvrhi::MessageSeverity::Warning:
//             DM_CORE_WARN("NVRHI: {}", messageText);
//             break;
//         case nvrhi::MessageSeverity::Error:
//         case nvrhi::MessageSeverity::Fatal:
//             DM_CORE_ERROR("NVRHI: {}", messageText);
//             break;
//         }
//     }
// };
//
// static NVRHIMessageCallback g_Callback;
//
// namespace Dimensional {
//
// bool VulkanDeviceManager::beginFrame()
// {
//     const auto& semaphore = m_AcquireSemaphores[m_AquireSemaphoreIndex];
//
//     vk::Result res;
//
//     int const maxAttempts = 3;
//     for (int attempt = 0; attempt < maxAttempts; ++attempt) {
//         res = m_Device.acquireNextImageKHR(
//             m_Swapchain,
//             std::numeric_limits<u64>::max(), // timeout
//             semaphore,
//             vk::Fence(),
//             &m_SwapchainImageIndex);
//
//         if (res == vk::Result::eErrorOutOfDateKHR && attempt < maxAttempts) {
//             // BackBufferResizing();
//             auto surfaceCaps = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);
//
//             m_CreateProps.windowWidth = surfaceCaps.currentExtent.width;
//             m_CreateProps.windowHeight = surfaceCaps.currentExtent.height;
//
//             resize(m_CreateProps.windowWidth, m_CreateProps.windowHeight);
//             // ResizeSwapChain();
//             // BackBufferResized();
//         } else
//             break;
//     }
//
//     m_AquireSemaphoreIndex = (m_AquireSemaphoreIndex + 1) % m_AcquireSemaphores.size();
//
//     if (res == vk::Result::eSuccess || res == vk::Result::eSuboptimalKHR) {
//         m_NvrhiDevice->queueWaitForSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);
//         return true;
//     }
//
//     return false;
// }
// bool VulkanDeviceManager::endFrame()
// {
//     const auto& semaphore = m_PresentSemaphores[m_PresentSemaphoreIndex];
//
//     m_NvrhiDevice->queueSignalSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);
//
//     m_NvrhiDevice->executeCommandLists(nullptr, 0);
//
//     vk::PresentInfoKHR info = vk::PresentInfoKHR()
//                                   .setWaitSemaphoreCount(1)
//                                   .setPWaitSemaphores(&semaphore)
//                                   .setSwapchainCount(1)
//                                   .setPSwapchains(&m_Swapchain)
//                                   .setPImageIndices(&m_SwapchainImageIndex);
//
//     const vk::Result res = m_PresentQueue.presentKHR(&info);
//     if (!(res == vk::Result::eSuccess || res == vk::Result::eErrorOutOfDateKHR)) {
//         return false;
//     }
//
//     m_PresentSemaphoreIndex = (m_PresentSemaphoreIndex + 1) % m_PresentSemaphores.size();
//
// #ifndef _WIN32
//     if (m_CreateProps.vsync || m_CreateProps.enableValidation) {
//         // according to vulkan-tutorial.com, "the validation layer implementation expects
//         // the application to explicitly synchronize with the GPU"
//         m_PresentQueue.waitIdle();
//     }
// #endif
//
//     while (m_FramesInFlight.size() >= m_CreateProps.maxFramesInFlight) {
//         auto query = m_FramesInFlight.front();
//         m_FramesInFlight.pop();
//
//         m_NvrhiDevice->waitEventQuery(query);
//
//         m_QueryPool.push_back(query);
//     }
//
//     nvrhi::EventQueryHandle query;
//     if (!m_QueryPool.empty()) {
//         query = m_QueryPool.back();
//         m_QueryPool.pop_back();
//     } else {
//         query = m_NvrhiDevice->createEventQuery();
//     }
//
//     m_NvrhiDevice->resetEventQuery(query);
//     m_NvrhiDevice->setEventQuery(query, nvrhi::CommandQueue::Graphics);
//     m_FramesInFlight.push(query);
//     return true;
// }
//
// void VulkanDeviceManager::resize(uint32_t width, uint32_t height)
// {
//     if (m_Swapchain) {
//         destroySwapchain();
//         createSwapchain(width, height);
//     }
// }
//
// bool VulkanDeviceManager::initialize(DeviceCreateProps& props)
// {
//     m_CreateProps = props;
//
//     if (!initInstance(props))
//         return false;
//     if (!initSurface())
//         return false;
//     if (!initDevice())
//         return false;
//     if (!initSwapchain(props.windowWidth, props.windowHeight))
//         return false;
//
//     return true;
// }
//
// bool VulkanDeviceManager::initInstance(DeviceCreateProps& props)
// {
//     glfwInit();
//     glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//     m_Window = glfwCreateWindow(props.windowWidth, props.windowHeight, props.title.c_str(), nullptr, nullptr);
//
//     vkb::InstanceBuilder builder;
//     auto inst_ret = builder.set_app_name(props.title.c_str())
//                         .set_engine_name("Dimensional")
//                         .require_api_version(1, 3, 0)
//                         .enable_extensions(InstanceExtensions)
//                         .request_validation_layers(props.enableValidation)
//                         .use_default_debug_messenger()
//                         .build();
//
//     if (!inst_ret) {
//         DM_CORE_ERROR("Failed to create Vulkan instance");
//         return false;
//     }
//
//     m_VkbInstance = inst_ret.value();
//     m_Instance = m_VkbInstance.instance;
//
//     m_dynamicLoader = CreateScope<VulkanDynamicLoader>();
//     PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_dynamicLoader->getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
//     VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
//
//     VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);
//     return true;
// }
//
// bool VulkanDeviceManager::initSurface()
// {
//     if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface) != VK_SUCCESS) {
//         DM_CORE_ERROR("Failed to create GLFW surface");
//         return false;
//     }
//     return true;
// }
//
// bool VulkanDeviceManager::initDevice()
// {
//
//     // Create Vulkan Device
//     auto pdev_ret = vkb::PhysicalDeviceSelector(m_VkbInstance)
//                         .set_surface(m_Surface)
//                         .add_required_extensions(DeviceExtensions)
//                         .select();
//
//     if (!pdev_ret.has_value()) {
//         DM_CORE_ERROR("Failed to select physical device");
//         return false;
//     }
//
//     m_VkbPhysicalDevice = pdev_ret.value();
//     m_PhysicalDevice = m_VkbPhysicalDevice.physical_device;
//
//     auto dev_ret = vkb::DeviceBuilder(m_VkbPhysicalDevice).build();
//     if (!dev_ret.has_value()) {
//         DM_CORE_ERROR("Failed to create Vulkan device");
//         return false;
//     }
//
//     m_VkbDevice = dev_ret.value();
//     m_Device = m_VkbDevice.device;
//     VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device);
//
//     auto queue_ret = m_VkbDevice.get_queue(vkb::QueueType::graphics);
//     if (queue_ret) {
//         m_GraphicsQueue = queue_ret.value();
//     }
//     queue_ret = m_VkbDevice.get_queue(vkb::QueueType::compute);
//     if (queue_ret) {
//         m_ComputeQueue = queue_ret.value();
//     }
//     queue_ret = m_VkbDevice.get_queue(vkb::QueueType::transfer);
//     if (queue_ret) {
//         m_TransferQueue = queue_ret.value();
//     }
//     queue_ret = m_VkbDevice.get_queue(vkb::QueueType::present);
//     if (queue_ret) {
//         m_PresentQueue = queue_ret.value();
//     }
//
//     //=====
//
//     // Create NVRHI device
//     nvrhi::vulkan::DeviceDesc desc = {};
//     desc.instance = m_Instance;
//     desc.physicalDevice = m_PhysicalDevice;
//     desc.device = m_Device;
//
//     desc.graphicsQueue = m_GraphicsQueue;
//     desc.graphicsQueueIndex = m_VkbDevice.get_queue_index(vkb::QueueType::graphics).value();
//
//     if (m_ComputeQueue != VK_NULL_HANDLE) {
//         desc.computeQueue = m_ComputeQueue;
//         desc.computeQueueIndex = m_VkbDevice.get_queue_index(vkb::QueueType::compute).value();
//     }
//
//     if (m_TransferQueue != VK_NULL_HANDLE) {
//         desc.transferQueue = m_TransferQueue;
//         desc.transferQueueIndex = m_VkbDevice.get_queue_index(vkb::QueueType::transfer).value();
//     }
//
//     std::vector<const char*> extensions;
//     for (auto e : pdev_ret->get_extensions()) {
//         extensions.push_back(e.c_str());
//     }
//     desc.deviceExtensions = extensions.data();
//     desc.numDeviceExtensions = extensions.size();
//
//     desc.instanceExtensions = InstanceExtensions.data();
//     desc.numInstanceExtensions = InstanceExtensions.size();
//
//     desc.errorCB = &g_Callback;
//
//     m_NvrhiDevice = nvrhi::vulkan::createDevice(desc);
//
//     if (m_CreateProps.enableValidation) {
//         m_ValidationDevice = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
//     }
//
//     return true;
// }
//
// void VulkanDeviceManager::destroySwapchain()
// {
//     if (m_Device) {
//         m_Device.waitIdle();
//     }
//
//     if (m_Swapchain) {
//         m_Device.destroySwapchainKHR(m_Swapchain);
//         m_Swapchain = nullptr;
//     }
//
//     m_SwapChainImages.clear();
// }
//
// bool VulkanDeviceManager::createSwapchain(u32 width, u32 height)
// {
//     destroySwapchain();
//
//     vkb::SwapchainBuilder scBuilder { m_VkbPhysicalDevice, m_VkbDevice, m_Surface };
//     auto sc_ret = scBuilder
//                       .use_default_format_selection()
//                       .set_desired_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
//                       .set_desired_present_mode(m_CreateProps.vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR)
//                       .set_desired_extent(width, height)
//                       .add_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
//                       .set_pre_transform_flags(VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
//                       .set_composite_alpha_flags(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
//                       .set_old_swapchain(VK_NULL_HANDLE)
//                       .set_clipped(true)
//                       .set_image_array_layer_count(1)
//                       .build();
//
//     if (!sc_ret) {
//         DM_CORE_ERROR("Failed to create swapchain");
//         return false;
//     }
//     m_VkbSwapchain = sc_ret.value();
//     m_Swapchain = m_VkbSwapchain.swapchain;
//
//     VkExtent2D extent = m_VkbSwapchain.extent;
//
//     m_SwapchainFormat = {
//         m_VkbSwapchain.image_format,
//         m_VkbSwapchain.color_space
//     };
//
//     std::vector<vk::Format> imageFormats = { m_SwapchainFormat.format };
//
//     switch (m_SwapchainFormat.format) {
//     case vk::Format::eR8G8B8A8Unorm:
//         imageFormats.push_back(vk::Format::eR8G8B8A8Srgb);
//         break;
//     case vk::Format::eR8G8B8A8Srgb:
//         imageFormats.push_back(vk::Format::eR8G8B8A8Unorm);
//         break;
//     case vk::Format::eB8G8R8A8Unorm:
//         imageFormats.push_back(vk::Format::eB8G8R8A8Srgb);
//         break;
//     case vk::Format::eB8G8R8A8Srgb:
//         imageFormats.push_back(vk::Format::eB8G8R8A8Unorm);
//         break;
//     default:
//         break;
//     }
//
//     auto imageFormatListCreateInfo = vk::ImageFormatListCreateInfo()
//                                          .setViewFormats(imageFormats);
//
//     auto images = m_Device.getSwapchainImagesKHR(m_Swapchain);
//     for (auto image : images) {
//         SwapChainImage si;
//         si.image = image;
//
//         nvrhi::TextureDesc textureDesc;
//         textureDesc.width = m_CreateProps.windowWidth;
//         textureDesc.height = m_CreateProps.windowHeight;
//         textureDesc.format = m_CreateProps.swapchainFormat;
//         textureDesc.debugName = "Swap chain image";
//         textureDesc.initialState = nvrhi::ResourceStates::Present;
//         textureDesc.keepInitialState = true;
//         textureDesc.isRenderTarget = true;
//
//         si.nvrhiHandle = getNVRHIDevice()->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image, nvrhi::Object(si.image), textureDesc);
//         m_SwapChainImages.push_back(si);
//     }
//
//     m_SwapchainImageIndex = 0;
//     return true;
// }
//
// bool VulkanDeviceManager::initSwapchain(u32 width, u32 height)
// {
//     if (createSwapchain(width, height)) {
//         m_PresentSemaphores.reserve(m_CreateProps.maxFramesInFlight + 1);
//         m_AcquireSemaphores.reserve(m_CreateProps.maxFramesInFlight + 1);
//         for (uint32_t i = 0; i < m_CreateProps.maxFramesInFlight + 1; ++i) {
//             m_PresentSemaphores.push_back(m_Device.createSemaphore(vk::SemaphoreCreateInfo()));
//             m_AcquireSemaphores.push_back(m_Device.createSemaphore(vk::SemaphoreCreateInfo()));
//         }
//         return true;
//     }
//
//     return false;
// }
//
// void VulkanDeviceManager::shutdown()
// {
//     vkDeviceWaitIdle(m_Device);
//
//     m_NvrhiFramebuffers.clear();
//     m_NvrhiDevice = nullptr;
//
//     for (auto view : m_SwapchainImageViews)
//         vkDestroyImageView(m_Device, view, nullptr);
//
//     if (m_Swapchain)
//         vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
//
//     if (m_Surface)
//         vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
//
//     if (m_Device)
//         vkDestroyDevice(m_Device, nullptr);
//
//     if (m_Instance)
//         vkDestroyInstance(m_Instance, nullptr);
//
//     if (m_Window)
//         glfwDestroyWindow(m_Window);
//
//     glfwTerminate();
// }
// }
