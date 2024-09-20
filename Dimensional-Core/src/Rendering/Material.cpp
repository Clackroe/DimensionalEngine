#include "Assets/Asset.hpp"
#include <Rendering/Material.hpp>

namespace Dimensional {
Ref<Texture> Material::s_WhiteTexture;
Ref<Texture> Material::s_BlackTexture;

Material::Material()
{
    if (!s_WhiteTexture) {
        // s_WhiteTexture = CreateRef<Texture>(1, 1);
        u32 data = 0x808080;
        // s_WhiteTexture->setData(&data, sizeof(u32));
    }
    if (!s_BlackTexture) {
        // s_BlackTexture = CreateRef<Texture>(1, 1);
        u32 data = 0x00000000;
        // s_BlackTexture->setData(&data, sizeof(u32));
    }

    m_AlbedoTexture = s_WhiteTexture;
    m_NormalTexture = nullptr;
    m_MetalnessTexture = s_BlackTexture;
    m_RoughnessTexture = s_WhiteTexture;
    m_AOTexture = s_WhiteTexture;
}

Material::Material(MaterialSettings settings, std::string name)
{
    if (settings.Albedo) {

        m_AlbedoTexture = settings.Albedo;
    } else {
        m_AlbedoTexture = s_WhiteTexture;
    }

    if (settings.Normal) {
        m_NormalTexture = settings.Normal;
    } else {
        m_NormalTexture = nullptr;
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
Ref<Texture> Material::getTexture(MaterialTexture slot)
{
    switch (slot) {
    case (MaterialTexture::Albedo):
        return m_AlbedoTexture;
        break;
    case (MaterialTexture::Normal):
        return m_NormalTexture;
        break;
    case (MaterialTexture::Metalness):
        return m_MetalnessTexture;
        break;
    case (MaterialTexture::Roughness):
        return m_RoughnessTexture;
        break;
    case (MaterialTexture::AO):
        return m_AOTexture;
        break;
    default:
        DM_CORE_ERROR("ERROR");
        break;
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

    if (m_NormalTexture) {
        m_NormalTexture->bind(MaterialTexture::Normal);
    }
    m_MetalnessTexture->bind(MaterialTexture::Metalness);
    m_RoughnessTexture->bind(MaterialTexture::Roughness);
    m_AOTexture->bind(MaterialTexture::AO);

    shad->setInt("albedoMap", MaterialTexture::Albedo);
    shad->setInt("normalMap", MaterialTexture::Normal);
    if (!m_NormalTexture) {
        shad->setBool("uShouldUseNormalMap", false);
    } else {
        shad->setBool("uShouldUseNormalMap", true);
    }
    shad->setInt("metallicMap", MaterialTexture::Metalness);
    shad->setInt("roughnessMap", MaterialTexture::Roughness);
    shad->setInt("aoMap", MaterialTexture::AO);
}
}
