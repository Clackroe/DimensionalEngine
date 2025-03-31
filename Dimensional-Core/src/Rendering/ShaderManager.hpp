#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include "Rendering/OpenGL/OpenGL_Shader.hpp"
#include "Rendering/Shader.hpp"
#include "core.hpp"
#include <Core/UUID.hpp>

namespace Dimensional {

union GraphicsShader {
    OpenGLShader glShader = {};
};

struct ShaderManager {

    static UUID CreateShader(const ShaderData& data);

    static void Bind(UUID id);
    static void UnBind(UUID id); // Maybe unecessary

    static void DeleteShader(UUID id);

private:
    static UMap<UUID, GraphicsShader> s_ShaderMap;
};

}

#endif
