#ifndef SHADER_HELPERS_HPP
#define SHADER_HELPERS_HPP
#include "Rendering/Shader.hpp"
#include "nvrhi/nvrhi.h"
#include <slang/slang.h>
namespace Dimensional {

inline std::string ShaderTypeToSpecifier(const ShaderType type)
{
    switch (type) {
    case COMPUTE:
        return "##COMPUTE";
    case VERTEX:
        return "##VERTEXSHADER";
    case FRAGMENT:
        return "##FRAGSHADER";
    case GEOMETRY:
        return "##GEOMETRY";
    case NONE:
        break;
    }
    return "";
};

inline std::string ShaderTypeToString(const ShaderType type)
{
    switch (type) {
    case COMPUTE:
        return "COMPUTE";
    case VERTEX:
        return "VERTEXSHADER";
    case FRAGMENT:
        return "FRAGSHADER";
    case GEOMETRY:
        return "GEOMETRY";
    case NONE:
        return "NONE";
    }
};

inline nvrhi::ShaderType ShaderTypeToNVRHI(const ShaderType type)
{
    switch (type) {
    case COMPUTE:
        return nvrhi::ShaderType::Compute;
    case VERTEX:
        return nvrhi::ShaderType::Vertex;
    case FRAGMENT:
        return nvrhi::ShaderType::Pixel;
    case GEOMETRY:
        return nvrhi::ShaderType::Geometry;
    case NONE:
        return nvrhi::ShaderType::None;
        break;
    }
};

inline nvrhi::ShaderType SlangStageToNVRHI(SlangStage stage)
{
    switch (stage) {
    case SLANG_STAGE_VERTEX:
        return nvrhi::ShaderType::Vertex;
    case SLANG_STAGE_PIXEL:
        return nvrhi::ShaderType::Pixel;
    case SLANG_STAGE_COMPUTE:
        return nvrhi::ShaderType::Compute;
    case SLANG_STAGE_GEOMETRY:
        return nvrhi::ShaderType::Geometry;
    case SLANG_STAGE_HULL:
        return nvrhi::ShaderType::Hull;
    case SLANG_STAGE_DOMAIN:
        return nvrhi::ShaderType::Domain;
    default:
        return nvrhi::ShaderType::None;
    }
}

};

inline uint32_t GetFormatSize(nvrhi::Format format)
{
    switch (format) {
    case nvrhi::Format::R32_FLOAT:
    case nvrhi::Format::R32_SINT:
    case nvrhi::Format::R32_UINT:
        return 4;
    case nvrhi::Format::RG32_FLOAT:
    case nvrhi::Format::RG32_SINT:
    case nvrhi::Format::RG32_UINT:
        return 8;
    case nvrhi::Format::RGB32_FLOAT:
    case nvrhi::Format::RGB32_SINT:
    case nvrhi::Format::RGB32_UINT:
        return 12;
    case nvrhi::Format::RGBA32_FLOAT:
    case nvrhi::Format::RGBA32_SINT:
    case nvrhi::Format::RGBA32_UINT:
        return 16;
    case nvrhi::Format::R16_FLOAT:
        return 2;
    case nvrhi::Format::RG16_FLOAT:
        return 4;
    case nvrhi::Format::RGBA16_FLOAT:
        return 8;
    default:
        return 4; // Default fallback
    }
}

inline nvrhi::Format slangTypeToNVRHIFormat(slang::TypeReflection* type)
{
    auto kind = type->getKind();
    auto scalarType = type->getScalarType();
    SlangUInt rowCount = type->getRowCount();
    SlangUInt colCount = type->getColumnCount();

    // Handle vectors and scalars
    if (kind == slang::TypeReflection::Kind::Vector || kind == slang::TypeReflection::Kind::Scalar) {
        if (scalarType == slang::TypeReflection::ScalarType::Float32) {
            if (colCount == 1)
                return nvrhi::Format::R32_FLOAT;
            else if (colCount == 2)
                return nvrhi::Format::RG32_FLOAT;
            else if (colCount == 3)
                return nvrhi::Format::RGB32_FLOAT;
            else if (colCount == 4)
                return nvrhi::Format::RGBA32_FLOAT;
        } else if (scalarType == slang::TypeReflection::ScalarType::Int32) {
            if (colCount == 1)
                return nvrhi::Format::R32_SINT;
            else if (colCount == 2)
                return nvrhi::Format::RG32_SINT;
            else if (colCount == 3)
                return nvrhi::Format::RGB32_SINT;
            else if (colCount == 4)
                return nvrhi::Format::RGBA32_SINT;
        } else if (scalarType == slang::TypeReflection::ScalarType::UInt32) {
            if (colCount == 1)
                return nvrhi::Format::R32_UINT;
            else if (colCount == 2)
                return nvrhi::Format::RG32_UINT;
            else if (colCount == 3)
                return nvrhi::Format::RGB32_UINT;
            else if (colCount == 4)
                return nvrhi::Format::RGBA32_UINT;
        } else if (scalarType == slang::TypeReflection::ScalarType::Float16) {
            if (colCount == 1)
                return nvrhi::Format::R16_FLOAT;
            else if (colCount == 2)
                return nvrhi::Format::RG16_FLOAT;
            else if (colCount == 4)
                return nvrhi::Format::RGBA16_FLOAT;
        }
    }

    DM_CORE_WARN("Unsupported shader input type, defaulting to RGBA32_FLOAT");
    return nvrhi::Format::RGBA32_FLOAT;
}

inline nvrhi::ResourceType slangResourceTypeToNVRHI(slang::TypeReflection* type)
{
    auto kind = type->getKind();
    if (kind == slang::TypeReflection::Kind::Resource) {
        auto shape = type->getResourceShape();
        auto access = type->getResourceAccess();

        // Extract base shape using the mask
        auto baseShape = shape & SLANG_RESOURCE_BASE_SHAPE_MASK;

        // Textures
        if (baseShape == SLANG_TEXTURE_1D || baseShape == SLANG_TEXTURE_2D || baseShape == SLANG_TEXTURE_3D || baseShape == SLANG_TEXTURE_CUBE || baseShape == SLANG_TEXTURE_SUBPASS) {
            if (access & SLANG_RESOURCE_ACCESS_READ_WRITE) {
                return nvrhi::ResourceType::Texture_UAV;
            } else {
                return nvrhi::ResourceType::Texture_SRV;
            }
        }
        // Texture buffer (treated as buffer)
        else if (baseShape == SLANG_TEXTURE_BUFFER) {
            if (access & SLANG_RESOURCE_ACCESS_READ_WRITE) {
                return nvrhi::ResourceType::TypedBuffer_UAV;
            } else {
                return nvrhi::ResourceType::TypedBuffer_SRV;
            }
        }
        // Structured buffers
        else if (baseShape == SLANG_STRUCTURED_BUFFER) {
            if (access & SLANG_RESOURCE_ACCESS_READ_WRITE) {
                return nvrhi::ResourceType::StructuredBuffer_UAV;
            } else {
                return nvrhi::ResourceType::StructuredBuffer_SRV;
            }
        }
        // Byte address buffers
        else if (baseShape == SLANG_BYTE_ADDRESS_BUFFER) {
            if (access & SLANG_RESOURCE_ACCESS_READ_WRITE) {
                return nvrhi::ResourceType::RawBuffer_UAV;
            } else {
                return nvrhi::ResourceType::RawBuffer_SRV;
            }
        }
        // Acceleration structures (for raytracing)
        else if (baseShape == SLANG_ACCELERATION_STRUCTURE) {
            return nvrhi::ResourceType::RayTracingAccelStruct;
        }
    } else if (kind == slang::TypeReflection::Kind::SamplerState) {
        return nvrhi::ResourceType::Sampler;
    }

    return nvrhi::ResourceType::None;
}

#endif // SHADER_HELPERS_HPP
