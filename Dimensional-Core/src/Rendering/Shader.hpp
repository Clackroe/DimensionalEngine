#ifndef SHADER_AGNOST_HPP
#define SHADER_AGNOST_HPP

#include "Core/UUID.hpp"
#include <vector>
namespace Dimensional {

enum ShaderType {
    NONE = 0,
    COMPUTE,
    VERTEX,
    FRAGMENT,
    GEOMETRY
};

struct ShaderData {
    std::string name;
    UMap<ShaderType, std::string> programs;
};

struct Shader {

    // static Ref<Shader> CreateCompute(const std::string& path);
    static Ref<Shader> Create(const std::string& path);

    void Bind();
    void UnBind();

    UUID GetUUID() { return m_ID; };

private:
    UUID m_ID;
    Shader() = default;
};

}

#endif // SHADER_AGNOST_HPP
