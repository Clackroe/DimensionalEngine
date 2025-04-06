#ifndef OPENGL_BUFFER_HPP
#define OPENGL_BUFFER_HPP

#include "Rendering/GPUBuffer.hpp"
#include "Rendering/VAO.hpp"
#include <vector>

namespace Dimensional {

struct OpenGLVAO {

    static OpenGLVAO Create();

    void Bind();
    void UnBind();
    void Destroy();

    void SetData(const VAOData& data);

private:
    u32 m_VAO;
    u32 m_VBO = 0;
    u32 m_EBO = 0;
};

struct OpenGLGPUBuffer {

    static OpenGLGPUBuffer Create(const GPUBufferData& data);

    void Bind(u32 slot);

    void SetData(const void* data, size_t offset, size_t sizeBytes);

    void Resize(size_t sizeBytes);

    void ZeroData();

    void* GetPersistPTR();

    void Destroy();

private:
    u32 m_GLID;
    void* m_MappedPtr = nullptr;
    bool m_IsPersistant = false;
    GPUBufferData m_Data;
};

}

#endif // OPENGL_BUFFER_HPP
