#include "Rendering/GPUBufferManager.hpp"
#include <Rendering/GPUBuffer.hpp>

namespace Dimensional {

Ref<GPUBuffer> GPUBuffer::Create(const GPUBufferData& data)
{
    Ref<GPUBuffer> target = Ref<GPUBuffer>(new GPUBuffer());
    target->m_UUID = GPUBufferManager::CreateGPUBuffer(data);
    return target;
}

void GPUBuffer::Bind(u32 slot)
{
    GPUBufferManager::Bind(m_UUID, slot);
}

void GPUBuffer::Resize(size_t sizeBytes)
{
    GPUBufferManager::Resize(m_UUID, sizeBytes);
}

void GPUBuffer::SetData(const void* data, size_t offset, size_t sizeBytes)
{
    GPUBufferManager::SetData(m_UUID, data, offset, sizeBytes);
};

void GPUBuffer::ZeroData()
{
    GPUBufferManager::ZeroData(m_UUID);
}

void GPUBuffer::Destroy()
{
    GPUBufferManager::Destroy(m_UUID);
}
}
