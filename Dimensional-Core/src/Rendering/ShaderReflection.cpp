#include "Log/log.hpp"
#include "Rendering/ShaderHelpersAndEnums.hpp"
#include "nvrhi/nvrhi.h"
#include <Rendering/ShaderCompiler.hpp>
#include <Rendering/ShaderReflection.hpp>
#include <algorithm>
#include <slang/slang.h>

namespace Dimensional {

namespace ShaderReflector {

    static ShaderResourceKind _getResourceKind(slang::TypeReflection* ref)
    {
        auto kind = ref->getKind();

        if (kind == slang::TypeReflection::Kind::ConstantBuffer) {
            return ShaderResourceKind::ConstantBuffer;
        }

        if (kind != slang::TypeReflection::Kind::Resource) {
            return ShaderResourceKind::Unknown;
        }

        auto shape = ref->getResourceShape();
        using SR = SlangResourceShape;
        auto baseShape = shape & SR::SLANG_RESOURCE_BASE_SHAPE_MASK;

        switch (baseShape) {
        case SR::SLANG_TEXTURE_1D:
            return ShaderResourceKind::Texture1D;
        case SR::SLANG_TEXTURE_2D:
            return ShaderResourceKind::Texture2D;
        case SR::SLANG_TEXTURE_3D:
            return ShaderResourceKind::Texture3D;
        case SR::SLANG_TEXTURE_BUFFER:
            return ShaderResourceKind::TextureBuffer;
        case SR::SLANG_TEXTURE_CUBE:
            return ShaderResourceKind::TextureCube;
        case SR::SLANG_STRUCTURED_BUFFER:
            return ShaderResourceKind::StructuredBuffer;
        case SR::SLANG_BYTE_ADDRESS_BUFFER:
            return ShaderResourceKind::ByteBuffer;
        default:
            return ShaderResourceKind::Unknown;
        }
    }

    static ShaderResourceAccess _getResourceAccess(SlangResourceAccess acc)
    {
        switch (acc) {
        case SLANG_RESOURCE_ACCESS_READ_WRITE:
            return ShaderResourceAccess::READ_WRITE;
        case SLANG_RESOURCE_ACCESS_READ:
            return ShaderResourceAccess::READ;

        case SLANG_RESOURCE_ACCESS_WRITE:
            return ShaderResourceAccess::WRITE;

        case SLANG_RESOURCE_ACCESS_RASTER_ORDERED:
        case SLANG_RESOURCE_ACCESS_APPEND:
        case SLANG_RESOURCE_ACCESS_CONSUME:
        case SLANG_RESOURCE_ACCESS_FEEDBACK:
        case SLANG_RESOURCE_ACCESS_NONE:
        case SLANG_RESOURCE_ACCESS_UNKNOWN:
            // Safe Default I guess?
            return ShaderResourceAccess::READ;
            break;
        }
    }

    std::string shaderResourceKindToString(ShaderResourceKind kind)
    {
        switch (kind) {
        case ShaderResourceKind::ConstantBuffer:
            return "ConstantBuffer";
        case ShaderResourceKind::StructuredBuffer:
            return "StructuredBuffer";
        case ShaderResourceKind::Texture1D:
            return "Texture1D";
        case ShaderResourceKind::Texture2D:
            return "Texture2D";
        case ShaderResourceKind::Texture3D:
            return "Texture3D";
        case ShaderResourceKind::TextureCube:
            return "TextureCube";
        case ShaderResourceKind::TextureBuffer:
            return "TextureBuffer";
        case ShaderResourceKind::Sampler:
            return "Sampler";
        case ShaderResourceKind::ByteBuffer:
            return "ByteBuffer";
        case ShaderResourceKind::Unknown:
            return "Unknown";
        }
        return "Unknown";
    }

    std::string shaderBufferAccessToString(ShaderResourceAccess access)
    {
        switch (access) {
        case ShaderResourceAccess::READ:
            return "READ";
        case ShaderResourceAccess::WRITE:
            return "WRITE";
        case ShaderResourceAccess::READ_WRITE:
            return "READ_WRITE";
        }
        return "UNKNOWN";
    }

    ShaderReflectionData extractFromProgram(Slang::ComPtr<slang::IComponentType> program, const EntryPointDescription& entryPointDesc)
    {
        ShaderReflectionData data;

        auto reflection = program->getLayout();
        int paramCount = reflection->getParameterCount();

        for (int i = 0; i < paramCount; ++i) {
            auto param = reflection->getParameterByIndex(i);
            auto name = std::string(param->getName());
            u32 bindingIndex = param->getBindingIndex();
            u32 bindingSet = param->getBindingSpace();

            auto typeRef = param->getType();
            ShaderResourceKind kind = _getResourceKind(typeRef);
            auto acc = _getResourceAccess(typeRef->getResourceAccess());

            ShaderResource res = {};
            data.name = entryPointDesc.name;
            data.type = entryPointDesc.type;

            if (kind != ShaderResourceKind::Unknown) {

                res.name = name;
                res.binding = { .slot = bindingIndex, .space = bindingSet };
                res.kind = kind;
                res.access = acc;
                data.resources.push_back(std::move(res));
            }
        }

        return data;
    }

    void printReflection(const ShaderReflectionData& data)
    {
        DM_CORE_INFO("\n\t==== {0} | {1} Shader ====", data.name, ShaderTypeToString(data.type))

        for (auto res : data.resources) {
            DM_CORE_INFO("\t{0} | Binding ({1},{2})", res.name, res.binding.slot, res.binding.space)
            DM_CORE_INFO("\tTYPE: {}", shaderResourceKindToString(res.kind))
            DM_CORE_INFO("\tACCESS: {}", shaderBufferAccessToString(res.access))
            DM_CORE_INFO("------")
        }
        DM_CORE_INFO("\t================\n")
    }
}

}
