#ifndef DM_SHADERH
#define DM_SHADERH

#include <core.hpp>

#include "Core/Assets/Asset.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Dimensional {
class DMCORE_API Shader : public Asset {
public:
    unsigned int ID;

    Shader(const std::string& vertexPath, const std::string& fragPath);

    void use();

    void setMat4(const std::string& name, glm::mat4 value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setBool(const std::string& name, bool value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

private:
    void load();
};
}

#endif
