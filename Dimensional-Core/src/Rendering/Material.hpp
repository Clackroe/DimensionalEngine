#ifndef DM_MATERIAL_H
#define DM_MATERIAL_H
#include "Assets/Asset.hpp"
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

struct MaterialSettings {
    Ref<Texture> Albedo;
    Ref<Texture> Normal;
    Ref<Texture> Metalness;
    Ref<Texture> Roughness;
    Ref<Texture> AO;
};

class DMCORE_API Material : public Asset {
public:
    Material();
    Material(MaterialSettings settings, std::string name);
    ~Material() = default;

    void bind(Ref<Shader> shad);

    void setTexture(MaterialTexture slot, Ref<Texture> tex);
    Ref<Texture> getTexture(MaterialTexture slot);

private:
    // For now, all meshes will utilize the same shader. This will be changed in the future most likely

    Ref<Texture> m_AlbedoTexture; // 0
    Ref<Texture> m_NormalTexture; // 1
    Ref<Texture> m_MetalnessTexture; // 2
    Ref<Texture> m_RoughnessTexture; // 3
    Ref<Texture> m_AOTexture; // 4

    static Ref<Texture> s_WhiteTexture;
    static Ref<Texture> s_BlackTexture;
};
}
#endif
