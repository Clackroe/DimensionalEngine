#ifndef DM_SHADER_REFLECTION
#define DM_SHADER_REFLECTION

#include "nvrhi/nvrhi.h"
#include <Rendering/ShaderHelpersAndEnums.hpp>
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>
#include <vector>

namespace Dimensional {

struct EntryPointDescription;

struct Binding {
    int slot, space; // Space -> Set
};

struct ShaderParam {
    Binding binding;
    std::string name;
    nvrhi::ResourceType type;
};

// Constant and Structured Buffers (Maybe it could be useful to separate them?)
struct ShaderBuffer : public ShaderParam {
    u32 size;
    std::vector<std::pair<std::string, u32>> members; // (member name, offset)
};

typedef ShaderParam ShaderTexture;

struct ShaderReflection {
    std::vector<ShaderBuffer> shaderBuffers; // Set -> Buffer
    std::vector<ShaderTexture> shaderTextures; // Set -> Texture
};

class ShaderReflector {
public:
    ShaderReflector() = default;
    ~ShaderReflector() = default;

    static ShaderReflection extractFromProgram(Slang::ComPtr<slang::IComponentType> program, const EntryPointDescription& entryPointDesc);

private:
};

}

#endif // DM_SHADER_REFLECTION
