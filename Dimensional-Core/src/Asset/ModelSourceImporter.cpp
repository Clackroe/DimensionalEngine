// #include "Asset/Asset.hpp"
// #include "Asset/AssetManager.hpp"
// #include "Log/log.hpp"
// #include "assimp/material.h"
// #include <Asset/MaterialSerializer.hpp>
// #include <Asset/ModelSourceImporter.hpp>
// #include <Rendering/Material.hpp>
//
// #include <assimp/Importer.hpp> // C++ importer interface
// #include <assimp/postprocess.h> // Post processing flags
// #include <assimp/scene.h> // Output data structure
//
// namespace Dimensional {
//
// void PrintMaterials(const aiScene* scene)
// {
//     if (!scene) {
//         std::cerr << "No scene loaded." << std::endl;
//         return;
//     }
//
//     for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
//         aiMaterial* material = scene->mMaterials[i];
//
//         // List all the textures associated with the material
//         for (unsigned int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; ++j) {
//             aiString texturePath; // To hold the path of the texture
//             if (material->GetTexture(static_cast<aiTextureType>(j), 0, &texturePath) == AI_SUCCESS) {
//                 std::string textureType = aiTextureTypeToString(static_cast<aiTextureType>(j));
//                 std::cout << "Material " << i << ": "
//                           << "Texture Path: " << texturePath.C_Str()
//                           << ", Type: " << textureType << std::endl;
//             }
//         }
//     }
// }
//
// static AssetHandle loadEmbeddedTexture(const aiTexture* texture)
// {
//     // TODO: HandleEmbeddedTextures
//     if (texture->mHeight == 0) {
//         DM_CORE_INFO("Loading compressed embedded texture format: {0}", texture->mFilename.C_Str());
//     } else {
//         DM_CORE_INFO("Loading raw embedded texture format.");
//     }
//     return 0;
// }
//
// static AssetHandle loadTextureFromFile(const std::string& filePath)
// {
//     // Implement your texture loading logic here.
//     // Example:
//     // AssetHandle handle = AssetManager::getInstance().loadTexture(filePath);
//     return 0;
// }
//
// static AssetHandle loadTextureOfType(aiMaterial* mat, aiTextureType type, const aiScene* scene, const std::filesystem::path& modelPath)
// {
//     if (mat->GetTextureCount(type) > 0) {
//         aiString path;
//         if (mat->GetTexture(type, 0, &path) == AI_SUCCESS) {
//             if (path.C_Str()[0] == '*') {
//                 // Handle embedded texture
//                 int textureIndex = std::atoi(path.C_Str() + 1);
//                 if (textureIndex >= 0 && textureIndex < scene->mNumTextures) {
//                     aiTexture* embeddedTexture = scene->mTextures[textureIndex];
//                     AssetHandle textureHandle = loadEmbeddedTexture(embeddedTexture);
//                     DM_CORE_WARN("Loaded embedded texture: {0}, TYPE: {1}", path.C_Str(), aiTextureTypeToString(type));
//                     return textureHandle;
//                 }
//             } else {
//                 std::filesystem::path texturePath = (modelPath.parent_path() / std::filesystem::path(path.C_Str()));
//                 std::string texturePathString = texturePath.string();
//                 std::replace(texturePathString.begin(), texturePathString.end(), '\\', '/');
//
//                 // Use the updated texturePathString for any further operations
//                 DM_CORE_WARN("PATH: {0}, TYPE: {1}", texturePathString, aiTextureTypeToString(type));
//
//                 if (!std::filesystem::exists(texturePathString)) {
//                     DM_CORE_WARN("Texture file not found: {0}", texturePathString);
//                     return 0; // Return an invalid handle if the file doesn't exist
//                 }
//
//                 AssetHandle textureHandle = AssetManager::getInstance().getAssetHandleFromPath(texturePathString);
//                 if (!textureHandle) {
//                     textureHandle = AssetManager::getInstance().registerAsset(texturePathString);
//                 }
//                 if (textureHandle) {
//                     DM_CORE_INFO("Successfully loaded texture: {0}", texturePathString);
//                 } else {
//                     DM_CORE_ERROR("SourceImporter:: Failed to load texture: {0}", texturePathString);
//                     return 0;
//                 }
//                 return textureHandle;
//             }
//         }
//     }
//     return 0; // No texture found
// }
//
// static void loadMaterialTextures(aiMaterial* mat, const aiScene* scene, MaterialSettings& material, const std::filesystem::path& modelPath)
// {
//     AssetHandle diffuse = 0;
//     diffuse = loadTextureOfType(mat, aiTextureType_DIFFUSE, scene, modelPath);
//
//     AssetHandle normal = 0;
//     normal = loadTextureOfType(mat, aiTextureType_NORMALS, scene, modelPath);
//
//     AssetHandle roughness = 0;
//     roughness = loadTextureOfType(mat, aiTextureType_SPECULAR, scene, modelPath);
//
//     AssetHandle metalness = 0;
//     metalness = loadTextureOfType(mat, aiTextureType_METALNESS, scene, modelPath);
//
//     AssetHandle ao = 0;
//     ao = loadTextureOfType(mat, aiTextureType_AMBIENT_OCCLUSION, scene, modelPath);
//
//     material.Albedo = diffuse;
//     material.Normal = normal;
//     material.Metalness = metalness;
//     material.Roughness = roughness;
//     material.AO = ao;
// }
//
// static void loadMaterialProperties(aiMaterial* mat, MaterialSettings& material)
// {
//     aiColor4D color;
//     float value;
//
//     if (AI_SUCCESS == aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) {
//         material.color = glm::vec3(color.r, color.g, color.b);
//     }
//
//     if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_ROUGHNESS_FACTOR, &value)) {
//         material.roughnessMult = 1.0f - (value / 100.0f);
//     }
//
//     if (AI_SUCCESS == aiGetMaterialFloat(mat, AI_MATKEY_METALLIC_FACTOR, &value)) {
//         material.metalnessMult = value;
//     }
// }
//
// static Ref<Mesh> processMesh(aiMesh* mesh, const aiScene* scene, AssetHandle& handle, const std::filesystem::path& modelPath)
// {
//     std::vector<Vertex> vertices;
//     std::vector<u32> indices;
//
//     for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
//         auto& vert = mesh->mVertices[i];
//         auto& norm = mesh->mNormals[i];
//
//         Vertex vertex;
//         glm::vec3 pos = glm::vec3(vert.x, vert.y, vert.z);
//         vertex.Position = pos;
//
//         glm::vec3 normal = glm::vec3(norm.x, norm.y, norm.z);
//         vertex.Normal = normal;
//
//         if (mesh->mTextureCoords[0]) {
//             glm::vec2 vec;
//             vec.x = mesh->mTextureCoords[0][i].x;
//             vec.y = mesh->mTextureCoords[0][i].y;
//             vertex.TexCoords = vec;
//         } else {
//             vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//         }
//
//         if (mesh->HasTangentsAndBitangents()) {
//             auto& tangent = mesh->mTangents[i];
//             auto& biTangent = mesh->mBitangents[i];
//
//             glm::vec3 tan = glm::vec3(tangent.x, tangent.y, tangent.z);
//             glm::vec3 bitan = glm::vec3(biTangent.x, biTangent.y, biTangent.z);
//
//             vertex.Tangent = tan;
//             vertex.BiTangent = bitan;
//
//         } else {
//             vertex.Tangent = glm::vec3(0.0f);
//             vertex.BiTangent = glm::vec3(0.0f);
//         }
//
//         vertices.push_back(vertex);
//     }
//
//     for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//         aiFace face = mesh->mFaces[i];
//         for (unsigned int j = 0; j < face.mNumIndices; j++)
//             indices.push_back(face.mIndices[j]);
//     }
//
//     // We can now process materials, if present.
//     if (mesh->mMaterialIndex >= 0) {
//         aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
//         MaterialSettings material;
//         loadMaterialTextures(mat, scene, material, modelPath);
//         loadMaterialProperties(mat, material);
//
//         // Register new MaterialAsset if it doesnt exist
//         std::string name = modelPath.stem().string() + "_" + mat->GetName().C_Str() + std::to_string(mesh->mMaterialIndex) + ".dmat";
//         std::filesystem::path outPath = modelPath.parent_path() / name;
//
//         DM_CORE_WARN("Loading Material | Name: {0}, Path: {1}", name, outPath.string());
//
//         if (!AssetManager::getInstance().isAssetRegistered(outPath.string())) {
//             MaterialSerializer::Serialize(outPath, material);
//             handle = AssetManager::getInstance().registerAsset(outPath);
//         } else {
//             MaterialSerializer::Serialize(outPath, material);
//             handle = AssetManager::getInstance().getAssetHandleFromPath(outPath.string());
//         }
//     }
//
//     return CreateRef<Mesh>(vertices, indices);
// }
//
// static void processNode(aiNode* node, const aiScene* scene, std::vector<Ref<Mesh>> meshes, std::vector<AssetHandle>& handles, const std::filesystem::path& modelPath)
// {
//     // process all the node's meshes
//     for (unsigned int i = 0; i < node->mNumMeshes; i++) {
//         aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//         AssetHandle id = 0;
//         meshes.push_back(processMesh(mesh, scene, id, modelPath));
//         handles.push_back(id);
//     }
//     // Continue to do the same for all children
//     for (unsigned int i = 0; i < node->mNumChildren; i++) {
//         processNode(node->mChildren[i], scene, meshes, handles, modelPath);
//     }
// }
//
// Ref<ModelSource> ModelSourceImporter::importModelSource(AssetMetaData meta)
// {
//     return loadModelSourceFromPath(meta.sourcePath);
// }
//
// Ref<ModelSource> ModelSourceImporter::loadModelSourceFromPath(std::filesystem::path path)
// {
//     Assimp::Importer import;
//
//     // TODO: aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals
//     const aiScene* scene = import.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);
//
//     if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
//         DM_CORE_ERROR("ASSIMP: {0}", import.GetErrorString());
//         return nullptr;
//     }
//
//     PrintMaterials(scene);
//
//     ModelSourceLoadSettings settings;
//     processNode(scene->mRootNode, scene, settings.meshes, settings.meshMaterialHandles, path);
//
//     Ref<ModelSource> out = CreateRef<ModelSource>(settings);
//     return out;
// }
// }
