#ifndef DM_TEXTURE_H
#define DM_TEXTURE_H
#include <Core/Assets/Asset.hpp>
#include <core.hpp>

namespace Dimensional {

class Texture : public Asset {
public:
    Texture(std::string path);
    void bind(u32 textureSlot);

private:
    void load(std::string path);

    std::string m_Path;

    u32 m_Width, m_Height, m_Channels;
    u32 m_GLId;

    // Add more member vars to support alternative formats
};
}

#endif
