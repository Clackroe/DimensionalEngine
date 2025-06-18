#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "nvrhi/nvrhi.h"
#include <Rendering/Buffer.hpp>

namespace Dimensional {

// =====================
// ========BUFFER======
// =====================

Ref<Buffer> Buffer::InternalCreate(nvrhi::BufferDesc& desc, const BufferCreateInfo& info)
{
    Ref<Buffer> buff = Ref<Buffer>(new Buffer());
    buff->m_DebugName = info.debugName;

    desc.setByteSize(info.sizeBytes);

    if (info.cpuAccess) {
        desc.setCpuAccess(nvrhi::CpuAccessMode::Read);
    }

    desc.keepInitialState = true;

    auto dev = Application::getDevice();

    auto handle = dev->createBuffer(desc);
    if (!handle) {
        DM_CORE_ERROR("FAILED TO CREATE BUFFER: {}", info.debugName);
        return nullptr;
    }

    buff->m_Handle = handle;
    buff->m_SizeBytes = info.sizeBytes;

    return buff;
}

void Buffer::SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset)
{
    if (offset + size > m_SizeBytes) {
        DM_CORE_WARN("Tried to set data of size {0} bytes on buffer [{1}], which has a size of {2}", offset + size, m_DebugName, m_SizeBytes);
        return;
    }
    cmd->writeBuffer(m_Handle, data, size, offset);
}

void Buffer::Resize(nvrhi::CommandListHandle cmd, size_t size)
{

    auto dev = Application::getDevice();

    auto desc = m_Handle->getDesc();
    desc.setByteSize(size);
    auto handle = dev->createBuffer(desc);
    if (!handle) {
        DM_CORE_ERROR("FAILED TO RESIZE BUFFER: {}", m_DebugName);
        return;
    }

    m_Handle = handle;
    m_SizeBytes = size;
}

// ===========================
// ========Vertex BUFFER======
// ==========================

Ref<VertexBuffer> VertexBuffer::Create(const BufferCreateInfo& info)
{

    Ref<VertexBuffer> buff = Ref<VertexBuffer>(new VertexBuffer());

    nvrhi::BufferDesc desc;
    desc.setIsVertexBuffer(true);
    desc.setInitialState(nvrhi::ResourceStates::VertexBuffer);
    desc.setDebugName(info.debugName);

    buff->m_Buffer = Buffer::InternalCreate(desc, info);

    return buff;
}

void VertexBuffer::SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset)
{
    m_Buffer->SetData(cmd, data, size, offset);
}
void VertexBuffer::Resize(nvrhi::CommandListHandle cmd, size_t size)
{
    m_Buffer->Resize(cmd, size);
}

// ===========================
// ========Index BUFFER======
// ==========================

Ref<IndexBuffer> IndexBuffer::Create(const BufferCreateInfo& info)
{

    Ref<IndexBuffer> buff = Ref<IndexBuffer>(new IndexBuffer());

    nvrhi::BufferDesc desc;
    desc.setIsIndexBuffer(true);
    desc.setInitialState(nvrhi::ResourceStates::IndexBuffer);
    desc.setDebugName(info.debugName);

    buff->m_Buffer = Buffer::InternalCreate(desc, info);

    return buff;
}

void IndexBuffer::SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset)
{
    m_Buffer->SetData(cmd, data, size, offset);
}
void IndexBuffer::Resize(nvrhi::CommandListHandle cmd, size_t size)
{
    m_Buffer->Resize(cmd, size);
}

// ===========================
// ========Constant BUFFER======
// ==========================

Ref<ConstantBuffer> ConstantBuffer::Create(const BufferCreateInfo& info)
{

    Ref<ConstantBuffer> buff = Ref<ConstantBuffer>(new ConstantBuffer());

    nvrhi::BufferDesc desc;
    desc.setIsConstantBuffer(true);
    desc.setInitialState(nvrhi::ResourceStates::ConstantBuffer);
    desc.setDebugName(info.debugName);

    buff->m_Buffer = Buffer::InternalCreate(desc, info);

    return buff;
}

void ConstantBuffer::SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset)
{
    m_Buffer->SetData(cmd, data, size, offset);
}
void ConstantBuffer::Resize(nvrhi::CommandListHandle cmd, size_t size)
{
    m_Buffer->Resize(cmd, size);
}

// ===========================
// ========Constant BUFFER======
// ==========================

Ref<StorageBuffer> StorageBuffer::Create(const BufferCreateInfo& info)
{

    Ref<StorageBuffer> buff = Ref<StorageBuffer>(new StorageBuffer());

    nvrhi::BufferDesc desc;
    desc.setCanHaveUAVs(true);
    desc.setInitialState(nvrhi::ResourceStates::UnorderedAccess);
    desc.setDebugName(info.debugName);

    buff->m_Buffer = Buffer::InternalCreate(desc, info);

    return buff;
}

void StorageBuffer::SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset)
{
    m_Buffer->SetData(cmd, data, size, offset);
}
void StorageBuffer::Resize(nvrhi::CommandListHandle cmd, size_t size)
{
    m_Buffer->Resize(cmd, size);
}

}
