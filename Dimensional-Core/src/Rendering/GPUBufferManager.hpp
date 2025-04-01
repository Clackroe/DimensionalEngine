#ifndef GPU_BUFFER_MANAGER_AGNOST_HPP
#define GPU_BUFFER_MANAGER_AGNOST_HPP
#include "Rendering/GPUBuffer.hpp"
#include "Rendering/OpenGL/OpenGL_Buffer.hpp"
namespace Dimensional {

union GraphicsGPUBuffer {
    OpenGLGPUBuffer glBuffer;
};

struct GPUBufferManager {

    static UUID CreateGPUBuffer(const GPUBufferData& data);

    static void Bind(UUID id, u32 slot);

    static void SetData(UUID id, const void* data, size_t sizeBytes);

    static void Destroy(UUID id);

private:
    static UMap<UUID, GraphicsGPUBuffer> s_RTMap;
};

}
#endif
