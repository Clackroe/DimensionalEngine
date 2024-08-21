#include <Rendering/ElementBuffer.hpp>
#include <glad.h>

namespace Dimensional {
ElementBuffer::ElementBuffer(const u32* data, u32 count)
    : m_Count(count)
{
    glGenBuffers(1, &m_GLId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), data, GL_STATIC_DRAW);
}
ElementBuffer::~ElementBuffer()
{
    glDeleteBuffers(1, &m_GLId);
}

void ElementBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLId);
}
void ElementBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
