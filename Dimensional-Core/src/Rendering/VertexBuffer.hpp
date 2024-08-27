#ifndef DM_VERTEXBUFFER_H
#define DM_VERTEXBUFFER_H
#include <core.hpp>
namespace Dimensional {
class DMCORE_API VertexBuffer {
public:
    VertexBuffer(const void* data, u32 sizeBytes);
    VertexBuffer() = default;

    ~VertexBuffer();

    u32 getId() const { return m_GLId; }

    void Bind() const;
    void Unbind() const;

private:
    u32 m_GLId;
};
}
#endif
