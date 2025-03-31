#include "Rendering/TextureEnums.hpp"

#include <glad.h>
namespace Dimensional {

static UMap<TextureFormat, GLenum> TextureFormatToGLFormat = {
    // Single Channel
    { TextureFormat::R8, GL_RED },
    { TextureFormat::R16, GL_RED },
    { TextureFormat::R32F, GL_RED },

    // Two Channel
    { TextureFormat::RG8, GL_RG },
    { TextureFormat::RG16, GL_RG },
    { TextureFormat::RG16F, GL_RG },
    { TextureFormat::RG32F, GL_RG },

    // Three Channel
    { TextureFormat::RGB8, GL_RGB },
    { TextureFormat::RGB16, GL_RGB },
    { TextureFormat::RGB16F, GL_RGB },
    { TextureFormat::RGB32F, GL_RGB },

    // Four Channel
    { TextureFormat::RGBA8, GL_RGBA },
    { TextureFormat::RGBA16, GL_RGBA },
    { TextureFormat::RGBA16F, GL_RGBA },
    { TextureFormat::RGBA32F, GL_RGBA },

    // Depth / Stencil Formats
    { TextureFormat::DEPTH16, GL_DEPTH_COMPONENT },
    { TextureFormat::DEPTH32F, GL_DEPTH_COMPONENT },
    { TextureFormat::DEPTH24_STENCIL8, GL_DEPTH_STENCIL },
    { TextureFormat::DEPTH32F_STENCIL8, GL_DEPTH_STENCIL }
};

static UMap<TextureFormat, GLenum> TextureFormatToGLDataType = {
    // Single Channel
    { TextureFormat::R8, GL_UNSIGNED_BYTE },
    { TextureFormat::R16, GL_UNSIGNED_SHORT },
    { TextureFormat::R32F, GL_FLOAT },

    // Two Channel
    { TextureFormat::RG8, GL_UNSIGNED_BYTE },
    { TextureFormat::RG16, GL_UNSIGNED_SHORT },
    { TextureFormat::RG16F, GL_HALF_FLOAT },
    { TextureFormat::RG32F, GL_FLOAT },

    // Three Channel
    { TextureFormat::RGB8, GL_UNSIGNED_BYTE },
    { TextureFormat::RGB16, GL_UNSIGNED_SHORT },
    { TextureFormat::RGB16F, GL_HALF_FLOAT },
    { TextureFormat::RGB32F, GL_FLOAT },

    // Four Channel
    { TextureFormat::RGBA8, GL_UNSIGNED_BYTE },
    { TextureFormat::RGBA16, GL_UNSIGNED_SHORT },
    { TextureFormat::RGBA16F, GL_HALF_FLOAT },
    { TextureFormat::RGBA32F, GL_FLOAT },

    // Depth / Stencil Formats
    { TextureFormat::DEPTH16, GL_UNSIGNED_SHORT },
    { TextureFormat::DEPTH32F, GL_FLOAT },
    { TextureFormat::DEPTH24_STENCIL8, GL_UNSIGNED_INT },
    { TextureFormat::DEPTH32F_STENCIL8, GL_FLOAT }
};

static UMap<TextureFormat, GLenum> TextureFormatToGLInternalFormat = {
    // Single Channel
    { TextureFormat::R8, GL_R8 },
    { TextureFormat::R16, GL_R16 },
    { TextureFormat::R32F, GL_R32F },

    // Two Channel
    { TextureFormat::RG8, GL_RG8 },
    { TextureFormat::RG16, GL_RG16 },
    { TextureFormat::RG16F, GL_RG16F },
    { TextureFormat::RG32F, GL_RG32F },

    // Three Channel
    { TextureFormat::RGB8, GL_RGB8 },
    { TextureFormat::RGB16, GL_RGB16 },
    { TextureFormat::RGB16F, GL_RGB16F },
    { TextureFormat::RGB32F, GL_RGB32F },

    // Four Channel
    { TextureFormat::RGBA8, GL_RGBA8 },
    { TextureFormat::RGBA16, GL_RGBA16 },
    { TextureFormat::RGBA16F, GL_RGBA16F },
    { TextureFormat::RGBA32F, GL_RGBA32F },

    // Depth / Stencil Formats
    { TextureFormat::DEPTH16, GL_DEPTH_COMPONENT16 },
    { TextureFormat::DEPTH32F, GL_DEPTH_COMPONENT32F },
    { TextureFormat::DEPTH24_STENCIL8, GL_DEPTH24_STENCIL8 },
    { TextureFormat::DEPTH32F_STENCIL8, GL_DEPTH32F_STENCIL8 }
};

static UMap<TextureFiltering, GLenum> TextureFilterToGL = {
    { TextureFiltering::Nearest, GL_NEAREST },
    { TextureFiltering::Linear, GL_LINEAR },
    { TextureFiltering::LinearMipmap, GL_LINEAR_MIPMAP_NEAREST },
    { TextureFiltering::NearestMipmap, GL_NEAREST_MIPMAP_NEAREST },
};

static UMap<TextureWrapMode, GLenum> TextureWrapToGL = {
    { TextureWrapMode::Repeat, GL_REPEAT },
    { TextureWrapMode::MirroredRepeat, GL_MIRRORED_REPEAT },
    { TextureWrapMode::ClampToEdge, GL_CLAMP_TO_EDGE },
    { TextureWrapMode::ClampToBorder, GL_CLAMP_TO_BORDER }
};

}
