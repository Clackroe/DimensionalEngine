#ifndef OPENGL_SHADER_HPP
#define OPENGL_SHADER_HPP
#include "Core/UUID.hpp"
#include "Rendering/Shader.hpp"
namespace Dimensional {

struct OpenGLShader {

    static OpenGLShader Create(const ShaderData& data);

    void Bind();
    void UnBind();
    void Destroy();

private:
    void link(const std::vector<u32> programs);

    u32 m_GLID;
};

};
#endif // OPENGL_SHADER_HPP
