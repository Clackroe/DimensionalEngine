#include <Assets/AssetManager.hpp>
#include <Core/UUID.hpp>
#include <Rendering/Material.hpp>
#include <Rendering/Model.hpp>
#include <Rendering/Shader.hpp>
#include <Rendering/Texture.hpp>

namespace Dimensional {
UMap<UUID, Ref<Material>> AssetManager::m_MaterialMap;
UMap<UUID, Ref<Model>> AssetManager::m_ModelMap;
UMap<UUID, Ref<Texture>> AssetManager::m_TextureMap;
UMap<UUID, Ref<Shader>> AssetManager::m_ShaderMap;

std::vector<std::string> AssetManager::getAllAssetNames(AssetType type)
{
    std::vector<std::string> names;

    switch (type) {
    case AssetType::MaterialType:
        for (const auto& [uuid, material] : m_MaterialMap) {
            names.push_back(material->name);
        }
        break;

    case AssetType::ModelType:
        for (const auto& [uuid, model] : m_ModelMap) {
            names.push_back(model->name);
        }
        break;

    case AssetType::TextureType:
        for (const auto& [uuid, texture] : m_TextureMap) {
            names.push_back(texture->name);
        }
        break;

    case AssetType::ShaderType:
        for (const auto& [uuid, shader] : m_ShaderMap) {
            names.push_back(shader->name);
        }
        break;

    default:
        break;
    }

    return names;
}

Ref<Material> AssetManager::loadMaterial(MaterialSettings matSettings, std::string name)
{
    Ref<Material> m = CreateRef<Material>(matSettings, name);
    m_MaterialMap[m->handle] = m;
    return m;
}

Ref<Material> AssetManager::loadMaterial(std::string name)
{
    // TODO REWRITE THE WHOLE DANG THING
    auto m = AssetManager::getMaterial(name);
    if (m) {
        return m;
    }
    return CreateRef<Material>(MaterialSettings {}, name);
}

Ref<Material> AssetManager::loadMaterial()
{
    Ref<Material> m = CreateRef<Material>();
    m_MaterialMap[m->handle] = m;
    return m;
}

Ref<Model> AssetManager::loadModel(std::string path)
{
    auto a = getModelByPath(path);
    if (a) {
        return a;
    }
    Ref<Model> m = CreateRef<Model>(path);
    m_ModelMap[m->handle] = m;
    return m;
}
Ref<Texture> AssetManager::loadTexture(std::string path, bool retain)
{
    auto a = getTextureByPath(path);
    if (a) {
        return a;
    }

    Ref<Texture> t = CreateRef<Texture>(path, retain);
    m_TextureMap[t->handle] = t;
    return t;
}
Ref<Shader> AssetManager::loadShader(std::string path, enum ShaderType type)
{
    auto a = getShaderByPath(path);
    if (a) {
        return a;
    }

    Ref<Shader> s = CreateRef<Shader>(path, type);
    m_ShaderMap[s->handle] = s;
    return s;
}
Ref<Shader> AssetManager::loadShader(std::string vertexPath, std::string fragPath)
{

    Ref<Shader> s = CreateRef<Shader>(vertexPath, fragPath);
    m_ShaderMap[s->handle] = s;
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

Ref<Material> AssetManager::getMaterialByPath(std::string path)
{
    for (const auto& [id, material] : m_MaterialMap) {
        if (material->path == path) {
            return material;
        }
    }
    return nullptr;
}
Ref<Model> AssetManager::getModelByPath(std::string path)
{
    for (const auto& [id, model] : m_ModelMap) {
        if (model->path == path) {
            return model;
        }
    }
    return nullptr;
}
Ref<Texture> AssetManager::getTextureByPath(std::string path)
{
    for (const auto& [id, texture] : m_TextureMap) {
        if (texture->path == path) {
            return texture;
        }
    }
    return nullptr;
}
Ref<Shader> AssetManager::getShaderByPath(std::string path)
{
    for (const auto& [id, shader] : m_ShaderMap) {
        if (shader->path == path) {
            return shader;
        }
    }
    return nullptr;
}

}
