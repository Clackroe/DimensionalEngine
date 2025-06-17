#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "nvrhi/nvrhi.h"
#include <Rendering/Texture.hpp>

namespace Dimensional {

Ref<Texture2D> Texture2D::Create(const TextureCreateInfo& info)
{
    auto dev = Application::getDevice();

    Ref<Texture2D> tex = Ref<Texture2D>(new Texture2D());

    if (!tex->Init(dev, info)) {
        DM_CORE_ERROR("Failed to create Texture2D: {}", info.debugName);
        return nullptr;
    }
    return tex;
}

bool Texture2D::Init(nvrhi::DeviceHandle dev, const TextureCreateInfo& info)
{
    if (!dev) {
        return false;
    }

    nvrhi::TextureDesc desc;
    desc.setDebugName(info.debugName);
    desc.setDepth(1);
    desc.setWidth(info.width);
    desc.setHeight(info.height);
    desc.setIsUAV(info.storage);
    desc.isShaderResource = true;
    desc.format = info.format;
    desc.setDimension(nvrhi::TextureDimension::Texture2D);
    desc.setInitialState(nvrhi::ResourceStates::ShaderResource);
    desc.keepInitialState = true;
    desc.arraySize = 1;

    auto t = dev->createTexture(desc);
    if (!t) {
        return false;
    }

    m_Handle = t;
    m_IsUAV = info.storage;
    m_Depth = 1;
    m_ArraySize = info.arraySize;
    m_Width = info.width;
    m_Height = info.height;
    m_GenerateMips = info.generateMipmaps;
    m_Format = info.format;
    m_DebugName = info.debugName;

    return true;
}

void Texture2D::SetData(const unsigned char* data, size_t sizeBytes, u32 mip)
{
    auto dev = Application::getDevice();
    if (!m_Handle || !dev || !data)
        return;

    // TODO: Move to submission based, record, then submit to exicution queue. (Most likely resource loading queue)
    nvrhi::CommandListHandle commandList = dev->createCommandList();
    commandList->open();

    // Calculate row pitch based on format and width
    u32 mipWidth = std::max(1u, m_Width >> mip);
    u32 bytesPerPixel = nvrhi::getFormatInfo(m_Format).bytesPerBlock;
    u32 rowPitch = mipWidth * bytesPerPixel;

    commandList->writeTexture(m_Handle, mip, 0, data, rowPitch);

    commandList->close();
    dev->executeCommandList(commandList);
}

void Texture2D::Resize(u32 width, u32 height)
{
    nvrhi::TextureDesc desc;
    desc.setDebugName(m_DebugName);
    desc.setDepth(1);
    desc.setWidth(width);
    desc.setHeight(height);
    desc.setIsUAV(m_IsUAV);
    desc.isShaderResource = true;
    desc.format = m_Format;
    desc.setDimension(nvrhi::TextureDimension::Texture2D);
    desc.setInitialState(nvrhi::ResourceStates::ShaderResource);
    desc.keepInitialState = true;
    desc.arraySize = 1;

    auto dev = Application::getDevice();
    auto t = dev->createTexture(desc);
    if (!t) {
        DM_CORE_WARN("Failed to recreate texture2D {}", m_DebugName);
        return;
    }
    m_Handle = t;
    m_Width = width;
    m_Height = height;
}

}
