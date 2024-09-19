#ifndef DM_ASSETMETA_HPP
#define DM_ASSETMETA_HPP
namespace Dimensional {

enum class AssetType {
    NONE = 0,
    TEXTURE = 1,
    MATERIAL = 2,
    MODEL = 3,
    SHADER = 4,
};

struct AssetMetaData {
    std::string sourcePath = "NONE";
    AssetType type = AssetType::NONE;
};

}

#endif
