#ifndef DM_TEXTURE_H
#define DM_TEXTURE_H
#include <Core/Assets/Asset.hpp>
#include <core.hpp>

namespace Dimensional {

class Texture : public Asset {
public:
    Texture(std::string path);

private:
    std::vector<unsigned char> textureData;
};
}

#endif
