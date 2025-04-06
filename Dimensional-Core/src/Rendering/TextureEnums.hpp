#ifndef TEXTURE_ENUMS
#define TEXTURE_ENUMS
namespace Dimensional {
enum class TextureFormat {
    NONE,
    // Single Channel
    R8, // 8-bit red
    R16, // 16-bit red (integer) R16F, // 16-bit floating point red
    R32F, // 32-bit floating point red

    // Two Channel
    RG8, // 8-bit red, green
    RG16, // 16-bit red, green (integer)
    RG16F, // 16-bit floating point red, green
    RG32F, // 32-bit floating point red, green

    // Three Channel
    RGB8, // 8-bit red, green, blue
    RGB16, // 16-bit red, green, blue (integer)
    RGB16F, // 16-bit floating point red, green, blue
    RGB32F, // 32-bit floating point red, green, blue

    // Four Channel
    RGBA8, // 8-bit red, green, blue, alpha
    RGBA16, // 16-bit red, green, blue, alpha (integer)
    RGBA16F, // 16-bit floating point red, green, blue, alpha
    RGBA32F, // 32-bit floating point red, green, blue, alpha

    // Depth / Stencil Formats
    DEPTH16, // 16-bit depth
    DEPTH32F, // 32-bit floating point depth
    DEPTH24_STENCIL8, // 24-bit depth, 8-bit stencil
    DEPTH32F_STENCIL8, // 32-bit floating point depth, 8-bit stencil
    DEFAULT = RGBA16F
};

enum class TextureFiltering {
    Nearest, // No interpolation (pixelated look)
    Linear, // Linear interpolation (smooth look)
    NearestMipmap, // Nearest filtering with mipmaps
    LinearMipmap, // Linear filtering with mipmaps
    DEFAULT = Nearest
};

enum class TextureWrapMode {
    Repeat, // Repeats the texture (tiling)
    MirroredRepeat, // Repeats but mirrors each time
    ClampToEdge, // Clamps UV to the edge of the texture
    ClampToBorder, // Clamps and uses a specified border color
    DEFAULT = ClampToEdge
};

static UMap<TextureFormat, u32> TextureFormatToChannels = {
    // Single Channel
    { TextureFormat::R8, 1 },
    { TextureFormat::R16, 1 },
    { TextureFormat::R32F, 1 },

    // Two Channel
    { TextureFormat::RG8, 2 },
    { TextureFormat::RG16, 2 },
    { TextureFormat::RG16F, 2 },
    { TextureFormat::RG32F, 2 },

    // Three Channel
    { TextureFormat::RGB8, 3 },
    { TextureFormat::RGB16, 3 },
    { TextureFormat::RGB16F, 3 },
    { TextureFormat::RGB32F, 3 },

    // Four Channel
    { TextureFormat::RGBA8, 4 },
    { TextureFormat::RGBA16, 4 },
    { TextureFormat::RGBA16F, 4 },
    { TextureFormat::RGBA32F, 4 },

    // Depth / Stencil Formats
    { TextureFormat::DEPTH16, 1 },
    { TextureFormat::DEPTH32F, 1 },
    { TextureFormat::DEPTH24_STENCIL8, 2 },
    { TextureFormat::DEPTH32F_STENCIL8, 2 }
};
}

#endif // TEXTURE_ENUMS
