#ifndef DM_RENDERING_ENUMS
#define DM_RENDERING_ENUMS
#include "nvrhi/nvrhi.h"
namespace Dimensional {

enum class Format {

    Unknown,

    // Unsigned normalized
    R8UN,
    RG8UN,
    RGBA8UN,
    BGRA8UN,

    // Signed normalized
    R8SN,
    RG8SN,
    RGBA8SN,

    // Float formats
    R16F,
    RG16F,
    RGBA16F,
    R32F,
    RG32F,
    RGBA32F,

    // Unsigned integer
    R32U,
    RG32U,
    RGBA32U,

    // Signed integer
    R32I,
    RG32I,
    RGBA32I,

    // Depth/stencil
    D16,
    D24S8,
    D32F,
    D32S8,

    // sRGB
    RGBA8SRGB,
    BGRA8SRGB
};

inline std::string NVRHIFormatToString(nvrhi::Format format)
{
    using enum Format;

    switch (format) {
    case nvrhi::Format::UNKNOWN:
        return "UNKNOWN";
    case nvrhi::Format::R8_UINT:
        return "R8_UINT";
    case nvrhi::Format::R8_SINT:
        return "R8_SINT";
    case nvrhi::Format::R8_UNORM:
        return "R8_UNORM";
    case nvrhi::Format::R8_SNORM:
        return "RG8_UINT";
    case nvrhi::Format::RG8_UINT:
        return "RG8_SINT";
    case nvrhi::Format::RG8_SINT:
        return "RG8_SINT";
    case nvrhi::Format::RG8_UNORM:
        return "RG8_UNORM";
    case nvrhi::Format::RG8_SNORM:
        return "R16_UINT";
    case nvrhi::Format::R16_UINT:
        return "R16_SINT";
    case nvrhi::Format::R16_SINT:
        return "R16_SINT";
    case nvrhi::Format::R16_UNORM:
        return "R16_UNORM";
    case nvrhi::Format::R16_SNORM:
        return "R16_SNORM";
    case nvrhi::Format::R16_FLOAT:
        return "R16_FLOAT";
    case nvrhi::Format::BGRA4_UNORM:
        return "BGRA4_UNORM";
    case nvrhi::Format::B5G6R5_UNORM:
        return "B5G6R5_UNORM";
    case nvrhi::Format::B5G5R5A1_UNORM:
        return "B5G5R5A1_UNORM";
    case nvrhi::Format::RGBA8_UINT:
        return "RGBA8_UINT";
    case nvrhi::Format::RGBA8_SINT:
        return "RGBA8_SINT";
    case nvrhi::Format::RGBA8_UNORM:
        return "RGBA8_UNORM";
    case nvrhi::Format::RGBA8_SNORM:
        return "RGBA8_SNORM";
    case nvrhi::Format::BGRA8_UNORM:
        return "BGRA8_UNORM";
    case nvrhi::Format::SRGBA8_UNORM:
        return "SRGBA8_UNORM";
    case nvrhi::Format::SBGRA8_UNORM:
        return "SBGRA8_UNORM";
    case nvrhi::Format::R10G10B10A2_UNORM:
        return "R10G10B10A2_UNORM";
    case nvrhi::Format::R11G11B10_FLOAT:
        return "R11G11B10_FLOAT";
    case nvrhi::Format::RG16_UINT:
        return "RG16_UINT";
    case nvrhi::Format::RG16_SINT:
        return "RG16_SINT";
    case nvrhi::Format::RG16_UNORM:
        return "RG16_UNORM";
    case nvrhi::Format::RG16_SNORM:
        return "RG16_SNORM";
    case nvrhi::Format::RG16_FLOAT:
        return "RG16_FLOAT";
    case nvrhi::Format::R32_UINT:
        return "R32_UINT";
    case nvrhi::Format::R32_SINT:
        return "R32_SINT";
    case nvrhi::Format::R32_FLOAT:
        return "R32_FLOAT";
    case nvrhi::Format::RGBA16_UINT:
        return "RGBA16_UINT";
    case nvrhi::Format::RGBA16_SINT:
        return "RGBA16_SINT";
    case nvrhi::Format::RGBA16_FLOAT:
        return "RGBA16_FLOAT";
    case nvrhi::Format::RGBA16_UNORM:
        return "RGBA16_UNORM";
    case nvrhi::Format::RGBA16_SNORM:
        return "RGBA16_SNORM";
    case nvrhi::Format::RG32_UINT:
        return "RG32_UINT";
    case nvrhi::Format::RG32_SINT:
        return "RG32_SINT";
    case nvrhi::Format::RG32_FLOAT:
        return "RG32_FLOAT";
    case nvrhi::Format::RGB32_UINT:
        return "RGB32_UINT";
    case nvrhi::Format::RGB32_SINT:
        return "RGB32_SINT";
    case nvrhi::Format::RGB32_FLOAT:
        return "RGB32_FLOAT";
    case nvrhi::Format::RGBA32_UINT:
        return "RGBA32_UINT";
    case nvrhi::Format::RGBA32_SINT:
        return "RGBA32_SINT";
    case nvrhi::Format::RGBA32_FLOAT:
        return "RGBA32_FLOAT";
    case nvrhi::Format::D16:
        return "D16";
    case nvrhi::Format::D24S8:
        return "D24S8";
    case nvrhi::Format::X24G8_UINT:
        return "X24G8_UINT";
    case nvrhi::Format::D32:
        return "D32";
    case nvrhi::Format::D32S8:
        return "D32S8";
    case nvrhi::Format::X32G8_UINT:
        return "X32G8_UINT";
    case nvrhi::Format::BC1_UNORM:
        return "BC1_UNORM";
    case nvrhi::Format::BC1_UNORM_SRGB:
        return "BC1_UNORM_SRGB";
    case nvrhi::Format::BC2_UNORM:
        return "BC2_UNORM";
    case nvrhi::Format::BC2_UNORM_SRGB:
        return "BC2_UNORM_SRGB";
    case nvrhi::Format::BC3_UNORM:
        return "BC3_UNORM";
    case nvrhi::Format::BC3_UNORM_SRGB:
        return "BC3_UNORM_SRGB";
    case nvrhi::Format::BC4_UNORM:
        return "BC4_UNORM";
    case nvrhi::Format::BC4_SNORM:
        return "BC4_SNORM";
    case nvrhi::Format::BC5_UNORM:
        return "BC5_UNORM";
    case nvrhi::Format::BC5_SNORM:
        return "BC5_SNORM";
    case nvrhi::Format::BC6H_UFLOAT:
        return "BC6H_UFLOAT";
    case nvrhi::Format::BC6H_SFLOAT:
        return "BC6H_SFLOAT";
    case nvrhi::Format::BC7_UNORM:
        return "BC7_UNORM";
    case nvrhi::Format::BC7_UNORM_SRGB:
        return "BC7_UNORM_SRGB";
    case nvrhi::Format::COUNT:
        return "COUNT";
        break;
    }
}

inline nvrhi::Format ConvertToNVRHIFormat(Format format)
{
    using enum Format;

    switch (format) {
    case Unknown:
        return nvrhi::Format::UNKNOWN;
    case R8UN:
        return nvrhi::Format::R8_UNORM;
    case RG8UN:
        return nvrhi::Format::RG8_UNORM;
    case RGBA8UN:
        return nvrhi::Format::RGBA8_UNORM;
    case BGRA8UN:
        return nvrhi::Format::BGRA8_UNORM;
    case R8SN:
        return nvrhi::Format::R8_SNORM;
    case RG8SN:
        return nvrhi::Format::RG8_SNORM;
    case RGBA8SN:
        return nvrhi::Format::RGBA8_SNORM;

    case R16F:
        return nvrhi::Format::R16_FLOAT;
    case RG16F:
        return nvrhi::Format::RG16_FLOAT;
    case RGBA16F:
        return nvrhi::Format::RGBA16_FLOAT;

    case R32F:
        return nvrhi::Format::R32_FLOAT;
    case RG32F:
        return nvrhi::Format::RG32_FLOAT;
    case RGBA32F:
        return nvrhi::Format::RGBA32_FLOAT;

    case R32U:
        return nvrhi::Format::R32_UINT;
    case RG32U:
        return nvrhi::Format::RG32_UINT;
    case RGBA32U:
        return nvrhi::Format::RGBA32_UINT;

    case R32I:
        return nvrhi::Format::R32_SINT;
    case RG32I:
        return nvrhi::Format::RG32_SINT;
    case RGBA32I:
        return nvrhi::Format::RGBA32_SINT;

    case D16:
        return nvrhi::Format::D16;
    case D24S8:
        return nvrhi::Format::D24S8;
    case D32F:
        return nvrhi::Format::D32;
    case D32S8:
        return nvrhi::Format::D32S8;
    case RGBA8SRGB:
        return nvrhi::Format::SRGBA8_UNORM;
    case BGRA8SRGB:
        return nvrhi::Format::BGRA8_UNORM;

    default:
        return nvrhi::Format::UNKNOWN;
    }
}

enum class FormatType {
    Unknown,

    // Scalars
    Float,
    Int,
    Uint,

    // Vectors
    Vec2,
    Vec3,
    Vec4,

    IVec2,
    IVec3,
    IVec4,

    UVec2,
    UVec3,
    UVec4,
};

inline nvrhi::Format ConvertToNVRHIFormat(FormatType type)
{
    using enum FormatType;

    switch (type) {
    case Float:
        return nvrhi::Format::R32_FLOAT;
    case Int:
        return nvrhi::Format::R32_SINT;
    case Uint:
        return nvrhi::Format::R32_UINT;

    case Vec2:
        return nvrhi::Format::RG32_FLOAT;
    case Vec3:
        return nvrhi::Format::RGB32_FLOAT;
    case Vec4:
        return nvrhi::Format::RGBA32_FLOAT;

    case IVec2:
        return nvrhi::Format::RG32_SINT;
    case IVec3:
        return nvrhi::Format::RGB32_SINT;
    case IVec4:
        return nvrhi::Format::RGBA32_SINT;

    case UVec2:
        return nvrhi::Format::RG32_UINT;
    case UVec3:
        return nvrhi::Format::RGB32_UINT;
    case UVec4:
        return nvrhi::Format::RGBA32_UINT;
    default:
        return nvrhi::Format::UNKNOWN;
    }
}

}
#endif // DM_RENDERING_ENUMS
