#ifndef DM_ASSETMETA_HPP
#define DM_ASSETMETA_HPP
#include <core.hpp>
namespace Dimensional {
enum class AssetType : u32 {
    NONE = 0,
    TEXTURE = 1,
    MATERIAL = 2,
    MODEL = 3,
    MODELSOURCE = 5,
    SHADER = 4
};

struct AssetMetaData {
    std::string sourcePath = "NONE";
    AssetType type = AssetType::NONE;
};

}

#endif
