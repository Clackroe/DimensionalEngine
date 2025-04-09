#include "Log/log.hpp"
#include "VkBootstrap.h"
#include "nvrhi/validation.h"
#include <Rendering/Vulkan/VulkanDeviceManager.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>

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

static std::vector<const char*> stringSetToVector(const std::unordered_set<std::string>& set)
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

bool DeviceManager_VK::createInstance()
{
    if (!m_DeviceParams.headlessDevice) {
        if (!glfwVulkanSupported()) {
            DM_CORE_ERROR("GLFW reports that Vulkan is not supported. Perhaps missing a call to glfwInit()?");
            return false;
        }

        // add any extensions required by GLFW
        u32 glfwExtCount;
        const char** glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);
        assert(glfwExt);

        for (u32 i = 0; i < glfwExtCount; i++) {
            enabledExtensions.instance.insert(std::string(glfwExt[i]));
        }
    }

    // add instance extensions requested by the user
    for (const std::string& name : m_DeviceParams.requiredVulkanInstanceExtensions) {
        enabledExtensions.instance.insert(name);
    }
    for (const std::string& name : m_DeviceParams.optionalVulkanInstanceExtensions) {
        optionalExtensions.instance.insert(name);
    }

    // add layers requested by the user
    for (const std::string& name : m_DeviceParams.requiredVulkanLayers) {
        enabledExtensions.layers.insert(name);
    }
    for (const std::string& name : m_DeviceParams.optionalVulkanLayers) {
        optionalExtensions.layers.insert(name);
    }

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
        std::stringstream ss;
        ss << "Cannot create a Vulkan instance because the following required extension(s) are not supported:";
        for (const auto& ext : requiredExtensions)
            ss << std::endl
               << "  - " << ext;

        DM_CORE_ERROR("{}", ss.str().c_str());
        return false;
    }

    DM_CORE_INFO("Enabled Vulkan instance extensions:");
    for (const auto& ext : enabledExtensions.instance) {
        DM_CORE_INFO("    {}", ext.c_str());
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
        ss << "Cannot create a Vulkan instance because the following required layer(s) are not supported:";
        for (const auto& ext : requiredLayers)
            ss << std::endl
               << "  - " << ext;

        DM_CORE_ERROR("{}", ss.str().c_str());
        return false;
    }

    DM_CORE_INFO("Enabled Vulkan layers:");
    for (const auto& layer : enabledExtensions.layers) {
        DM_CORE_INFO("    {}", layer.c_str());
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
    const u32 minimumVulkanVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);

    // Check if the Vulkan API version is sufficient.
    if (applicationInfo.apiVersion < minimumVulkanVersion) {
        DM_CORE_ERROR("The Vulkan API version supported on the system ({0}.{1}.{2}) is too low, at least %d.%d.%d is required.",
            VK_API_VERSION_MAJOR(applicationInfo.apiVersion), VK_API_VERSION_MINOR(applicationInfo.apiVersion), VK_API_VERSION_PATCH(applicationInfo.apiVersion),
            VK_API_VERSION_MAJOR(minimumVulkanVersion), VK_API_VERSION_MINOR(minimumVulkanVersion), VK_API_VERSION_PATCH(minimumVulkanVersion));
        return false;
    }

    // Spec says: A non-zero variant indicates the API is a variant of the Vulkan API and applications will typically need to be modified to run against it.
    if (VK_API_VERSION_VARIANT(applicationInfo.apiVersion) != 0) {
        DM_CORE_ERROR("The Vulkan API supported on the system uses an unexpected variant: {}", VK_API_VERSION_VARIANT(applicationInfo.apiVersion));
        return false;
    }

    vkb::InstanceBuilder builder;
    auto inst_build = builder.set_app_name(m_WindowTitle.c_str())
                          .set_engine_name("Dimensional")
                          .require_api_version(applicationInfo.apiVersion)
                          .enable_extensions(instanceExtVec)
                          .request_validation_layers(m_DeviceParams.enableGPUValidation)
                          .use_default_debug_messenger();

    for (auto it : layerVec) {
        inst_build = inst_build.enable_layer(it);
    }
    auto inst_ret = inst_build.build();

    if (!inst_ret) {
        DM_CORE_ERROR("Failed to create Vulkan instance");
        return false;
    }

    m_VkbInstance = inst_ret.value();
    m_VulkanInstance = inst_ret->instance;
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_VulkanInstance);

    return true;
}

bool DeviceManager_VK::pickPhysicalDevice()
{

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

    DM_CORE_INFO("Enabled Vulkan device extensions:");
    for (const auto& ext : enabledExtensions.device) {
        DM_CORE_INFO("    {}", ext.c_str());

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
    APPEND_EXTENSION(true, bufferDeviceAddressFeatures);
    APPEND_EXTENSION(maintenance4Supported, maintenance4Features);
    APPEND_EXTENSION(aftermathSupported, aftermathPhysicalFeatures);
    physicalDeviceFeatures2.pNext = pNext;

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
    auto storage16BitFeatures = vk::PhysicalDevice16BitStorageFeatures()
                                    .setStorageBuffer16BitAccess(true);
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

    pNext = nullptr;
    APPEND_EXTENSION(accelStructSupported, accelStructFeatures)
    APPEND_EXTENSION(rayPipelineSupported, rayPipelineFeatures)
    APPEND_EXTENSION(rayQuerySupported, rayQueryFeatures)
    APPEND_EXTENSION(meshletsSupported, meshletFeatures)
    APPEND_EXTENSION(vrsSupported, vrsFeatures)
    APPEND_EXTENSION(interlockSupported, interlockFeatures)
    APPEND_EXTENSION(barycentricSupported, barycentricFeatures)
    APPEND_EXTENSION(storage16BitSupported, storage16BitFeatures)

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
                              .setVertexPipelineStoresAndAtomics(true);

    // Add a Vulkan 1.1 structure with default settings to make it easier for apps to modify them
    auto vulkan11features = vk::PhysicalDeviceVulkan11Features()
                                .setPNext(pNext);

    auto vulkan12features = vk::PhysicalDeviceVulkan12Features()
                                .setDescriptorIndexing(true)
                                .setRuntimeDescriptorArray(true)
                                .setDescriptorBindingPartiallyBound(true)
                                .setDescriptorBindingVariableDescriptorCount(true)
                                .setTimelineSemaphore(true)
                                .setShaderSampledImageArrayNonUniformIndexing(true)
                                .setBufferDeviceAddress(bufferDeviceAddressFeatures.bufferDeviceAddress);

    auto layerVec = stringSetToVector(enabledExtensions.layers);
    auto extVec = stringSetToVector(enabledExtensions.device);

    auto requiredExtensions = stringSetToVector(enabledExtensions.device);

    auto pdev_ret = vkb::PhysicalDeviceSelector(m_VkbInstance)
                        .set_surface(m_WindowSurface)
                        .add_required_extensions(requiredExtensions)
                        .set_minimum_version(1, 2)
                        .require_present(!m_DeviceParams.headlessDevice)
                        .set_required_features_11(vulkan11features)
                        .set_required_features_12(vulkan12features)
                        .set_required_features(deviceFeatures)
                        .add_required_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    auto pdev = pdev_ret.select();

    if (!pdev.has_value()) {
        DM_CORE_ERROR("Failed to select physical device");
        return false;
    }

    m_VkbPhysicalDevice = pdev.value();
    m_VulkanPhysicalDevice = pdev->physical_device;

    m_BufferDeviceAddressSupported = vulkan12features.bufferDeviceAddress;

    return true;
}

bool DeviceManager_VK::createDevice()
{

    const vk::PhysicalDeviceProperties physicalDeviceProperties = m_VulkanPhysicalDevice.getProperties();
    m_RendererString = std::string(physicalDeviceProperties.deviceName.data());

    auto dev_ret = vkb::DeviceBuilder(m_VkbPhysicalDevice).build();
    if (!dev_ret.has_value()) {
        DM_CORE_ERROR("Failed to create Vulkan device");
        return false;
    }

    m_VkbDevice = dev_ret.value();
    m_VulkanDevice = dev_ret->device;

    auto queue_ret = m_VkbDevice.get_queue(vkb::QueueType::graphics);
    if (queue_ret) {
        m_GraphicsQueue = queue_ret.value();
    }

    if (!m_DeviceParams.enableCopyQueue) {
        auto queue_ret = m_VkbDevice.get_queue(vkb::QueueType::transfer);
        m_TransferQueue = queue_ret.value();
    }
    if (!m_DeviceParams.enableComputeQueue) {
        auto queue_ret = m_VkbDevice.get_queue(vkb::QueueType::compute);
        m_ComputeQueue = queue_ret.value();
    }
    if (!m_DeviceParams.headlessDevice) {
        auto queue_ret = m_VkbDevice.get_queue(vkb::QueueType::present);
        m_PresentQueue = queue_ret.value();
    }
    VULKAN_HPP_DEFAULT_DISPATCHER.init(m_VulkanDevice);

    DM_CORE_INFO("Created Vulkan device: {}", m_RendererString.c_str());

    return true;
}

bool DeviceManager_VK::createWindowSurface()
{
    const VkResult res = glfwCreateWindowSurface(m_VulkanInstance, m_Window, nullptr, (VkSurfaceKHR*)&m_WindowSurface);
    if (res != VK_SUCCESS) {
        DM_CORE_ERROR("Failed to create a GLFW window surface, error code = {}", nvrhi::vulkan::resultToString(res));
        return false;
    }
    return true;
}

void DeviceManager_VK::destroySwapChain()
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

bool DeviceManager_VK::createSwapChain()
{
    destroySwapChain();

    m_SwapChainFormat = {
        vk::Format(nvrhi::vulkan::convertFormat(m_DeviceParams.swapChainFormat)),
        vk::ColorSpaceKHR::eSrgbNonlinear
    };

    vk::Extent2D extent = vk::Extent2D(m_DeviceParams.backBufferWidth, m_DeviceParams.backBufferHeight);

    std::unordered_set<uint32_t> uniqueQueues = {
        m_VkbDevice.get_queue_index(vkb::QueueType::graphics).value(),
        m_VkbDevice.get_queue_index(vkb::QueueType::present).value(),
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

    auto images = m_VulkanDevice.getSwapchainImagesKHR(m_SwapChain);
    for (auto image : images) {
        SwapChainImage sci;
        sci.image = image;

        nvrhi::TextureDesc textureDesc;
        textureDesc.width = m_DeviceParams.backBufferWidth;
        textureDesc.height = m_DeviceParams.backBufferHeight;
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

#define CHECK(a)      \
    if (!(a)) {       \
        return false; \
    }

bool DeviceManager_VK::CreateInstanceInternal()
{

    if (m_DeviceParams.enableDebugRuntime) {
        enabledExtensions.instance.insert("VK_EXT_debug_report");
        enabledExtensions.layers.insert("VK_LAYER_KHRONOS_validation");
    }

    m_dynamicLoader = CreateScope<VulkanDynamicLoader>(m_DeviceParams.vulkanLibraryName);
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_dynamicLoader->getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    return createInstance();
}

bool DeviceManager_VK::CreateDevice()
{
    // if (m_DeviceParams.enableDebugRuntime) {
    //     installDebugCallback();
    // }

    if (!m_DeviceParams.headlessDevice) {
        // Need to adjust the swap chain format before creating the device because it affects physical device selection
        if (m_DeviceParams.swapChainFormat == nvrhi::Format::SRGBA8_UNORM)
            m_DeviceParams.swapChainFormat = nvrhi::Format::SBGRA8_UNORM;
        else if (m_DeviceParams.swapChainFormat == nvrhi::Format::RGBA8_UNORM)
            m_DeviceParams.swapChainFormat = nvrhi::Format::BGRA8_UNORM;

        CHECK(createWindowSurface())
    }
    CHECK(pickPhysicalDevice())
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
    deviceDesc.graphicsQueueIndex = m_VkbDevice.get_queue_index(vkb::QueueType::graphics).value();
    if (m_DeviceParams.enableComputeQueue) {
        deviceDesc.computeQueue = m_ComputeQueue;
        deviceDesc.computeQueueIndex = m_VkbDevice.get_queue_index(vkb::QueueType::compute).value();
    }
    if (m_DeviceParams.enableCopyQueue) {
        deviceDesc.transferQueue = m_TransferQueue;
        deviceDesc.transferQueueIndex = m_VkbDevice.get_queue_index(vkb::QueueType::transfer).value();
    }
    deviceDesc.instanceExtensions = vecInstanceExt.data();
    deviceDesc.numInstanceExtensions = vecInstanceExt.size();
    deviceDesc.deviceExtensions = vecDeviceExt.data();
    deviceDesc.numDeviceExtensions = vecDeviceExt.size();
    deviceDesc.bufferDeviceAddressSupported = m_BufferDeviceAddressSupported;
    deviceDesc.vulkanLibraryName = m_DeviceParams.vulkanLibraryName;

    m_NvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

    if (m_DeviceParams.enableNvrhiValidationLayer) {
        m_ValidationLayer = nvrhi::validation::createValidationLayer(m_NvrhiDevice);
    }

    return true;
}

bool DeviceManager_VK::CreateSwapChain()
{
    CHECK(createSwapChain())

    m_PresentSemaphores.reserve(m_DeviceParams.maxFramesInFlight + 1);
    m_AcquireSemaphores.reserve(m_DeviceParams.maxFramesInFlight + 1);
    for (uint32_t i = 0; i < m_DeviceParams.maxFramesInFlight + 1; ++i) {
        m_PresentSemaphores.push_back(m_VulkanDevice.createSemaphore(vk::SemaphoreCreateInfo()));
        m_AcquireSemaphores.push_back(m_VulkanDevice.createSemaphore(vk::SemaphoreCreateInfo()));
    }

    return true;
}

void DeviceManager_VK::DestroyDeviceAndSwapChain()
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

bool DeviceManager_VK::BeginFrame()
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

            m_DeviceParams.backBufferWidth = surfaceCaps.currentExtent.width;
            m_DeviceParams.backBufferHeight = surfaceCaps.currentExtent.height;

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

bool DeviceManager_VK::Present()
{
    const auto& semaphore = m_PresentSemaphores[m_PresentSemaphoreIndex];

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

    m_PresentSemaphoreIndex = (m_PresentSemaphoreIndex + 1) % m_PresentSemaphores.size();

#ifndef _WIN32
    if (m_DeviceParams.vsyncEnabled || m_DeviceParams.enableDebugRuntime) {
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

DeviceManager* DeviceManager::CreateVK()
{
    return new DeviceManager_VK();
}
#undef CHECK

}
