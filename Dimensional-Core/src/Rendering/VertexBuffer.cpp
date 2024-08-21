#include "Rendering/VertexBuffer.hpp"
#include <glad.h>
namespace Dimensional {
VertexBuffer::VertexBuffer(const void* data, u32 sizeBytes)
{
    glGenBuffers(1, &m_GLId);

    glBindBuffer(GL_ARRAY_BUFFER, m_GLId);
    glBufferData(GL_ARRAY_BUFFER, sizeBytes, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_GLId);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_GLId);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
}
