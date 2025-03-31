#include "Rendering/OpenGL/OpenGL_Buffer.hpp"
#include "Log/log.hpp"
#include "Rendering/VAO.hpp"

#include <glad.h>
namespace Dimensional {

static UMap<AttributeType, GLuint> AttributeTypeToGLType = {
    { AttributeType::FLOAT, GL_FLOAT },
    { AttributeType::U32, GL_UNSIGNED_INT },
    { AttributeType::BYTE, GL_BYTE },
    { AttributeType::INT, GL_INT },
    { AttributeType::UBYTE, GL_UNSIGNED_BYTE },
};

OpenGLVAO OpenGLVAO::Create()
{
    OpenGLVAO vao;
    glCreateVertexArrays(1, &vao.m_VAO);
    return vao;
}

void OpenGLVAO::Bind()
{
    glBindVertexArray(m_VAO);
}
void OpenGLVAO::UnBind()
{
    glBindVertexArray(0);
}

void OpenGLVAO::Destroy()
{
    glDeleteVertexArrays(1, &m_VAO); // Always present

    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
    }
}

void OpenGLVAO::SetData(const VAOData& data)
{

    DM_CORE_INFO("Setting VAO Data");

    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
    }
    if (m_EBO != 0) {
        glDeleteBuffers(1, &m_EBO);
    }

    glCreateBuffers(1, &m_VBO);
    glNamedBufferData(m_VBO, data.dataSizeBytes, data.data, GL_STATIC_DRAW); // TODO: Maybe make usage configurable

    // setup buffer attributes and such
    int index = 0;
    int offset = 0;
    for (VertexAttribute att : data.layout) {

        glEnableVertexArrayAttrib(m_VAO, index);
        glVertexArrayAttribFormat(m_VAO, index, att.elementsCnt, AttributeTypeToGLType.at(att.type), (GLboolean)att.normalized, (GLint)offset);
        glVertexArrayAttribBinding(m_VAO, index, 0);
        offset += att.elementsCnt * AttributeTypeToSize.at(att.type);
        index++;
    }

    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, offset);

    // Setup EBO
    if (data.indexBuffer.size() > 0) {
        glCreateBuffers(1, &m_EBO);
        glNamedBufferStorage(m_EBO, data.indexBuffer.size() * sizeof(u32), data.indexBuffer.data(), GL_MAP_READ_BIT);
        glVertexArrayElementBuffer(m_VAO, m_EBO);
    }
}

}
