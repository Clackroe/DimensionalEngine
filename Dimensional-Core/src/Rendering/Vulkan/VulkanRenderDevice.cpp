
#include "Rendering/Vulkan/VulkanRenderDevice.hpp"
#include "GLFW/glfw3.h"
#include "Log/log.hpp"
#include "nvrhi/validation.h"
#include <nvrhi/vulkan.h>
#include <vulkan/vulkan_core.h>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace Dimensional {

struct NVRHIMessageCallback : public nvrhi::IMessageCallback {
    virtual void message(nvrhi::MessageSeverity severity, const char* messageText) override
    {
        switch (severity) {
        case nvrhi::MessageSeverity::Info:
            DM_CORE_INFO("NVRHI: {}", messageText);
            break;
        case nvrhi::MessageSeverity::Warning:
            DM_CORE_WARN("NVRHI: {}", messageText);
            break;
        case nvrhi::MessageSeverity::Error:
        case nvrhi::MessageSeverity::Fatal:
            DM_CORE_ERROR("NVRHI: {}", messageText);
            break;
        }
    }
};

static NVRHIMessageCallback g_Callback;

static constexpr uint32_t kComputeQueueIndex = 0;
static constexpr uint32_t kGraphicsQueueIndex = 0;
static constexpr uint32_t kPresentQueueIndex = 0;
static constexpr uint32_t kTransferQueueIndex = 0;

RenderDevice* RenderDevice::CreateVK()
{
    return new VulkanRenderDevice();
}

static std::vector<const char*>
stringSetToVector(const std::unordered_set<std::string>& set)
{
    std::vector<const char*> ret;
    for (const auto& s : set) {
        ret.push_back(s.c_str());
    }

    return ret;
}

template <typename T>
static std::vector<T> setToVector(const std::unordered_set<T>& set)
{
    std::vector<T> ret;
    for (const auto& s : set) {
        ret.push_back(s);
    }

    return ret;
}

void VulkanRenderDevice::OnWindowResize(uint32_t width, uint32_t height) { }
void VulkanRenderDevice::SetVSync(bool enabled) { }

bool VulkanRenderDevice::CreateInstanceInternal()
{

    if (m_DeviceParams.enableDebugLayer) {
        enabledExtensions.instance.insert("VK_EXT_debug_report");
        enabledExtensions.layers.insert("VK_LAYER_KHRONOS_validation");
    }

    m_dynamicLoader = CreateScope<VulkanDynamicLoader>("");
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_dynamicLoader->getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    return createInstance();
}
#define CHECK(a)      \
    if (!(a)) {       \
        return false; \
    }

bool VulkanRenderDevice::CreateDevice()
{
    if (m_DeviceParams.enableDebugLayer) {
        installDebugCallback();
    }

    // add device extensions requested by the user
    // for (const std::string& name : m_DeviceParams.requiredVulkanDeviceExtensions) {
    //     enabledExtensions.device.insert(name);
    // }
    // for (const std::string& name : m_DeviceParams.optionalVulkanDeviceExtensions) {
    //     optionalExtensions.device.insert(name);
    // }

    if (!m_DeviceParams.isHeadless) {
        // Need to adjust the swap chain format before creating the device because it affects physical device selection
        if (m_DeviceParams.swapChainFormat == nvrhi::Format::SRGBA8_UNORM)
            m_DeviceParams.swapChainFormat = nvrhi::Format::SBGRA8_UNORM;
        else if (m_DeviceParams.swapChainFormat == nvrhi::Format::RGBA8_UNORM)
            m_DeviceParams.swapChainFormat = nvrhi::Format::BGRA8_UNORM;

        CHECK(createWindowSurface())
    }
    CHECK(pickPhysicalDevice())
    CHECK(findQueueFamilies(m_VulkanPhysicalDevice))
    CHECK(createDevice())

    auto vecInstanceExt = stringSetToVector(enabledExtensions.instance);
    auto vecLayers = stringSetToVector(enabledExtensions.layers);
    auto vecDeviceExt = stringSetToVector(enabledExtensions.device);

    nvrhi::vulkan::DeviceDesc deviceDesc;
    deviceDesc.errorCB = &g_Callback;
    deviceDesc.instance = m_VulkanInstance;
    deviceDesc.physicalDevice = m_VulkanPhysicalDevice;
    deviceDesc.device = m_VulkanDevice;
    deviceDesc.graphicsQueue = m_GraphicsQueue;
    deviceDesc.graphicsQueueIndex = m_GraphicsQueueFamily;
    // if (m_DeviceParams.enableComputeQueue) {
    deviceDesc.computeQueue = m_ComputeQueue;
    deviceDesc.computeQueueIndex = m_ComputeQueueFamily;
    // }
    // if (m_DeviceParams.enableCopyQueue) {
    deviceDesc.transferQueue = m_TransferQueue;
    deviceDesc.transferQueueIndex = m_TransferQueueFamily;
    // }
    deviceDesc.instanceExtensions = vecInstanceExt.data();
    deviceDesc.numInstanceExtensions = vecInstanceExt.size();
    deviceDesc.deviceExtensions = vecDeviceExt.data();
    deviceDesc.numDeviceExtensions = vecDeviceExt.size();
    deviceDesc.bufferDeviceAddressSupported = m_BufferDeviceAddressSupported;

    deviceDesc.vulkanLibraryName = "";
    // deviceDesc.logBufferLifetime = false;

    m_NvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

    if (m_DeviceParams.enableValidationLayer) {
        m_ValidationLayer = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
    }

    return true;
}
bool VulkanRenderDevice::CreateSwapChain()
{
    CHECK(createSwapChain())

    size_t const numPresentSemaphores = m_SwapChainImages.size();
    m_PresentSemaphores.reserve(numPresentSemaphores);
    for (uint32_t i = 0; i < numPresentSemaphores; ++i) {
        m_PresentSemaphores.push_back(m_VulkanDevice.createSemaphore(vk::SemaphoreCreateInfo()));
    }

    size_t const numAcquireSemaphores = std::max(size_t(m_DeviceParams.maxFramesInFlight),
        m_SwapChainImages.size());
    m_AcquireSemaphores.reserve(numAcquireSemaphores);
    for (uint32_t i = 0; i < numAcquireSemaphores; ++i) {
        m_AcquireSemaphores.push_back(m_VulkanDevice.createSemaphore(vk::SemaphoreCreateInfo()));
    }

    return true;
}

#undef CHECK
void VulkanRenderDevice::DestroyDeviceAndSwapChain()
{
    destroySwapChain();

    for (auto& semaphore : m_PresentSemaphores) {
        if (semaphore) {
            m_VulkanDevice.destroySemaphore(semaphore);
            semaphore = vk::Semaphore();
        }
    }

    for (auto& semaphore : m_AcquireSemaphores) {
        if (semaphore) {
            m_VulkanDevice.destroySemaphore(semaphore);
            semaphore = vk::Semaphore();
        }
    }

    m_NvrhiDevice = nullptr;
    m_ValidationLayer = nullptr;
    m_RendererString.clear();

    if (m_VulkanDevice) {
        m_VulkanDevice.destroy();
        m_VulkanDevice = nullptr;
    }

    if (m_WindowSurface) {
        assert(m_VulkanInstance);
        m_VulkanInstance.destroySurfaceKHR(m_WindowSurface);
        m_WindowSurface = nullptr;
    }

    if (m_DebugReportCallback) {
        m_VulkanInstance.destroyDebugReportCallbackEXT(m_DebugReportCallback);
    }

    if (m_VulkanInstance) {
        m_VulkanInstance.destroy();
        m_VulkanInstance = nullptr;
    }
}

bool VulkanRenderDevice::BeginFrameInternal()
{
    const auto& semaphore = m_AcquireSemaphores[m_AcquireSemaphoreIndex];

    vk::Result res;

    int const maxAttempts = 3;
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        res = m_VulkanDevice.acquireNextImageKHR(
            m_SwapChain,
            std::numeric_limits<uint64_t>::max(), // timeout
            semaphore,
            vk::Fence(),
            &m_SwapChainIndex);

        if ((res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR) && attempt < maxAttempts) {
            BackBufferResizing();
            auto surfaceCaps = m_VulkanPhysicalDevice.getSurfaceCapabilitiesKHR(m_WindowSurface);

            m_Width = surfaceCaps.currentExtent.width;
            m_Height = surfaceCaps.currentExtent.height;

            ResizeSwapChain();
            BackBufferResized();
        } else
            break;
    }

    m_AcquireSemaphoreIndex = (m_AcquireSemaphoreIndex + 1) % m_AcquireSemaphores.size();

    if (res == vk::Result::eSuccess || res == vk::Result::eSuboptimalKHR) // Suboptimal is considered a success
    {
        // Schedule the wait. The actual wait operation will be submitted when the app executes any command list.
        m_NvrhiDevice->queueWaitForSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);
        return true;
    }

    return false;
}
bool VulkanRenderDevice::PresentInternal()
{
    const auto& semaphore = m_PresentSemaphores[m_SwapChainIndex];

    m_NvrhiDevice->queueSignalSemaphore(nvrhi::CommandQueue::Graphics, semaphore, 0);

    // NVRHI buffers the semaphores and signals them when something is submitted to a queue.
    // Call 'executeCommandLists' with no command lists to actually signal the semaphore.
    m_NvrhiDevice->executeCommandLists(nullptr, 0);

    vk::PresentInfoKHR info = vk::PresentInfoKHR()
                                  .setWaitSemaphoreCount(1)
                                  .setPWaitSemaphores(&semaphore)
                                  .setSwapchainCount(1)
                                  .setPSwapchains(&m_SwapChain)
                                  .setPImageIndices(&m_SwapChainIndex);

    const vk::Result res = m_PresentQueue.presentKHR(&info);
    if (!(res == vk::Result::eSuccess || res == vk::Result::eErrorOutOfDateKHR || res == vk::Result::eSuboptimalKHR)) {
        return false;
    }

#ifndef _WIN32
    if (m_DeviceParams.vsyncEnabled || m_DeviceParams.enableDebugLayer) {
        // according to vulkan-tutorial.com, "the validation layer implementation expects
        // the application to explicitly synchronize with the GPU"
        m_PresentQueue.waitIdle();
    }
#endif

    while (m_FramesInFlight.size() >= m_DeviceParams.maxFramesInFlight) {
        auto query = m_FramesInFlight.front();
        m_FramesInFlight.pop();

        m_NvrhiDevice->waitEventQuery(query);

        m_QueryPool.push_back(query);
    }

    nvrhi::EventQueryHandle query;
    if (!m_QueryPool.empty()) {
        query = m_QueryPool.back();
        m_QueryPool.pop_back();
    } else {
        query = m_NvrhiDevice->createEventQuery();
    }

    m_NvrhiDevice->resetEventQuery(query);
    m_NvrhiDevice->setEventQuery(query, nvrhi::CommandQueue::Graphics);
    m_FramesInFlight.push(query);
    return true;
}

bool VulkanRenderDevice::createInstance()
{
    if (!m_DeviceParams.isHeadless) {

        // add any extensions required by GLFW
        uint32_t glfwExtCount;
        const char** glfwExt = m_Window->GetRequiredExtensions_VK(glfwExtCount);
        assert(glfwExt);

        for (uint32_t i = 0; i < glfwExtCount; i++) {
            enabledExtensions.instance.insert(std::string(glfwExt[i]));
        }
    }

    // add instance extensions requested by the user
    // for (const std::string& name : m_DeviceParams.requiredVulkanInstanceExtensions) {
    //     enabledExtensions.instance.insert(name);
    // }
    // for (const std::string& name : m_DeviceParams.optionalVulkanInstanceExtensions) {
    //     optionalExtensions.instance.insert(name);
    // }
    //
    // // add layers requested by the user
    // for (const std::string& name : m_DeviceParams.requiredVulkanLayers) {
    //     enabledExtensions.layers.insert(name);
    // }

    // for (const std::string& name : m_DeviceParams.optionalVulkanLayers) {
    //     optionalExtensions.layers.insert(name);
    // }

    std::unordered_set<std::string> requiredExtensions = enabledExtensions.instance;

    // figure out which optional extensions are supported
    for (const auto& instanceExt : vk::enumerateInstanceExtensionProperties()) {
        const std::string name = instanceExt.extensionName;
        if (optionalExtensions.instance.find(name) != optionalExtensions.instance.end()) {
            enabledExtensions.instance.insert(name);
        }

        requiredExtensions.erase(name);
    }

    if (!requiredExtensions.empty()) {
        DM_CORE_ERROR("Cannot create a Vulkan instance because the following required extension(s) are not supported:");
        for (const auto& ext : requiredExtensions) {
            DM_CORE_ERROR("\t-{}", ext);
        }
        return false;
    }

    DM_CORE_INFO("Enabled Vulkan instance extensions:");
    for (const auto& ext : enabledExtensions.instance) {
        DM_CORE_INFO("\t{}", ext.c_str());
    }

    std::unordered_set<std::string> requiredLayers = enabledExtensions.layers;

    for (const auto& layer : vk::enumerateInstanceLayerProperties()) {
        const std::string name = layer.layerName;
        if (optionalExtensions.layers.find(name) != optionalExtensions.layers.end()) {
            enabledExtensions.layers.insert(name);
        }

        requiredLayers.erase(name);
    }

    if (!requiredLayers.empty()) {
        std::stringstream ss;
        DM_CORE_ERROR("Cannot create a Vulkan instance because the following required layer(s) are not supported:");
        for (const auto& ext : requiredLayers) {
            DM_CORE_ERROR("\t- {}", ext);
        }
        return false;
    }

    DM_CORE_INFO("Enabled Vulkan layers:");
    for (const auto& layer : enabledExtensions.layers) {
        DM_CORE_INFO("\t{}", layer.c_str());
    }

    auto instanceExtVec = stringSetToVector(enabledExtensions.instance);
    auto layerVec = stringSetToVector(enabledExtensions.layers);

    auto applicationInfo = vk::ApplicationInfo();

    // Query the Vulkan API version supported on the system to make sure we use at least 1.3 when that's present.
    vk::Result res = vk::enumerateInstanceVersion(&applicationInfo.apiVersion);

    if (res != vk::Result::eSuccess) {
        DM_CORE_ERROR("Call to vkEnumerateInstanceVersion failed, error code = {}", nvrhi::vulkan::resultToString(VkResult(res)));
        return false;
    }

    const uint32_t minimumVulkanVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);

    // Check if the Vulkan API version is sufficient.
    if (applicationInfo.apiVersion < minimumVulkanVersion) {
        DM_CORE_ERROR("The Vulkan API version supported on the system (%d.%d.%d) is too low, at least %d.%d.%d is required.",
            VK_API_VERSION_MAJOR(applicationInfo.apiVersion), VK_API_VERSION_MINOR(applicationInfo.apiVersion), VK_API_VERSION_PATCH(applicationInfo.apiVersion),
            VK_API_VERSION_MAJOR(minimumVulkanVersion), VK_API_VERSION_MINOR(minimumVulkanVersion), VK_API_VERSION_PATCH(minimumVulkanVersion));
        return false;
    }

    // Spec says: A non-zero variant indicates the API is a variant of the Vulkan API and applications will typically need to be modified to run against it.
    if (VK_API_VERSION_VARIANT(applicationInfo.apiVersion) != 0) {
        DM_CORE_ERROR("The Vulkan API supported on the system uses an unexpected variant: %d.", VK_API_VERSION_VARIANT(applicationInfo.apiVersion));
        return false;
    }

    // Create the vulkan instance
    vk::InstanceCreateInfo info = vk::InstanceCreateInfo()
                                      .setEnabledLayerCount(uint32_t(layerVec.size()))
                                      .setPpEnabledLayerNames(layerVec.data())
                                      .setEnabledExtensionCount(uint32_t(instanceExtVec.size()))
                                      .setPpEnabledExtensionNames(instanceExtVec.data())
                                      .setPApplicationInfo(&applicationInfo);

    res = vk::createInstance(&info, nullptr, &m_VulkanInstance);
    if (res != vk::Result::eSuccess) {
        DM_CORE_ERROR("Failed to create a Vulkan instance, error code = {}", nvrhi::vulkan::resultToString(VkResult(res)));
        return false;
    }

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_VulkanInstance);

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
    vk::DebugReportFlagsEXT flags,
    vk::DebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{

    DM_CORE_WARN("[Vulkan: location=0x{0} code={1}, layerPrefix='{2}'] {3}", location, code, layerPrefix, msg);

    return VK_FALSE;
}
void VulkanRenderDevice::installDebugCallback()
{
    auto info = vk::DebugReportCallbackCreateInfoEXT()
                    .setFlags(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning |
                        //   vk::DebugReportFlagBitsEXT::eInformation |
                        vk::DebugReportFlagBitsEXT::ePerformanceWarning)
                    .setPfnCallback(vulkanDebugCallback)
                    .setPUserData(this);

    vk::Result res = m_VulkanInstance.createDebugReportCallbackEXT(&info, nullptr, &m_DebugReportCallback);
    assert(res == vk::Result::eSuccess);
}

bool VulkanRenderDevice::findQueueFamilies(vk::PhysicalDevice physicalDevice)
{
    auto props = physicalDevice.getQueueFamilyProperties();

    for (int i = 0; i < int(props.size()); i++) {
        const auto& queueFamily = props[i];

        if (m_GraphicsQueueFamily == -1) {
            if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)) {
                m_GraphicsQueueFamily = i;
            }
        }

        if (m_ComputeQueueFamily == -1) {
            if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) && !(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)) {
                m_ComputeQueueFamily = i;
            }
        }

        if (m_TransferQueueFamily == -1) {
            if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer) && !(queueFamily.queueFlags & vk::QueueFlagBits::eCompute) && !(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)) {
                m_TransferQueueFamily = i;
            }
        }

        if (m_PresentQueueFamily == -1) {
            if (queueFamily.queueCount > 0 && glfwGetPhysicalDevicePresentationSupport(m_VulkanInstance, physicalDevice, i)) {
                m_PresentQueueFamily = i;
            }
        }
    }

    if (m_GraphicsQueueFamily == -1 || (m_PresentQueueFamily == -1 && !m_DeviceParams.isHeadless)) {
        return false;
    }

    return true;
}

bool VulkanRenderDevice::pickPhysicalDevice()
{
    VkFormat requestedFormat = nvrhi::vulkan::convertFormat(m_DeviceParams.swapChainFormat);

    vk::Extent2D requestedExtent(m_Window->GetWidth(), m_Window->GetHeight());

    auto devices = m_VulkanInstance.enumeratePhysicalDevices();

    int adapterIndex = -1;

    int firstDevice = 0;
    int lastDevice = int(devices.size()) - 1;
    if (adapterIndex >= 0) {
        if (adapterIndex > lastDevice) {
            DM_CORE_ERROR("The specified Vulkan physical device %d does not exist.", adapterIndex);
            return false;
        }
        firstDevice = adapterIndex;
        lastDevice = adapterIndex;
    }

    // Start building an error message in case we cannot find a device.
    std::stringstream errorStream;
    errorStream << "Cannot find a Vulkan device that supports all the required extensions and properties.";

    // build a list of GPUs
    std::vector<vk::PhysicalDevice> discreteGPUs;
    std::vector<vk::PhysicalDevice> otherGPUs;
    for (int deviceIndex = firstDevice; deviceIndex <= lastDevice; ++deviceIndex) {
        vk::PhysicalDevice const& dev = devices[deviceIndex];
        vk::PhysicalDeviceProperties prop = dev.getProperties();

        errorStream << std::endl
                    << prop.deviceName.data() << ":";

        // check that all required device extensions are present
        std::unordered_set<std::string> requiredExtensions = enabledExtensions.device;
        auto deviceExtensions = dev.enumerateDeviceExtensionProperties();
        for (const auto& ext : deviceExtensions) {
            requiredExtensions.erase(std::string(ext.extensionName.data()));
        }

        bool deviceIsGood = true;

        if (!requiredExtensions.empty()) {
            // device is missing one or more required extensions
            for (const auto& ext : requiredExtensions) {
                errorStream << std::endl
                            << "  - missing " << ext;
            }
            deviceIsGood = false;
        }

        auto deviceFeatures = dev.getFeatures();
        if (!deviceFeatures.samplerAnisotropy) {
            // device is a toaster oven
            errorStream << std::endl
                        << "  - does not support samplerAnisotropy";
            deviceIsGood = false;
        }
        if (!deviceFeatures.textureCompressionBC) {
            errorStream << std::endl
                        << "  - does not support textureCompressionBC";
            deviceIsGood = false;
        }

        if (!findQueueFamilies(dev)) {
            // device doesn't have all the queue families we need
            errorStream << std::endl
                        << "  - does not support the necessary queue types";
            deviceIsGood = false;
        }

        if (deviceIsGood && m_WindowSurface) {
            bool surfaceSupported = dev.getSurfaceSupportKHR(m_PresentQueueFamily, m_WindowSurface);
            if (!surfaceSupported) {
                errorStream << std::endl
                            << "  - does not support the window surface";
                deviceIsGood = false;
            } else {
                // check that this device supports our intended swap chain creation parameters
                auto surfaceCaps = dev.getSurfaceCapabilitiesKHR(m_WindowSurface);
                auto surfaceFmts = dev.getSurfaceFormatsKHR(m_WindowSurface);

                if (surfaceCaps.minImageCount > m_DeviceParams.swapChainBufferCount || (surfaceCaps.maxImageCount < m_DeviceParams.swapChainBufferCount && surfaceCaps.maxImageCount > 0)) {
                    errorStream << std::endl
                                << "  - cannot support the requested swap chain image count:";
                    errorStream << " requested " << m_DeviceParams.swapChainBufferCount << ", available " << surfaceCaps.minImageCount << " - " << surfaceCaps.maxImageCount;
                    deviceIsGood = false;
                }

                if (surfaceCaps.minImageExtent.width > requestedExtent.width || surfaceCaps.minImageExtent.height > requestedExtent.height || surfaceCaps.maxImageExtent.width < requestedExtent.width || surfaceCaps.maxImageExtent.height < requestedExtent.height) {
                    errorStream << std::endl
                                << "  - cannot support the requested swap chain size:";
                    errorStream << " requested " << requestedExtent.width << "x" << requestedExtent.height << ", ";
                    errorStream << " available " << surfaceCaps.minImageExtent.width << "x" << surfaceCaps.minImageExtent.height;
                    errorStream << " - " << surfaceCaps.maxImageExtent.width << "x" << surfaceCaps.maxImageExtent.height;
                    deviceIsGood = false;
                }

                bool surfaceFormatPresent = false;
                for (const vk::SurfaceFormatKHR& surfaceFmt : surfaceFmts) {
                    if (surfaceFmt.format == vk::Format(requestedFormat)) {
                        surfaceFormatPresent = true;
                        break;
                    }
                }

                if (!surfaceFormatPresent) {
                    // can't create a swap chain using the format requested
                    errorStream << std::endl
                                << "  - does not support the requested swap chain format";
                    deviceIsGood = false;
                }

                // check that we can present from the graphics queue
                uint32_t canPresent = dev.getSurfaceSupportKHR(m_GraphicsQueueFamily, m_WindowSurface);
                if (!canPresent) {
                    errorStream << std::endl
                                << "  - cannot present";
                    deviceIsGood = false;
                }
            }
        }

        if (!deviceIsGood)
            continue;

        if (prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
            discreteGPUs.push_back(dev);
        } else {
            otherGPUs.push_back(dev);
        }
    }

    // pick the first discrete GPU if it exists, otherwise the first integrated GPU
    if (!discreteGPUs.empty()) {
        uint32_t selectedIndex = 0;

        m_VulkanPhysicalDevice = discreteGPUs[selectedIndex];
        return true;
    }

    if (!otherGPUs.empty()) {
        uint32_t selectedIndex = 0;
        m_VulkanPhysicalDevice = otherGPUs[selectedIndex];
        return true;
    }

    DM_CORE_ERROR("{}", errorStream.str().c_str());

    return false;
}

bool VulkanRenderDevice::createDevice()
{
    // figure out which optional extensions are supported
    auto deviceExtensions = m_VulkanPhysicalDevice.enumerateDeviceExtensionProperties();
    for (const auto& ext : deviceExtensions) {
        const std::string name = ext.extensionName;
        if (optionalExtensions.device.find(name) != optionalExtensions.device.end()) {
            if (name == VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME && m_DeviceParams.isHeadless)
                continue;

            enabledExtensions.device.insert(name);
        }

        if (m_DeviceParams.enableRayTracingSupport && m_RayTracingExtensions.find(name) != m_RayTracingExtensions.end()) {
            enabledExtensions.device.insert(name);
        }
    }

    if (!m_DeviceParams.isHeadless) {
        enabledExtensions.device.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    const vk::PhysicalDeviceProperties physicalDeviceProperties = m_VulkanPhysicalDevice.getProperties();
    m_RendererString = std::string(physicalDeviceProperties.deviceName.data());

    bool accelStructSupported = false;
    bool rayPipelineSupported = false;
    bool rayQuerySupported = false;
    bool meshletsSupported = false;
    bool vrsSupported = false;
    bool interlockSupported = false;
    bool barycentricSupported = false;
    bool storage16BitSupported = false;
    bool synchronization2Supported = false;
    bool maintenance4Supported = false;
    bool aftermathSupported = false;
    bool clusterAccelerationStructureSupported = false;
    bool mutableDescriptorTypeSupported = false;

    DM_CORE_INFO("Enabled Vulkan device extensions:");
    for (const auto& ext : enabledExtensions.device) {
        DM_CORE_INFO("\t{}", ext.c_str());

        if (ext == VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)
            accelStructSupported = true;
        else if (ext == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)
            rayPipelineSupported = true;
        else if (ext == VK_KHR_RAY_QUERY_EXTENSION_NAME)
            rayQuerySupported = true;
        else if (ext == VK_NV_MESH_SHADER_EXTENSION_NAME)
            meshletsSupported = true;
        else if (ext == VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)
            vrsSupported = true;
        else if (ext == VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME)
            interlockSupported = true;
        else if (ext == VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME)
            barycentricSupported = true;
        else if (ext == VK_KHR_16BIT_STORAGE_EXTENSION_NAME)
            storage16BitSupported = true;
        else if (ext == VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)
            synchronization2Supported = true;
        else if (ext == VK_KHR_MAINTENANCE_4_EXTENSION_NAME)
            maintenance4Supported = true;
        else if (ext == VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME)
            m_SwapChainMutableFormatSupported = true;
        else if (ext == VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME)
            aftermathSupported = true;
        else if (ext == VK_NV_CLUSTER_ACCELERATION_STRUCTURE_EXTENSION_NAME)
            clusterAccelerationStructureSupported = true;
        else if (ext == VK_EXT_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME)
            mutableDescriptorTypeSupported = true;
    }

#define APPEND_EXTENSION(condition, desc) \
    if (condition) {                      \
        (desc).pNext = pNext;             \
        pNext = &(desc);                  \
    } // NOLINT(cppcoreguidelines-macro-usage)
    void* pNext = nullptr;

    vk::PhysicalDeviceFeatures2 physicalDeviceFeatures2;
    // Determine support for Buffer Device Address, the Vulkan 1.2 way
    auto bufferDeviceAddressFeatures = vk::PhysicalDeviceBufferDeviceAddressFeatures();
    // Determine support for maintenance4
    auto maintenance4Features = vk::PhysicalDeviceMaintenance4Features();
    // Determine support for aftermath
    auto aftermathPhysicalFeatures = vk::PhysicalDeviceDiagnosticsConfigFeaturesNV();

    // Put the user-provided extension structure at the end of the chain
    // pNext = m_DeviceParams.physicalDeviceFeatures2Extensions;
    APPEND_EXTENSION(true, bufferDeviceAddressFeatures);
    APPEND_EXTENSION(maintenance4Supported, maintenance4Features);
    APPEND_EXTENSION(aftermathSupported, aftermathPhysicalFeatures);

    physicalDeviceFeatures2.pNext = pNext;
    m_VulkanPhysicalDevice.getFeatures2(&physicalDeviceFeatures2);

    std::unordered_set<int> uniqueQueueFamilies = {
        m_GraphicsQueueFamily
    };

    if (!m_DeviceParams.isHeadless)
        uniqueQueueFamilies.insert(m_PresentQueueFamily);

    uniqueQueueFamilies.insert(m_ComputeQueueFamily);

    uniqueQueueFamilies.insert(m_TransferQueueFamily);

    float priority = 1.f;
    std::vector<vk::DeviceQueueCreateInfo> queueDesc;
    queueDesc.reserve(uniqueQueueFamilies.size());
    for (int queueFamily : uniqueQueueFamilies) {
        queueDesc.push_back(vk::DeviceQueueCreateInfo()
                .setQueueFamilyIndex(queueFamily)
                .setQueueCount(1)
                .setPQueuePriorities(&priority));
    }

    auto accelStructFeatures = vk::PhysicalDeviceAccelerationStructureFeaturesKHR()
                                   .setAccelerationStructure(true);
    auto rayPipelineFeatures = vk::PhysicalDeviceRayTracingPipelineFeaturesKHR()
                                   .setRayTracingPipeline(true)
                                   .setRayTraversalPrimitiveCulling(true);
    auto rayQueryFeatures = vk::PhysicalDeviceRayQueryFeaturesKHR()
                                .setRayQuery(true);
    auto meshletFeatures = vk::PhysicalDeviceMeshShaderFeaturesNV()
                               .setTaskShader(true)
                               .setMeshShader(true);
    auto interlockFeatures = vk::PhysicalDeviceFragmentShaderInterlockFeaturesEXT()
                                 .setFragmentShaderPixelInterlock(true);
    auto barycentricFeatures = vk::PhysicalDeviceFragmentShaderBarycentricFeaturesKHR()
                                   .setFragmentShaderBarycentric(true);
    auto vrsFeatures = vk::PhysicalDeviceFragmentShadingRateFeaturesKHR()
                           .setPipelineFragmentShadingRate(true)
                           .setPrimitiveFragmentShadingRate(true)
                           .setAttachmentFragmentShadingRate(true);
    auto vulkan13features = vk::PhysicalDeviceVulkan13Features()
                                .setSynchronization2(synchronization2Supported)
                                .setMaintenance4(maintenance4Features.maintenance4);
    auto aftermathFeatures = vk::DeviceDiagnosticsConfigCreateInfoNV()
                                 .setFlags(vk::DeviceDiagnosticsConfigFlagBitsNV::eEnableResourceTracking
                                     | vk::DeviceDiagnosticsConfigFlagBitsNV::eEnableShaderDebugInfo
                                     | vk::DeviceDiagnosticsConfigFlagBitsNV::eEnableShaderErrorReporting);
    auto clusterAccelerationStructureFeatures = vk::PhysicalDeviceClusterAccelerationStructureFeaturesNV()
                                                    .setClusterAccelerationStructure(true);
    auto mutableDescriptorTypeFeatures = vk::PhysicalDeviceMutableDescriptorTypeFeaturesEXT()
                                             .setMutableDescriptorType(true);

    pNext = nullptr;
    APPEND_EXTENSION(accelStructSupported, accelStructFeatures)
    APPEND_EXTENSION(rayPipelineSupported, rayPipelineFeatures)
    APPEND_EXTENSION(rayQuerySupported, rayQueryFeatures)
    APPEND_EXTENSION(meshletsSupported, meshletFeatures)
    APPEND_EXTENSION(vrsSupported, vrsFeatures)
    APPEND_EXTENSION(interlockSupported, interlockFeatures)
    APPEND_EXTENSION(barycentricSupported, barycentricFeatures)
    APPEND_EXTENSION(clusterAccelerationStructureSupported, clusterAccelerationStructureFeatures)
    APPEND_EXTENSION(mutableDescriptorTypeSupported, mutableDescriptorTypeFeatures)
    APPEND_EXTENSION(physicalDeviceProperties.apiVersion >= VK_API_VERSION_1_3, vulkan13features)
    APPEND_EXTENSION(physicalDeviceProperties.apiVersion < VK_API_VERSION_1_3 && maintenance4Supported, maintenance4Features);

#if DONUT_WITH_AFTERMATH
    if (aftermathPhysicalFeatures.diagnosticsConfig && m_DeviceParams.enableAftermath)
        APPEND_EXTENSION(aftermathSupported, aftermathFeatures);
#endif
#undef APPEND_EXTENSION

    auto deviceFeatures = vk::PhysicalDeviceFeatures()
                              .setShaderImageGatherExtended(true)
                              .setSamplerAnisotropy(true)
                              .setTessellationShader(true)
                              .setTextureCompressionBC(true)
                              .setGeometryShader(true)
                              .setImageCubeArray(true)
                              .setShaderInt16(true)
                              .setFillModeNonSolid(true)
                              .setFragmentStoresAndAtomics(true)
                              .setDualSrcBlend(true)
                              .setVertexPipelineStoresAndAtomics(true)
                              .setShaderInt64(true)
                              .setShaderStorageImageWriteWithoutFormat(true)
                              .setShaderStorageImageReadWithoutFormat(true);

    // Add a Vulkan 1.1 structure with default settings to make it easier for apps to modify them
    auto vulkan11features = vk::PhysicalDeviceVulkan11Features()
                                .setStorageBuffer16BitAccess(true)
                                .setPNext(pNext);

    auto vulkan12features = vk::PhysicalDeviceVulkan12Features()
                                .setDescriptorIndexing(true)
                                .setRuntimeDescriptorArray(true)
                                .setDescriptorBindingPartiallyBound(true)
                                .setDescriptorBindingVariableDescriptorCount(true)
                                .setTimelineSemaphore(true)
                                .setShaderSampledImageArrayNonUniformIndexing(true)
                                .setBufferDeviceAddress(bufferDeviceAddressFeatures.bufferDeviceAddress)
                                .setShaderSubgroupExtendedTypes(true)
                                .setScalarBlockLayout(true)
                                .setPNext(&vulkan11features);

    auto layerVec = stringSetToVector(enabledExtensions.layers);
    auto extVec = stringSetToVector(enabledExtensions.device);

    auto deviceDesc = vk::DeviceCreateInfo()
                          .setPQueueCreateInfos(queueDesc.data())
                          .setQueueCreateInfoCount(uint32_t(queueDesc.size()))
                          .setPEnabledFeatures(&deviceFeatures)
                          .setEnabledExtensionCount(uint32_t(extVec.size()))
                          .setPpEnabledExtensionNames(extVec.data())
                          .setEnabledLayerCount(uint32_t(layerVec.size()))
                          .setPpEnabledLayerNames(layerVec.data())
                          .setPNext(&vulkan12features);

    // if (m_DeviceParams.deviceCreateInfoCallback)
    //     m_DeviceParams.deviceCreateInfoCallback(deviceDesc);

    const vk::Result res = m_VulkanPhysicalDevice.createDevice(&deviceDesc, nullptr, &m_VulkanDevice);
    if (res != vk::Result::eSuccess) {
        DM_CORE_ERROR("Failed to create a Vulkan physical device, error code = {}", nvrhi::vulkan::resultToString(VkResult(res)));
        return false;
    }

    m_VulkanDevice.getQueue(m_GraphicsQueueFamily, kGraphicsQueueIndex, &m_GraphicsQueue);
    m_VulkanDevice.getQueue(m_ComputeQueueFamily, kComputeQueueIndex, &m_ComputeQueue);
    m_VulkanDevice.getQueue(m_TransferQueueFamily, kTransferQueueIndex, &m_TransferQueue);
    m_VulkanDevice.getQueue(m_PresentQueueFamily, kPresentQueueIndex, &m_PresentQueue);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_VulkanDevice);

    // remember the bufferDeviceAddress feature enablement
    m_BufferDeviceAddressSupported = vulkan12features.bufferDeviceAddress;

    DM_CORE_INFO("Created Vulkan device: %s", m_RendererString.c_str());

    return true;
}

bool VulkanRenderDevice::createWindowSurface()
{
    const VkResult res = glfwCreateWindowSurface(m_VulkanInstance, m_Window->getGLFWWindow(), nullptr, (VkSurfaceKHR*)&m_WindowSurface);
    if (res != VK_SUCCESS) {
        DM_CORE_ERROR("Failed to create a GLFW window surface, error code = %s", nvrhi::vulkan::resultToString(res));
        return false;
    }

    return true;
}

bool VulkanRenderDevice::createSwapChain()
{
    destroySwapChain();

    m_SwapChainFormat = {
        vk::Format(nvrhi::vulkan::convertFormat(m_DeviceParams.swapChainFormat)),
        vk::ColorSpaceKHR::eSrgbNonlinear
    };

    vk::Extent2D extent = vk::Extent2D(m_Window->GetWidth(), m_Window->GetHeight());

    std::unordered_set<uint32_t> uniqueQueues = {
        uint32_t(m_GraphicsQueueFamily),
        uint32_t(m_PresentQueueFamily)
    };

    std::vector<uint32_t> queues = setToVector(uniqueQueues);

    const bool enableSwapChainSharing = queues.size() > 1;

    auto desc = vk::SwapchainCreateInfoKHR()
                    .setSurface(m_WindowSurface)
                    .setMinImageCount(m_DeviceParams.swapChainBufferCount)
                    .setImageFormat(m_SwapChainFormat.format)
                    .setImageColorSpace(m_SwapChainFormat.colorSpace)
                    .setImageExtent(extent)
                    .setImageArrayLayers(1)
                    .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
                    .setImageSharingMode(enableSwapChainSharing ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive)
                    .setFlags(m_SwapChainMutableFormatSupported ? vk::SwapchainCreateFlagBitsKHR::eMutableFormat : vk::SwapchainCreateFlagBitsKHR(0))
                    .setQueueFamilyIndexCount(enableSwapChainSharing ? uint32_t(queues.size()) : 0)
                    .setPQueueFamilyIndices(enableSwapChainSharing ? queues.data() : nullptr)
                    .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
                    .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
                    .setPresentMode(m_DeviceParams.vsyncEnabled ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eImmediate)
                    .setClipped(true)
                    .setOldSwapchain(nullptr);

    std::vector<vk::Format> imageFormats = { m_SwapChainFormat.format };
    switch (m_SwapChainFormat.format) {
    case vk::Format::eR8G8B8A8Unorm:
        imageFormats.push_back(vk::Format::eR8G8B8A8Srgb);
        break;
    case vk::Format::eR8G8B8A8Srgb:
        imageFormats.push_back(vk::Format::eR8G8B8A8Unorm);
        break;
    case vk::Format::eB8G8R8A8Unorm:
        imageFormats.push_back(vk::Format::eB8G8R8A8Srgb);
        break;
    case vk::Format::eB8G8R8A8Srgb:
        imageFormats.push_back(vk::Format::eB8G8R8A8Unorm);
        break;
    default:
        break;
    }

    auto imageFormatListCreateInfo = vk::ImageFormatListCreateInfo()
                                         .setViewFormats(imageFormats);

    if (m_SwapChainMutableFormatSupported)
        desc.pNext = &imageFormatListCreateInfo;

    const vk::Result res = m_VulkanDevice.createSwapchainKHR(&desc, nullptr, &m_SwapChain);
    if (res != vk::Result::eSuccess) {
        DM_CORE_ERROR("Failed to create a Vulkan swap chain, error code = {}", nvrhi::vulkan::resultToString(VkResult(res)));
        return false;
    }

    // retrieve swap chain images
    auto images = m_VulkanDevice.getSwapchainImagesKHR(m_SwapChain);
    for (auto image : images) {
        SwapChainImage sci;
        sci.image = image;

        nvrhi::TextureDesc textureDesc;
        textureDesc.width = m_Window->GetWidth();
        textureDesc.height = m_Window->GetHeight();
        textureDesc.format = m_DeviceParams.swapChainFormat;
        textureDesc.debugName = "Swap chain image";
        textureDesc.initialState = nvrhi::ResourceStates::Present;
        textureDesc.keepInitialState = true;
        textureDesc.isRenderTarget = true;

        sci.rhiHandle = m_NvrhiDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::VK_Image, nvrhi::Object(sci.image), textureDesc);
        m_SwapChainImages.push_back(sci);
    }

    m_SwapChainIndex = 0;

    return true;
}
void VulkanRenderDevice::destroySwapChain()
{
    if (m_VulkanDevice) {
        m_VulkanDevice.waitIdle();
    }

    if (m_SwapChain) {
        m_VulkanDevice.destroySwapchainKHR(m_SwapChain);
        m_SwapChain = nullptr;
    }

    m_SwapChainImages.clear();
}

}
