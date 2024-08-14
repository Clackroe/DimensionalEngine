#ifndef DM_SHADERH
#define DM_SHADERH

#include <core.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Dimensional {
class Shader {
private:
    void load();

public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragPath);

    void use();

    void setMat4(const std::string& name, glm::mat4 value);
    void setBool(const std::string& name, bool value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
};
}

#endif
