#ifndef DM_TEXTURE_H
#define DM_TEXTURE_H
#include <Core/Assets/Asset.hpp>
#include <core.hpp>

namespace Dimensional {

class DMCORE_API Texture : public Asset {
public:
    Texture(std::string path, bool retainInMemory);
    Texture(u32 width, u32 height);
    ~Texture();
    void bind(u32 textureSlot);

    void setData(void* data, u32 sizeBytes);

    u32 getID() const { return m_GLId; };

private:
    void load(std::string path, bool retainInMemory, bool hdr = false);

    std::string m_Path;

    u32 m_Width, m_Height, m_Channels;
    u32 m_GLId;
    u32 m_IntFormat, m_DataFormat;

    std::vector<u8> m_TextureData;
};
}

#endif
