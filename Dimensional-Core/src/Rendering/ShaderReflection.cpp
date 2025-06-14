#include "Log/log.hpp"
#include "Rendering/ShaderHelpersAndEnums.hpp"
#include "nvrhi/nvrhi.h"
#include "nvrhi/utils.h"
#include <Rendering/ShaderCompiler.hpp>
#include <Rendering/ShaderReflection.hpp>
#include <algorithm>
#include <functional>
#include <slang/slang.h>
#include <vector>

namespace Dimensional {

namespace ShaderReflector {

    static nvrhi::Format _mapSlangTypeToNvrhiFormat(slang::TypeReflection* type)
    {
        if (!type) {
            return nvrhi::Format::UNKNOWN;
        }

        auto kind = type->getKind();

        switch (kind) {
        case slang::TypeReflection::Kind::Scalar: {
            auto scalarType = type->getScalarType();
            switch (scalarType) {
            case slang::TypeReflection::ScalarType::Float32:
                return nvrhi::Format::R32_FLOAT;
            case slang::TypeReflection::ScalarType::Int32:
                return nvrhi::Format::R32_SINT;
            case slang::TypeReflection::ScalarType::UInt32:
                return nvrhi::Format::R32_UINT;
            case slang::TypeReflection::ScalarType::Float16:
                return nvrhi::Format::R16_FLOAT;
            case slang::TypeReflection::ScalarType::Int16:
                return nvrhi::Format::R16_SINT;
            case slang::TypeReflection::ScalarType::UInt16:
                return nvrhi::Format::R16_UINT;
            case slang::TypeReflection::ScalarType::Int8:
                return nvrhi::Format::R8_SINT;
            case slang::TypeReflection::ScalarType::UInt8:
                return nvrhi::Format::R8_UINT;
            default:
                return nvrhi::Format::UNKNOWN;
            }
        }

        case slang::TypeReflection::Kind::Vector: {
            auto elementType = type->getElementType();
            uint32_t elementCount = type->getElementCount();

            if (elementType->getKind() == slang::TypeReflection::Kind::Scalar) {
                auto scalarType = elementType->getScalarType();

                // Float vectors
                if (scalarType == slang::TypeReflection::ScalarType::Float32) {
                    switch (elementCount) {
                    case 2:
                        return nvrhi::Format::RG32_FLOAT;
                    case 3:
                        return nvrhi::Format::RGB32_FLOAT;
                    case 4:
                        return nvrhi::Format::RGBA32_FLOAT;
                    default:
                        return nvrhi::Format::UNKNOWN;
                    }
                }
                // Int vectors
                else if (scalarType == slang::TypeReflection::ScalarType::Int32) {
                    switch (elementCount) {
                    case 2:
                        return nvrhi::Format::RG32_SINT;
                    case 3:
                        return nvrhi::Format::RGB32_SINT;
                    case 4:
                        return nvrhi::Format::RGBA32_SINT;
                    default:
                        return nvrhi::Format::UNKNOWN;
                    }
                }
                // UInt vectors
                else if (scalarType == slang::TypeReflection::ScalarType::UInt32) {
                    switch (elementCount) {
                    case 2:
                        return nvrhi::Format::RG32_UINT;
                    case 3:
                        return nvrhi::Format::RGB32_UINT;
                    case 4:
                        return nvrhi::Format::RGBA32_UINT;
                    default:
                        return nvrhi::Format::UNKNOWN;
                    }
                }
                // Float16 vectors
                else if (scalarType == slang::TypeReflection::ScalarType::Float16) {
                    switch (elementCount) {
                    case 2:
                        return nvrhi::Format::RG16_FLOAT;
                    case 4:
                        return nvrhi::Format::RGBA16_FLOAT;
                    default:
                        return nvrhi::Format::UNKNOWN;
                    }
                }
                // Int16 vectors
                else if (scalarType == slang::TypeReflection::ScalarType::Int16) {
                    switch (elementCount) {
                    case 2:
                        return nvrhi::Format::RG16_SINT;
                    case 4:
                        return nvrhi::Format::RGBA16_SINT;
                    default:
                        return nvrhi::Format::UNKNOWN;
                    }
                }
                // UInt16 vectors
                else if (scalarType == slang::TypeReflection::ScalarType::UInt16) {
                    switch (elementCount) {
                    case 2:
                        return nvrhi::Format::RG16_UINT;
                    case 4:
                        return nvrhi::Format::RGBA16_UINT;
                    default:
                        return nvrhi::Format::UNKNOWN;
                    }
                }
                // Int8 vectors
                else if (scalarType == slang::TypeReflection::ScalarType::Int8) {
                    switch (elementCount) {
                    case 2:
                        return nvrhi::Format::RG8_SINT;
                    case 4:
                        return nvrhi::Format::RGBA8_SINT;
                    default:
                        return nvrhi::Format::UNKNOWN;
                    }
                }
                // UInt8 vectors
                else if (scalarType == slang::TypeReflection::ScalarType::UInt8) {
                    switch (elementCount) {
                    case 2:
                        return nvrhi::Format::RG8_UINT;
                    case 4:
                        return nvrhi::Format::RGBA8_UINT;
                    default:
                        return nvrhi::Format::UNKNOWN;
                    }
                }
            }
            return nvrhi::Format::UNKNOWN;
        }

        case slang::TypeReflection::Kind::Matrix: {
            // Matrices are typically flattened to multiple vectors in vertex input
            // For now, return UNKNOWN as matrices need special handling
            return nvrhi::Format::UNKNOWN;
        }

        default:
            return nvrhi::Format::UNKNOWN;
        }
    }

    static ShaderResourceKind _getResourceKind(slang::TypeReflection* ref)
    {

        auto kind = ref->getKind();

        if (kind == slang::TypeReflection::Kind::ConstantBuffer) {
            return ShaderResourceKind::ConstantBuffer;
        }

        if (kind == slang::TypeReflection::Kind::SamplerState) {
            return ShaderResourceKind::Sampler;
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

        auto entry = reflection->getEntryPointByIndex(0); // Should only have one
        for (uint32_t i = 0; i < entry->getParameterCount(); ++i) {
            auto param = entry->getParameterByIndex(i);
            if (param->getCategory() == slang::ParameterCategory::VertexInput) {
                slang::TypeReflection* type = param->getType();
                slang::VariableLayoutReflection* layout = param->getPendingDataLayout();

                VertexInput input;
                input.name = param->getName();
                u64 inputSize = 0;
                if (type->getKind() == slang::TypeReflection::Kind::Struct) {

                    for (uint32_t m = 0; m < type->getFieldCount(); ++m) {
                        auto field = type->getFieldByIndex(m);
                        nvrhi::Format format = _mapSlangTypeToNvrhiFormat(field->getType());
                        u64 size = GetFormatSize(format);

                        ShaderVertexAttribute att;
                        att.offset = inputSize;
                        att.size = size;
                        att.name = field->getName();
                        att.format = format;

                        inputSize += size;
                        input.attributes.push_back(std::move(att));
                    }
                    input.size = inputSize;
                    data.vertexInputs.push_back(std::move(input));

                } else {
                    // For scalar inputs, currently unsupported
                }
            }
        }
        return data;
    }

    void printReflection(const ShaderReflectionData& data)
    {
        DM_CORE_INFO("\n\t==== {0} | {1} Shader ====", data.name, ShaderTypeToString(data.type))

        for (auto input : data.vertexInputs) {
            DM_CORE_INFO("Input: {}", input.name);
            for (auto att : input.attributes) {
                DM_CORE_INFO("\t{0}, Offset: {1}, Size {2}, Format: {3}", att.name, att.offset, att.size, nvrhi::utils::FormatToString(att.format));
            }
            DM_CORE_INFO("Size: {}", input.size);
        }

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
