#ifndef SHADER_HELPERS_HPP
#define SHADER_HELPERS_HPP
#include "Rendering/Shader.hpp"
#include "nvrhi/nvrhi.h"
#include <slang/slang.h>
namespace Dimensional {

// inline std::string ShaderTypeToSpecifier(const ShaderType type)
// {
//     switch (type) {
//     case COMPUTE:
//         return "##COMPUTE";
//     case VERTEX:
//         return "##VERTEXSHADER";
//     case FRAGMENT:
//         return "##FRAGSHADER";
//     case GEOMETRY:
//         return "##GEOMETRY";
//     case NONE:
//         break;
//     }
//     return "";
// };

inline std::string ShaderTypeToString(const nvrhi::ShaderType type)
{
    switch (type) {
    case nvrhi::ShaderType::None:
        return "None";
    case nvrhi::ShaderType::Compute:
        return "Compute";
    case nvrhi::ShaderType::Vertex:
        return "Vertex";
    case nvrhi::ShaderType::Hull:
        return "Hull";
    case nvrhi::ShaderType::Domain:
        return "Domain";
    case nvrhi::ShaderType::Geometry:
        return "Geometry";
    case nvrhi::ShaderType::Pixel:
        return "Pixel";
    case nvrhi::ShaderType::Amplification:
        return "Amplification";
    case nvrhi::ShaderType::Mesh:
        return "Mesh";
    case nvrhi::ShaderType::AllGraphics:
        return "AllGraphics";
    case nvrhi::ShaderType::RayGeneration:
        return "RayGeneration";
    case nvrhi::ShaderType::AnyHit:
        return "AnyHit";
    case nvrhi::ShaderType::ClosestHit:
        return "ClosestHit";
    case nvrhi::ShaderType::Miss:
        return "Miss";
    case nvrhi::ShaderType::Intersection:
        return "Intersection";
    case nvrhi::ShaderType::Callable:
        return "Callable";
    case nvrhi::ShaderType::AllRayTracing:
        return "AllRayTracing";
    case nvrhi::ShaderType::All:
        return "All";
        break;
    }
};

// inline nvrhi::ShaderType ShaderTypeToNVRHI(const ShaderType type)
// {
//     switch (type) {
//     case COMPUTE:
//         return nvrhi::ShaderType::Compute;
//     case VERTEX:
//         return nvrhi::ShaderType::Vertex;
//     case FRAGMENT:
//         return nvrhi::ShaderType::Pixel;
//     case GEOMETRY:
//         return nvrhi::ShaderType::Geometry;
//     case NONE:
//         return nvrhi::ShaderType::None;
//         break;
//     }
// };

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

// Helper function to convert Slang types to NVRHI types
inline nvrhi::Format SlangTypeToNVRHIFormat(slang::TypeReflection* type)
{
    using namespace slang;

    auto kind = type->getKind();
    auto scalarType = type->getScalarType();
    auto rowCount = type->getRowCount();
    auto colCount = type->getColumnCount();

    // Handle scalar types
    if (kind == TypeReflection::Kind::Scalar) {
        switch (scalarType) {
        case TypeReflection::ScalarType::Float32:
            return nvrhi::Format::R32_FLOAT;
        case TypeReflection::ScalarType::Int32:
            return nvrhi::Format::R32_SINT;
        case TypeReflection::ScalarType::UInt32:
            return nvrhi::Format::R32_UINT;
        case TypeReflection::ScalarType::Float16:
            return nvrhi::Format::R16_FLOAT;
        case TypeReflection::ScalarType::Int16:
            return nvrhi::Format::R16_SINT;
        case TypeReflection::ScalarType::UInt16:
            return nvrhi::Format::R16_UINT;
        case TypeReflection::ScalarType::Int8:
            return nvrhi::Format::R8_SINT;
        case TypeReflection::ScalarType::UInt8:
            return nvrhi::Format::R8_UINT;
            break;
        }
    }

    // Handle vector types
    if (kind == TypeReflection::Kind::Vector) {
        if (scalarType == TypeReflection::ScalarType::Float32) {
            switch (colCount) {
            case 2:
                return nvrhi::Format::RG32_FLOAT;
            case 3:
                return nvrhi::Format::RGB32_FLOAT;
            case 4:
                return nvrhi::Format::RGBA32_FLOAT;
            }
        } else if (scalarType == TypeReflection::ScalarType::Int32) {
            switch (colCount) {
            case 2:
                return nvrhi::Format::RG32_SINT;
            case 3:
                return nvrhi::Format::RGB32_SINT;
            case 4:
                return nvrhi::Format::RGBA32_SINT;
            }
        } else if (scalarType == TypeReflection::ScalarType::UInt32) {
            switch (colCount) {
            case 2:
                return nvrhi::Format::RG32_UINT;
            case 3:
                return nvrhi::Format::RGB32_UINT;
            case 4:
                return nvrhi::Format::RGBA32_UINT;
            }
        } else if (scalarType == TypeReflection::ScalarType::Float16) {
            switch (colCount) {
            case 2:
                return nvrhi::Format::RG16_FLOAT;
            case 4:
                return nvrhi::Format::RGBA16_FLOAT;
            }
        }
    }

    // Handle matrix types (treat as multiple vectors)
    if (kind == TypeReflection::Kind::Matrix) {
        if (scalarType == TypeReflection::ScalarType::Float32) {
            if (rowCount == 4 && colCount == 4) {
                return nvrhi::Format::RGBA32_FLOAT; // Per row
            }
        }
    }

    return nvrhi::Format::UNKNOWN;
}

inline nvrhi::ResourceType SlangResourceTypeToNVRHI(slang::TypeReflection* type, slang::BindingType bindingType)
{
    using namespace slang;

    switch (bindingType) {
    case slang::BindingType::ConstantBuffer:
        return nvrhi::ResourceType::ConstantBuffer;
    case slang::BindingType::Texture:
        return nvrhi::ResourceType::Texture_SRV;
    case slang::BindingType::MutableTexture:
        return nvrhi::ResourceType::Texture_UAV;
    case slang::BindingType::Sampler:
        return nvrhi::ResourceType::Sampler;
    case slang::BindingType::CombinedTextureSampler:
        return nvrhi::ResourceType::Texture_SRV;
    case slang::BindingType::RawBuffer:
    case slang::BindingType::MutableRawBuffer:
        return nvrhi::ResourceType::StructuredBuffer_SRV;
    default:
        return nvrhi::ResourceType::None;
    }
}

inline nvrhi::TextureDimension SlangTextureDimensionToNVRHI(slang::TypeReflection* type)
{
    using namespace slang;

    auto shape = type->getResourceShape();
    switch (shape & SLANG_RESOURCE_BASE_SHAPE_MASK) {
    case SLANG_TEXTURE_1D:
        return nvrhi::TextureDimension::Texture1D;
    case SLANG_TEXTURE_2D:
        return nvrhi::TextureDimension::Texture2D;
    case SLANG_TEXTURE_3D:
        return nvrhi::TextureDimension::Texture3D;
    case SLANG_TEXTURE_CUBE:
        return nvrhi::TextureDimension::TextureCube;
    default:
        return nvrhi::TextureDimension::Texture2D;
    }
}

#endif // SHADER_HELPERS_HPP
