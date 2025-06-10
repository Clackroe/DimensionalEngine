#ifndef DM_SHADER_REFLECTION
#define DM_SHADER_REFLECTION

#include "nvrhi/nvrhi.h"
#include <Rendering/ShaderHelpersAndEnums.hpp>
#include <slang/slang-com-ptr.h>
#include <slang/slang.h>
#include <string>
#include <vector>

namespace Dimensional {

struct EntryPointDescription;

enum class ShaderResourceKind {
    ConstantBuffer,
    StructuredBuffer,
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    TextureBuffer,
    Sampler,
    ByteBuffer,
    Unknown
};

enum class ShaderResourceAccess {
    READ,
    WRITE,
    READ_WRITE
};

struct Binding {
    u32 slot;
    u32 space;
};

struct ShaderResource {
    std::string name;
    Binding binding;
    ShaderResourceKind kind;
    ShaderResourceAccess access;
};

struct ShaderReflectionData {
    std::string name;
    nvrhi::ShaderType type;
    std::vector<ShaderResource> resources;
};
namespace ShaderReflector {

    ShaderReflectionData extractFromProgram(Slang::ComPtr<slang::IComponentType> program, const EntryPointDescription& entryPointDesc);

    void printReflection(const ShaderReflectionData& data);
};

}

#endif // DM_SHADER_REFLECTION
