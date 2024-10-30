#include "Log/log.hpp"
#include <Rendering/UniformBuffer.hpp>
#include <glad.h>
namespace Dimensional {
UniformBuffer::UniformBuffer(u32 sizeBytes, u32 binding)
    : m_Binding(binding)
    , m_SizeBytes(sizeBytes)
{
    glGenBuffers(1, &m_GLId);
    glBindBuffer(GL_UNIFORM_BUFFER, m_GLId);
    glBufferData(GL_UNIFORM_BUFFER, m_SizeBytes, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, m_Binding, m_GLId);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &m_GLId);
}

void UniformBuffer::bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_GLId);
}
void UniformBuffer::unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::zeroOut()
{
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, m_SizeBytes, nullptr);
    unbind();
}

void UniformBuffer::setData(const void* data, u32 offset, u32 sizeBytes)
{
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeBytes, data);
    unbind();
}
}
