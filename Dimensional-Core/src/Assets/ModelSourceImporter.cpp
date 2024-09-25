#include "Assets/Asset.hpp"
#include "Assets/AssetManager.hpp"
#include "Log/log.hpp"
#include "assimp/material.h"
#include <Assets/MaterialSerializer.hpp>
#include <Assets/ModelSourceImporter.hpp>
#include <Rendering/Material.hpp>

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

namespace Dimensional {
static AssetHandle loadEmbeddedTexture(const aiTexture* texture)
{
    // TODO: HandleEmbeddedTextures
    if (texture->mHeight == 0) {
        DM_CORE_INFO("Loading compressed embedded texture format: {0}", texture->mFilename.C_Str());
    } else {
        DM_CORE_INFO("Loading raw embedded texture format.");
    }
    return 0;
}

static AssetHandle loadTextureOfType(aiMaterial* mat, aiTextureType type, const aiScene* scene, const std::filesystem::path& modelPath)
{
    if (mat->GetTextureCount(type) > 0) {
        aiString path;
        if (mat->GetTexture(type, 0, &path) == AI_SUCCESS) {
            if (path.C_Str()[0] == '*') {
                // Handle embedded texture
                int textureIndex = std::atoi(path.C_Str() + 1);
                if (textureIndex >= 0 && textureIndex < scene->mNumTextures) {
                    aiTexture* embeddedTexture = scene->mTextures[textureIndex];
                    AssetHandle textureHandle = loadEmbeddedTexture(embeddedTexture);
                    DM_CORE_WARN("Loaded embedded texture: {0}, TYPE: {1}", path.C_Str(), aiTextureTypeToString(type));
                    return textureHandle;
                }
            } else {
                std::filesystem::path texturePath = modelPath.parent_path() / std::filesystem::path(path.C_Str());
                DM_CORE_WARN("PATH: {0}, TYPE: {1}", texturePath.string(), aiTextureTypeToString(type));

                if (!std::filesystem::exists(texturePath)) {
                    DM_CORE_ERROR("Texture file not found: {0}", texturePath.string());
                    return 0;
                }

                // AssetHandle textureHandle = loadTextureFromFile(texturePath.string());
                // return textureHandle;
                return 0;
            }
        }
    }
    return 0;
}

static void loadMaterialTextures(aiMaterial* mat, const aiScene* scene, MaterialSettings& material, const std::filesystem::path& modelPath)
{
    AssetHandle diffuse = 0;
    diffuse = loadTextureOfType(mat, aiTextureType_DIFFUSE, scene, modelPath);

    AssetHandle normal = 0;
    normal = loadTextureOfType(mat, aiTextureType_NORMALS, scene, modelPath);

    AssetHandle roughness = 0;
    roughness = loadTextureOfType(mat, aiTextureType_SPECULAR, scene, modelPath);
    if (!roughness) {
        roughness = loadTextureOfType(mat, aiTextureType_DIFFUSE_ROUGHNESS, scene, modelPath);
    }

    AssetHandle metalness = 0;
    metalness = loadTextureOfType(mat, aiTextureType_METALNESS, scene, modelPath);

    AssetHandle ao = 0;
    ao = loadTextureOfType(mat, aiTextureType_AMBIENT_OCCLUSION, scene, modelPath);

    material.Albedo = diffuse;
    material.Normal = normal;
    material.Metalness = metalness;
    material.Roughness = roughness;
    material.AO = ao;
}

static Mesh processMesh(aiMesh* mesh, const aiScene* scene, AssetHandle& handle, const std::filesystem::path& modelPath)
{
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

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

        if (mesh->HasTangentsAndBitangents()) {
            auto& tangent = mesh->mTangents[i];
            auto& biTangent = mesh->mBitangents[i];

            glm::vec3 tan = glm::vec3(tangent.x, tangent.y, tangent.z);
            glm::vec3 bitan = glm::vec3(biTangent.x, biTangent.y, biTangent.z);

            vertex.Tangent = tan;
            vertex.BiTangent = bitan;

        } else {
            vertex.Tangent = glm::vec3(0.0f);
            vertex.BiTangent = glm::vec3(0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // We can now process materials, if present.
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        MaterialSettings material;
        loadMaterialTextures(mat, scene, material, modelPath);

        // Register new MaterialAsset
        std::string name = modelPath.stem().string() + "_" + mat->GetName().C_Str() + ".dmat";
        std::filesystem::path outPath = modelPath.parent_path() / name;

        DM_CORE_WARN("Loading Material | Name: {0}, Path: {1}", name, outPath.string());

        if (!AssetManager::getInstance().isAssetRegistered(outPath.string())) {
            MaterialSerializer::Serialize(outPath, material);
            AssetHandle materialHandle;
            handle = AssetManager::getInstance().registerAsset(outPath);
        } else {
            handle = AssetManager::getInstance().getAssetHandleFromPath(outPath.string());
        }
    }

    return Mesh(vertices, indices);
}

static void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes, std::vector<AssetHandle>& handles, const std::filesystem::path& modelPath)
{
    // process all the node's meshes
    handles.resize(node->mNumMeshes);
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, handles[i], modelPath));
    }
    // Continue to do the same for all children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, meshes, handles, modelPath);
    }
}

Ref<ModelSource> ModelSourceImporter::importModelSource(AssetMetaData meta)
{
    return loadModelSourceFromPath(meta.sourcePath);
}

Ref<ModelSource> ModelSourceImporter::loadModelSourceFromPath(std::filesystem::path path)
{
    Assimp::Importer import;

    // TODO: aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals
    const aiScene* scene = import.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        DM_CORE_ERROR("ASSIMP: {0}", import.GetErrorString());
        return nullptr;
    }

    ModelSourceLoadSettings settings;
    processNode(scene->mRootNode, scene, settings.meshes, settings.meshMaterialHandles, path);

    Ref<ModelSource> out = CreateRef<ModelSource>(settings);
    return out;
}

}
