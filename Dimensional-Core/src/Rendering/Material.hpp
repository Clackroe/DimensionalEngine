#ifndef DM_MATERIAL_H
#define DM_MATERIAL_H
#include "Asset/Asset.hpp"
#include "Asset/AssetMeta.hpp"
#include "glm/ext/vector_float3.hpp"
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
    glm::vec3 color = glm::vec3(1.0f);

    AssetHandle Normal = 0;

    AssetHandle Metalness = 0;
    float metalnessMult = 1.0f;
    bool useMetalMap = true;

    AssetHandle Roughness = 0;
    float roughnessMult = 1.0f;
    bool useRoughnessMap = true;

    AssetHandle AO = 0;
};

class DMCORE_API Material : public Asset {
public:
    Material();
    Material(MaterialSettings settings);
    ~Material() = default;

    void bind(bool bindShader = true);

    // TODO: Set to return acutal shader
    const Ref<Shader> getShader() { return s_DefaultPBRShader; };

    void setTexture(MaterialTexture slot, AssetHandle textureHandle);
    void setColor(glm::vec3 color) { m_Settings.color = color; };
    void setUseMetalness(bool value) { m_Settings.useMetalMap = value; };
    void setMetalness(float value) { m_Settings.metalnessMult = value; };

    void setUseRoughness(bool value) { m_Settings.useRoughnessMap = value; };
    void setRoughness(float value) { m_Settings.roughnessMult = value; };

    glm::vec3 getColor() { return m_Settings.color; };
    float getUseMetalMap() { return m_Settings.useMetalMap; };
    float getMetalness() { return m_Settings.metalnessMult; };

    float getUseRoughMap() { return m_Settings.useRoughnessMap; };
    float getRoughness() { return m_Settings.roughnessMult; };

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
