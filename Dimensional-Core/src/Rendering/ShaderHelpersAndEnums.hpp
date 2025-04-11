#ifndef SHADER_HELPERS_HPP
#define SHADER_HELPERS_HPP
#include "Rendering/Shader.hpp"
#include "nvrhi/nvrhi.h"
#include <spirv_common.hpp>
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

inline shaderc_shader_kind ShaderTypeToShaderC(const ShaderType type)
{
    switch (type) {
    case COMPUTE:
        return shaderc_compute_shader;
    case VERTEX:
        return shaderc_vertex_shader;
    case FRAGMENT:
        return shaderc_fragment_shader;
    case GEOMETRY:
        return shaderc_geometry_shader;
    case NONE:
        return shaderc_geometry_shader;
        break;
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

inline nvrhi::Format getSpirvToNvrhiFormat(const spirv_cross::SPIRType& type)
{
    if (type.basetype == spirv_cross::SPIRType::Float) {
        if (type.vecsize == 1)
            return nvrhi::Format::R32_FLOAT;
        if (type.vecsize == 2)
            return nvrhi::Format::RG32_FLOAT;
        if (type.vecsize == 3)
            return nvrhi::Format::RGB32_FLOAT;
        if (type.vecsize == 4)
            return nvrhi::Format::RGBA32_FLOAT;
    } else if (type.basetype == spirv_cross::SPIRType::Int) {
        if (type.vecsize == 1)
            return nvrhi::Format::R32_SINT;
        if (type.vecsize == 2)
            return nvrhi::Format::RG32_SINT;
        if (type.vecsize == 3)
            return nvrhi::Format::RGB32_SINT;
        if (type.vecsize == 4)
            return nvrhi::Format::RGBA32_SINT;
    } else if (type.basetype == spirv_cross::SPIRType::UInt) {
        if (type.vecsize == 1)
            return nvrhi::Format::R32_UINT;
        if (type.vecsize == 2)
            return nvrhi::Format::RG32_UINT;
        if (type.vecsize == 3)
            return nvrhi::Format::RGB32_UINT;
        if (type.vecsize == 4)
            return nvrhi::Format::RGBA32_UINT;
    }
    return nvrhi::Format::UNKNOWN;
}

inline u32 spirvTypeToSize(const spirv_cross::SPIRType& type)
{

    if (type.basetype == spirv_cross::SPIRType::Float || type.basetype == spirv_cross::SPIRType::Int || type.basetype == spirv_cross::SPIRType::UInt) {
        return 4 * type.vecsize;
    }
    return 0;
}
};

#endif // SHADER_HELPERS_HPP
