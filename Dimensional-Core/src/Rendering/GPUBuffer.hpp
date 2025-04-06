#ifndef GPU_BUFFER_AGNOST_HPP
#define GPU_BUFFER_AGNOST_HPP

#include "Core/UUID.hpp"
#include <vector>
namespace Dimensional {

enum class GPUBufferType {
    UNIFORM,
    STORAGE,
    COMMAND,
    ELEMENT
};

enum class GPUBufferUsage {
    STATIC,
    DYNAMIC,
};

struct GPUBufferData {
    GPUBufferType type;
    GPUBufferUsage usage;
    bool persistant = false;
    const void* data = nullptr;
    size_t sizeBytes;
    u32 slot;
};

static std::vector<GPUBufferData> vec(10);

struct GPUBuffer {

    static Ref<GPUBuffer> Create(const GPUBufferData& data);

    void Bind(u32 slot);

    void SetData(const void* data, size_t offset, size_t sizeBytes);

    void ZeroData();

    void Resize(size_t sizeBytes);

    void Destroy();

private:
    GPUBuffer() = default;
    UUID m_UUID;
};

}
#endif // GPU_BUFFER_AGNOST_HPP
