#include "Asset.hpp"

namespace Dimensional {

std::string Asset::generateName(const std::string& path)
{
    std::string name = path;
    size_t lastSlash = name.find_last_of('/');
    if (lastSlash != std::string::npos) {
        name = name.substr(lastSlash + 1, name.length());
    }
    size_t pPos = name.find('.');
    if (pPos != std::string::npos) {
        name = name.substr(0, pPos);
    }
    return name;
}

}
