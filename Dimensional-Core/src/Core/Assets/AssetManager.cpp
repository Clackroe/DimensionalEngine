#include <Core/Assets/AssetManager.hpp>
#include <Rendering/Material.hpp>
#include <Rendering/Model.hpp>
#include <Rendering/Shader.hpp>
#include <Rendering/Texture.hpp>

namespace Dimensional {
UMap<UUID, Ref<Material>> AssetManager::m_MaterialMap;
UMap<UUID, Ref<Model>> AssetManager::m_ModelMap;
UMap<UUID, Ref<Texture>> AssetManager::m_TextureMap;
UMap<UUID, Ref<Shader>> AssetManager::m_ShaderMap;

Ref<Material> AssetManager::loadMaterial(MaterialSettings matSettings)
{
    Ref<Material> m = CreateRef<Material>(matSettings);
    m_MaterialMap[m->assetHandle] = m;
    return m;
}
Ref<Model> AssetManager::loadModel(std::string path)
{
    Ref<Model> m = CreateRef<Model>(path);
    m_ModelMap[m->assetHandle] = m;
    return m;
}
Ref<Texture> AssetManager::loadTexture(std::string path)
{
    Ref<Texture> t = CreateRef<Texture>(path);
    m_TextureMap[t->assetHandle] = t;
    return t;
}
Ref<Shader> AssetManager::loadShader(std::string path)
{
    Ref<Shader> s = CreateRef<Shader>(path);
    m_ShaderMap[s->assetHandle] = s;
    return s;
}
Ref<Shader> AssetManager::loadShader(std::string vertexPath, std::string fragPath)
{
    Ref<Shader> s = CreateRef<Shader>(vertexPath, fragPath);
    m_ShaderMap[s->assetHandle] = s;
    return s;
}

Ref<Material> AssetManager::getMaterial(std::string name)
{
    for (const auto& [id, material] : m_MaterialMap) {
        if (material->name == name) {
            return material;
        }
    }
    return nullptr;
}
Ref<Model> AssetManager::getModel(std::string name)
{
    for (const auto& [id, model] : m_ModelMap) {
        if (model->name == name) {
            return model;
        }
    }
    return nullptr;
}
Ref<Texture> AssetManager::getTexture(std::string name)
{
    for (const auto& [id, texture] : m_TextureMap) {
        if (texture->name == name) {
            return texture;
        }
    }
    return nullptr;
}
Ref<Shader> AssetManager::getShader(std::string name)
{
    for (const auto& [id, shader] : m_ShaderMap) {
        if (shader->name == name) {
            return shader;
        }
    }
    return nullptr;
}

}
