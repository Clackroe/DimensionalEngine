#ifndef DM_ASSET_HPP
#define DM_ASSET_HPP
#include <core.hpp>

namespace uuids {
class uuid;
}
namespace Dimensional {

class Asset {
public:
    // const uuids::uuid UID;

    const Hash hash;

    Asset(const std::string& path)
        : // UID(generateUUID())
        hash(generateHash(path))
    {
    }

    // std::string idAsString() const;

private:
    // static uuids::uuid generateUUID();
    static Hash generateHash(const std::string& path);
};

}
#endif
