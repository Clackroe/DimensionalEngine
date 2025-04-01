#ifndef GPU_BUFFER_AGNOST_HPP
#define GPU_BUFFER_AGNOST_HPP
#include "Core/UUID.hpp"
namespace Dimensional {

enum class GPUBufferType {
    UNIFORM,
    STORAGE
};

enum class GPUBufferUsage {
    STATIC,
    DYNAMIC
};

struct GPUBufferData {
    GPUBufferType type;
    GPUBufferUsage usage;
    const void* data;
    size_t sizeBytes;
};

struct GPUBuffer {

    static Ref<GPUBuffer> Create(const GPUBufferData& data);

    void Bind(u32 slot);

    void SetData(const void* data, size_t sizeBytes);

    void Destroy();

private:
    GPUBuffer() = default;
    UUID m_UUID;
};

}
#endif // GPU_BUFFER_AGNOST_HPP
