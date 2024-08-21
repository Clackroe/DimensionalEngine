#include <Rendering/VertexArray.hpp>
#include <Rendering/VertexLayout.hpp>
#include <glad.h>
namespace Dimensional {

struct vbElement {
    u32 type;
    u32 count;
    u8 normalized;

    static unsigned int GetSizeOfType(u32 type)
    {
        switch (type) {
        case GL_FLOAT:
            return 4;
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        }
        return 0;
    };
};

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_GLId);
}
VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_GLId);
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
    Bind();
    vb.Bind();
    const auto& elements = vLayout.getElements();
    u32 offset = 0;
    for (u32 i = 0; i < elements.size(); i++) {
        const auto& el = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, el.count, el.type, el.normalized, vLayout.getStride(), (const void*)offset);
        offset += el.count * vbElement::GetSizeOfType(el.type);
    }
}
}
