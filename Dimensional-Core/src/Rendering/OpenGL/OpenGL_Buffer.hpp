#ifndef OPENGL_BUFFER_HPP
#define OPENGL_BUFFER_HPP

#include "Rendering/VAO.hpp"

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

}

#endif // OPENGL_BUFFER_HPP
