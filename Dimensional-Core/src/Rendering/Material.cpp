#include "Asset/Asset.hpp"
#include "Asset/AssetManager.hpp"
#include "Log/log.hpp"
#include "Rendering/Texture.hpp"
#include <Rendering/Material.hpp>

namespace Dimensional {
Ref<Texture> Material::s_WhiteTexture;
Ref<Texture> Material::s_BlackTexture;
Ref<Shader> Material::s_DefaultPBRShader;

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
    if (!s_DefaultPBRShader) {
        s_DefaultPBRShader = CreateRef<Shader>("Assets/Shaders/PBRWithLighting.glsl");
    }
}

Material::Material(MaterialSettings settings)
{
    m_Settings = settings;
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

void Material::bind()
{
    if (!s_DefaultPBRShader) {
        s_DefaultPBRShader = CreateRef<Shader>("Assets/Shaders/PBRWithLighting.glsl");
    }

    s_DefaultPBRShader->use();

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

    s_DefaultPBRShader->setInt("albedoMap", MaterialTexture::Albedo);
    s_DefaultPBRShader->setInt("normalMap", MaterialTexture::Normal);
    if (!m_Settings.Normal) {
        s_DefaultPBRShader->setBool("uShouldUseNormalMap", false);
    } else {
        s_DefaultPBRShader->setBool("uShouldUseNormalMap", true);
    }
    s_DefaultPBRShader->setInt("metallicMap", MaterialTexture::Metalness);
    s_DefaultPBRShader->setInt("roughnessMap", MaterialTexture::Roughness);
    s_DefaultPBRShader->setInt("aoMap", MaterialTexture::AO);

    s_DefaultPBRShader->setVec3("uColorValue", m_Settings.color.x, m_Settings.color.y, m_Settings.color.z);
    s_DefaultPBRShader->setFloat("uMetalnessMult", m_Settings.metalnessMult);
    s_DefaultPBRShader->setFloat("uRoughnessMult", m_Settings.roughnessMult);

    s_DefaultPBRShader->setBool("uUseRoughnessMap", m_Settings.useRoughnessMap);
    s_DefaultPBRShader->setBool("uUseMetalMap", m_Settings.useMetalMap);
}
}
