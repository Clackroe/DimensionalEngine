#include "Asset/Asset.hpp"
#include "Asset/AssetManager.hpp"
#include "Log/log.hpp"
#include "Rendering/Texture.hpp"
#include "Scene/Scene.hpp"
#include <Rendering/Material.hpp>

namespace Dimensional {
Ref<Texture> Material::s_WhiteTexture;
Ref<Texture> Material::s_BlackTexture;

void Material::tryInitDefaultTextures()
{
    if (!s_WhiteTexture) {
        u32 data = 0x808080;
        TextureLoadSettings t;
        t.width = 1;
        t.height = 1;
        t.channels = 4;
        s_WhiteTexture = CreateRef<Texture>(t, &data, sizeof(u32));
    }
    if (!s_BlackTexture) {
        u32 data = 0x00000000;
        TextureLoadSettings t;
        t.width = 1;
        t.height = 1;
        t.channels = 4;
        s_BlackTexture = CreateRef<Texture>(t, &data, sizeof(u32));
    }
}

Material::Material()
{
    tryInitDefaultTextures();
}

Material::Material(MaterialSettings settings)
{
    tryInitDefaultTextures();
    if (settings.Albedo) {

        m_Settings.Albedo = settings.Albedo;
    } else {
        m_Settings.Albedo = 0;
    }

    if (settings.Normal) {
        m_Settings.Normal = settings.Normal;
    } else {
        m_Settings.Normal = 0;
    }

    if (settings.Metalness) {
        m_Settings.Metalness = settings.Metalness;
    } else {
        m_Settings.Metalness = 0;
    }

    if (settings.Roughness) {
        m_Settings.Roughness = settings.Roughness;
    } else {
        m_Settings.Roughness = 0;
    }

    if (settings.AO) {
        m_Settings.AO = settings.AO;
    } else {
        m_Settings.AO = 0;
    }
}
AssetHandle Material::getTexture(MaterialTexture slot)
{
    switch (slot) {
    case (MaterialTexture::Albedo):
        return m_Settings.Albedo;
        break;
    case (MaterialTexture::Normal):
        return m_Settings.Normal;
        break;
    case (MaterialTexture::Metalness):
        return m_Settings.Metalness;
        break;
    case (MaterialTexture::Roughness):
        return m_Settings.Roughness;
        break;
    case (MaterialTexture::AO):
        return m_Settings.AO;
        break;
    default:
        DM_CORE_ERROR("ERROR");
        break;
    }
}

void Material::setTexture(MaterialTexture slot, AssetHandle textureHandle)
{
    switch (slot) {
    case (MaterialTexture::Albedo):
        m_Settings.Albedo = textureHandle;
        break;
    case (MaterialTexture::Normal):
        m_Settings.Normal = textureHandle;
        break;
    case (MaterialTexture::Metalness):
        m_Settings.Metalness = textureHandle;
        break;
    case (MaterialTexture::Roughness):
        m_Settings.Roughness = textureHandle;
        break;
    case (MaterialTexture::AO):
        m_Settings.AO = textureHandle;
        break;
    }
}

void Material::bind(Ref<Shader> shad)
{
    shad->use();

    AssetManager& manager = AssetManager::getInstance();

    auto alb = manager.getAsset<Texture>(m_Settings.Albedo);
    if (alb) {
        alb->bind(MaterialTexture::Albedo);
    } else {
        s_WhiteTexture->bind(MaterialTexture::Albedo);
    }

    if (m_Settings.Normal) {
        auto norm = manager.getAsset<Texture>(m_Settings.Normal);
        if (norm) {
            norm->bind(MaterialTexture::Normal);
        }
    }

    auto metal = manager.getAsset<Texture>(m_Settings.Metalness);
    if (metal) {
        metal->bind(MaterialTexture::Metalness);
    } else {
        // s_WhiteTexture->bind(MaterialTexture::Metalness);
        s_BlackTexture->bind(MaterialTexture::Metalness);
    }

    auto rough = manager.getAsset<Texture>(m_Settings.Roughness);
    if (rough) {
        rough->bind(MaterialTexture::Roughness);
    } else {
        s_WhiteTexture->bind(MaterialTexture::Roughness);
    }

    auto ao = manager.getAsset<Texture>(m_Settings.AO);
    if (ao) {
        ao->bind(MaterialTexture::AO);
    } else {
        s_WhiteTexture->bind(MaterialTexture::AO);
    }

    shad->setInt("albedoMap", MaterialTexture::Albedo);
    shad->setInt("normalMap", MaterialTexture::Normal);
    if (!m_Settings.Normal) {
        shad->setBool("uShouldUseNormalMap", false);
    } else {
        shad->setBool("uShouldUseNormalMap", true);
    }
    shad->setInt("metallicMap", MaterialTexture::Metalness);
    shad->setInt("roughnessMap", MaterialTexture::Roughness);
    shad->setInt("aoMap", MaterialTexture::AO);
}
}
