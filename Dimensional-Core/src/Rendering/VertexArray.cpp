#include <Rendering/VertexArray.hpp>
#include <Rendering/VertexLayout.hpp>
#include <cstdint>
#include <glad.h>
namespace Dimensional {

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_GLId);
}
VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_GLId);
    glDeleteBuffers(1, &m_Buffer);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_GLId);
}
void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexLayout& vLayout)
{
    m_Buffer = vb.getId();
    Bind();
    vb.Bind();
    const auto& elements = vLayout.getElements();
    u32 offset = 0;
    for (u32 i = 0; i < elements.size(); i++) {
        const auto& el = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, el.count, el.type, el.normalized, vLayout.getStride(), (const void*)(uintptr_t)offset);
        offset += el.count * vbElement::GetSizeOfType(el.type);
    }
}
}
