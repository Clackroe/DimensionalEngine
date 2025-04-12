#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include "Rendering/Enums.hpp"
#include "nvrhi/nvrhi.h"
namespace Dimensional {

struct TextureCreateInfo {
    u32 width = 1, height = 1;
    Format format = Format::RGBA32F;

    std::string debugName;
};

struct Texture {

protected:
    Texture() = default;
    nvrhi::TextureHandle m_Handle;
};

struct Texture2D {

    static Ref<Texture2D> Create();

private:
    Texture2D() = default;
    nvrhi::TextureHandle m_Handle;
};

}

#endif // TEXTURE_HPP
