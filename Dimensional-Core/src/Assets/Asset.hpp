#ifndef DM_ASSET_HPP
#define DM_ASSET_HPP
#include "Core/UUID.hpp"
#include <Assets/AssetMeta.hpp>
#include <core.hpp>

namespace Dimensional {

using AssetHandle = UUID;

static UMap<std::string, AssetType> s_StringToTypeMap = {
    { "NONE", AssetType::NONE },
    { "SHADER", AssetType::SHADER },
    { "TEXTURE", AssetType::TEXTURE },
    { "MODEL", AssetType::MODEL },
    { "MODELSOURCE", AssetType::MODELSOURCE },
    { "MATERIAL", AssetType::MATERIAL },
};

class DMCORE_API Asset {
public:
    AssetHandle handle;

    virtual AssetType getAssetType() const = 0;

    static AssetType assetStringToType(std::string type)
    {
        AssetType t = s_StringToTypeMap[type];
        return t;
    }

    static std::string assetTypeToString(AssetType type)
    {
        switch (type) {
        case AssetType::NONE:
            return "NONE";
        case AssetType::SHADER:
            return "SHADER";
        case AssetType::TEXTURE:
            return "TEXTURE";
        case AssetType::MODEL:
            return "MODEL";
        case AssetType::MODELSOURCE:
            return "MODELSOURCE";
        case AssetType::MATERIAL:
            return "MATERIAL";
        }
        return "NONE";
    }
};

}
#endif
