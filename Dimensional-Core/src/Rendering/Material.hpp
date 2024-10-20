#ifndef DM_MATERIAL_H
#define DM_MATERIAL_H
#include "Asset/Asset.hpp"
#include "Asset/AssetMeta.hpp"
#include <Rendering/Shader.hpp>
#include <Rendering/Texture.hpp>
namespace Dimensional {

enum MaterialTexture {
    Albedo = 0,
    Normal = 1,
    Metalness = 2,
    Roughness = 3,
    AO = 4
};

struct DMCORE_API MaterialSettings {
    AssetHandle Albedo = 0;
    AssetHandle Normal = 0;
    AssetHandle Metalness = 0;
    AssetHandle Roughness = 0;
    AssetHandle AO = 0;
};

class DMCORE_API Material : public Asset {
public:
    Material();
    Material(MaterialSettings settings);
    ~Material() = default;

    void bind();

    // TODO: Set to return acutal shader
    const Ref<Shader> getShader() { return s_DefaultPBRShader; };

    void setTexture(MaterialTexture slot, AssetHandle textureHandle);

    AssetHandle getTexture(MaterialTexture slot);

    virtual AssetType getAssetType() const override { return AssetType::MATERIAL; };

    MaterialSettings& getSettings() { return m_Settings; };

private:
    // For now, all meshes will utilize the same shader. This will be changed in the future most likely

    void tryInitDefaultTextures();

    static Ref<Texture> s_WhiteTexture;
    static Ref<Texture> s_BlackTexture;

    // TODO: Needs to me smarter, should keep track of its own unique shader, once
    // the engine supports that.
    static Ref<Shader> s_DefaultPBRShader;

    MaterialSettings m_Settings;
};
}
#endif
