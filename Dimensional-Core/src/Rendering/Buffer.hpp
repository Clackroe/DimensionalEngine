#ifndef DM_BUFFER_HPP
#define DM_BUFFER_HPP

#include "Rendering/Renderer.hpp"
#include "nvrhi/nvrhi.h"
namespace Dimensional {

struct BufferCreateInfo {
    size_t sizeBytes;

    bool cpuAccess = false;
    std::string debugName = "Unamed Buffer";
};

class Buffer {

    void SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset = 0);
    void SetData(const void* data, size_t size, size_t offset = 0);

    void Resize(nvrhi::CommandListHandle handle, size_t size);

private:
    Buffer() = default;

    static Ref<Buffer> InternalCreate(nvrhi::BufferDesc& desc, const BufferCreateInfo& info);

    std::string m_DebugName;
    nvrhi::BufferHandle m_Handle;
    size_t m_SizeBytes;

    friend class VertexBuffer;
    friend class IndexBuffer;
    friend class ConstantBuffer;
    friend class StorageBuffer;
};

class VertexBuffer {
public:
    static Ref<VertexBuffer> Create(const BufferCreateInfo& info, VERTEX_BUFFER_TYPE type = VERTEX_BUFFER_TYPE::VERTEX_DATA);

    void SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset = 0);
    void SetData(const void* data, size_t size, size_t offset = 0);

    void Resize(nvrhi::CommandListHandle cmd, size_t size);

    size_t GetSize() { return m_Buffer->m_SizeBytes; }
    nvrhi::BufferHandle GetHandle() { return m_Buffer->m_Handle; };
    nvrhi::VertexBufferBinding GetBinding() { return m_Binding; };

private:
    VertexBuffer() = default;

    u32 m_VertexSlot;

    nvrhi::VertexBufferBinding m_Binding;

    Ref<Buffer> m_Buffer = nullptr; // Avoid virtual functions and inheritance
};

class IndexBuffer {
public:
    static Ref<IndexBuffer> Create(const BufferCreateInfo& info);

    void SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset = 0);
    void SetData(const void* data, size_t size, size_t offset = 0);

    void Resize(nvrhi::CommandListHandle cmd, size_t size);

    nvrhi::BufferHandle GetHandle() { return m_Buffer->m_Handle; };

private:
    IndexBuffer() = default;

    Ref<Buffer> m_Buffer = nullptr; // Avoid virtual functions and inheritance
    size_t GetSize() { return m_Buffer->m_SizeBytes; }
};

class StorageBuffer {
public:
    static Ref<StorageBuffer> Create(const BufferCreateInfo& info);

    void SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset = 0);
    void SetData(const void* data, size_t size, size_t offset = 0);

    void Resize(nvrhi::CommandListHandle cmd, size_t size);
    size_t GetSize() { return m_Buffer->m_SizeBytes; }

    nvrhi::BufferHandle GetHandle() { return m_Buffer->m_Handle; };

private:
    StorageBuffer() = default;

    Ref<Buffer> m_Buffer = nullptr; // Avoid virtual functions and inheritance
};

class ConstantBuffer {
public:
    static Ref<ConstantBuffer> Create(const BufferCreateInfo& info);

    void SetData(nvrhi::CommandListHandle cmd, const void* data, size_t size, size_t offset = 0);
    void SetData(const void* data, size_t size, size_t offset = 0);

    void Resize(nvrhi::CommandListHandle cmd, size_t size);
    size_t GetSize() { return m_Buffer->m_SizeBytes; }

    nvrhi::BufferHandle GetHandle() { return m_Buffer->m_Handle; };

private:
    ConstantBuffer() = default;

    Ref<Buffer> m_Buffer = nullptr; // Avoid virtual functions and inheritance
};

}

#endif // DM_BUFFER_HPP
