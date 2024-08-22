#ifndef DM_ASSET_HPP
#define DM_ASSET_HPP
#include <core.hpp>

namespace uuids {
class uuid;
}
namespace Dimensional {

class Asset {
public:
    const Hash hash;
    const std::string name;

    Asset(const std::string& path)
        : // UID(generateUUID())
        hash(generateHash(path))
        , name(generateName(path))
    {
    }

private:
    static Hash generateHash(const std::string& path);
    std::string generateName(const std::string& path);
};

}
#endif
