#ifndef DM_TEXTURE_H
#define DM_TEXTURE_H
#include <Core/Assets/Asset.hpp>
#include <core.hpp>

namespace Dimensional {

class Texture : public Asset {
public:
    Texture(std::string path, bool retainInMemory);
    ~Texture();
    void bind(u32 textureSlot);

private:
    void load(std::string path, bool retainInMemory);

    std::string m_Path;

    u32 m_Width, m_Height, m_Channels;
    u32 m_GLId;
    u32 m_IntFormat, m_DataFormat;

    std::vector<u8> m_TextureData;
};
}

#endif
