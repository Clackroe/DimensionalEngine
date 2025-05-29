#ifndef SHADER_HELPERS_HPP
#define SHADER_HELPERS_HPP
#include "Rendering/Shader.hpp"
#include "nvrhi/nvrhi.h"
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

#endif // SHADER_HELPERS_HPP
