#ifndef DM_VERTEXARRAY_H
#define DM_VERTEXARRAY_H
#include "Rendering/VertexBuffer.hpp"
#include "Rendering/VertexLayout.hpp"
namespace Dimensional {
class DMCORE_API VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddBuffer(const VertexBuffer& vb, const VertexLayout& vLayout);

private:
    u32 m_GLId;
};
}

#endif
