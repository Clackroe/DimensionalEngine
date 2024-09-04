#ifndef DM_ASSETMANAGER_H
#define DM_ASSETMANAGER_H
#include <Core/UUID.hpp>
#include <Rendering/Material.hpp>
namespace Dimensional {

class Model;
class Texture;
class Shader;

class AssetManager {
public:
    static Ref<Material> loadMaterial(MaterialSettings matSettings);
    static Ref<Model> loadModel(std::string path);
    static Ref<Texture> loadTexture(std::string path);
    static Ref<Shader> loadShader(std::string path);
    static Ref<Shader> loadShader(std::string vertexPath, std::string fragPath);

    static Ref<Material> getMaterial(UUID id) { return m_MaterialMap[id]; }
    static Ref<Model> getModel(UUID id) { return m_ModelMap[id]; }
    static Ref<Texture> getTexture(UUID id) { return m_TextureMap[id]; }
    static Ref<Shader> getShader(UUID id) { return m_ShaderMap[id]; }

    static Ref<Material> getMaterial(std::string name);
    static Ref<Model> getModel(std::string name);
    static Ref<Texture> getTexture(std::string name);
    static Ref<Shader> getShader(std::string name);

private:
    static UMap<UUID, Ref<Material>> m_MaterialMap;
    static UMap<UUID, Ref<Model>> m_ModelMap;
    static UMap<UUID, Ref<Texture>> m_TextureMap;
    static UMap<UUID, Ref<Shader>> m_ShaderMap;
};
}

#endif
