#include "assimp/material.h"
#include <Rendering/Mesh.hpp>

#include <Rendering/Model.hpp>

#include <Rendering/Texture.hpp>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

namespace Dimensional {

// Static in the cpp file to avoid includeing assimp in the main app.
// aiTextureType cannot be forward declared in the header.
static std::vector<TextureWrapper> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::string directory)
{
    std::vector<TextureWrapper> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        TextureWrapper txw;
        mat->GetTexture(type, i, &str);
        DM_CORE_INFO("Texture Path: {0}", (directory + "/" + std::string(str.C_Str())));
        Ref<Texture> t = Renderer::createTexture(directory + std::string(str.C_Str()), false);
        txw.name = t->name;
        txw.type = typeName;

        textures.push_back(txw);
    }
    return textures;
}
void Model::Init(std::string path)
{
    loadModel(path);
}

Model::Model(std::string path)
    : Asset(path)
{
    loadModel(path);
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        DM_CORE_ERROR("ASSIMP: {0}", import.GetErrorString());
        return;
    }
    m_Directory = path.substr(0, path.find_last_of('/'));
    DM_CORE_INFO("DIR: {0}", m_Directory);

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(processMesh(mesh, scene));
    }
    // Continue to do the same for all children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    std::vector<TextureWrapper> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        auto& vert = mesh->mVertices[i];
        auto& norm = mesh->mNormals[i];

        Vertex vertex;
        glm::vec3 pos = glm::vec3(vert.x, vert.y, vert.z);
        vertex.Position = pos;

        glm::vec3 normal = glm::vec3(norm.x, norm.y, norm.z);
        vertex.Normal = normal;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    DM_CORE_INFO("VERTS: {0} | IND: {1} | TEX: {2} | MAT: {3}", vertices.size(), indices.size(), textures.size(), mesh->mMaterialIndex);

    if (mesh->mMaterialIndex >= 0) {

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        DM_CORE_INFO("MAT: {0}", scene->mNumMaterials);
        for (int i = 0; i < scene->mNumMaterials; i++) {
            DM_CORE_INFO("MATS: {0}", std::to_string(scene->mMaterials[i]->mProperties[0]->mType));
        }

        /*
         *
        uniform sampler2D uAlbedoMap;
        uniform sampler2D uNormalMap;
        uniform sampler2D uMetallicMap;
        uniform sampler2D uRoughnessMap;
        uniform sampler2D uAOMap;

            */

        std::vector<TextureWrapper> albedoMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_albedo", m_Directory);
        textures.insert(textures.end(), albedoMaps.begin(), albedoMaps.end());

        std::vector<TextureWrapper> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", m_Directory);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        std::vector<TextureWrapper> metallicMaps = loadMaterialTextures(material, aiTextureType_METALNESS, "texture_metallic", m_Directory);
        textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());

        std::vector<TextureWrapper> roughnessMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness", m_Directory);
        textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

        std::vector<TextureWrapper> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT_OCCLUSION, "texture_ao", m_Directory);
        textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
    }

    return Mesh(vertices, indices, textures);
}
}
