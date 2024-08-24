#ifndef DM_VERTEXBUFFER_H
#define DM_VERTEXBUFFER_H

namespace Dimensional {
class DMCORE_API VertexBuffer {
public:
    VertexBuffer(const void* data, u32 sizeBytes);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

private:
    u32 m_GLId;
};
}
#endif
