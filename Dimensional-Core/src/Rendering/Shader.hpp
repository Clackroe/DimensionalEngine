#ifndef DM_SHADERH
#define DM_SHADERH

#include <core.hpp>

#include "Asset/Asset.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Dimensional {
enum DMCORE_API ShaderType {
    RENDER,
    COMPUTE,

    // for internal use only
    VERTEX,
    FRAGMENT
};

class DMCORE_API Shader : public Asset {
public:
    unsigned int ID;

    Shader(const std::string& path, enum ShaderType type = RENDER);

    void use();

    void dispatchCompute(u32 width, u32 height, u32 depth = 1);

    void setMat4(const std::string& name, glm::mat4 value);
    void setMat3(const std::string& name, glm::mat3 value);
    void setVec3(const std::string& name, float x, float y, float z);
    void setBool(const std::string& name, bool value) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;

    virtual AssetType getAssetType() const override { return AssetType::SHADER; };

private:
    u32 compile(const char* shaderProg, enum ShaderType type);
    void link(std::vector<u32> programs);
    enum ShaderType m_Type;
};
}

#endif
