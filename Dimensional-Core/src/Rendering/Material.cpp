#include <Rendering/Material.hpp>

namespace Dimensional {
Ref<Texture> Material::s_WhiteTexture;
Ref<Texture> Material::s_BlackTexture;

Material::Material()
    : Asset("Material")
{
    if (!s_WhiteTexture) {
        s_WhiteTexture = CreateRef<Texture>(1, 1);
        u32 data = 0xffffffff;
        s_WhiteTexture->setData(&data, sizeof(u32));
    }
    if (!s_BlackTexture) {
        s_BlackTexture = CreateRef<Texture>(1, 1);
        u32 data = 0x00000000;
        s_BlackTexture->setData(&data, sizeof(u32));
    }

    m_AlbedoTexture = s_WhiteTexture;
    m_NormalTexture = s_WhiteTexture;
    m_MetalnessTexture = s_BlackTexture;
    m_RoughnessTexture = s_WhiteTexture;
    m_AOTexture = s_WhiteTexture;
}

Material::Material(MaterialSettings settings)
    : Asset("Material")
{
    if (settings.Albedo) {

        m_AlbedoTexture = settings.Albedo;
    } else {
        m_AlbedoTexture = s_WhiteTexture;
    }

    if (settings.Normal) {
        m_NormalTexture = settings.Normal;
    } else {
        m_NormalTexture = s_WhiteTexture;
    }

    if (settings.Metalness) {
        m_MetalnessTexture = settings.Metalness;
    } else {
        m_MetalnessTexture = s_BlackTexture;
    }

    if (settings.Roughness) {
        m_RoughnessTexture = settings.Roughness;
    } else {
        m_RoughnessTexture = s_WhiteTexture;
    }

    if (settings.AO) {
        m_AOTexture = settings.AO;
    } else {
        m_AOTexture = s_WhiteTexture;
    }
}
void Material::setTexture(MaterialTexture slot, Ref<Texture> tex)
{
    switch (slot) {
    case (MaterialTexture::Albedo):
        m_AlbedoTexture = tex;
        break;
    case (MaterialTexture::Normal):
        m_NormalTexture = tex;
        break;
    case (MaterialTexture::Metalness):
        m_MetalnessTexture = tex;
        break;
    case (MaterialTexture::Roughness):
        m_RoughnessTexture = tex;
        break;
    case (MaterialTexture::AO):
        m_AOTexture = tex;
        break;
    }
}

void Material::bind(Ref<Shader> shad)
{
    shad->use();
    m_AlbedoTexture->bind(MaterialTexture::Albedo);
    m_NormalTexture->bind(MaterialTexture::Normal);
    m_MetalnessTexture->bind(MaterialTexture::Metalness);
    m_RoughnessTexture->bind(MaterialTexture::Roughness);
    m_AOTexture->bind(MaterialTexture::AO);

    shad->setInt("albedoMap", MaterialTexture::Albedo);
    shad->setInt("normalMap", MaterialTexture::Normal);
    shad->setInt("metallicMap", MaterialTexture::Metalness);
    shad->setInt("roughnessMap", MaterialTexture::Roughness);
    shad->setInt("aoMap", MaterialTexture::AO);
}
}
