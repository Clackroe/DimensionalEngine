#ifndef DM_RENDERER3D_HPP
#define DM_RENDERER3D_HPP

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <Rendering/Material.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/Model.hpp>
#include <Rendering/Shader.hpp>

#include <buffer.hpp>
#include <functional>
#include <vector>

namespace Dimensional {

using ShaderID = u32;
using MaterialID = u32;
using MeshID = u32;

struct RenderInstanceData {
    glm::mat4 transform;
};

struct RenderBatch {
    std::vector<RenderInstanceData> instances;
    Ref<Mesh> mesh;
    Ref<Material> material;
};

struct FrameSector {

    std::function<void()> setupFunc;
    std::function<void()> cleanupFunc;

    UMap<ShaderID, UMap<MaterialID, UMap<MeshID, RenderBatch>>> materialBatches;
    UMap<ShaderID, UMap<MeshID, RenderBatch>> shaderOnlyBatches;
};

struct Renderer3DData {
    // Eventually will be an array of Frames of an array of frame sectors (Basically renderpasses)
    std::vector<FrameSector> sectors;
    u32 currentSector = 0;
};

class Renderer3D {
public:
    static void Init();

    static void submitMesh(Mesh mesh, Ref<Material> material, glm::mat4 transform);
    static void submitMesh(Mesh mesh, Ref<Shader> shader, glm::mat4 transform);
    static void submitModel(Ref<Model> model, glm::mat4 transform, std::vector<AssetHandle> matOverrirdes = std::vector<AssetHandle>());
    static void submitModel(Ref<Model> model, glm::mat4 transform, Ref<Shader> shader);

    static void submitCube(Ref<Shader> shader, glm::mat4 transform = glm::identity<glm::mat4>());

    static void beginSector(std::function<void()> setupFunc);
    static void endSector(std::function<void()> cleanupFunc);

    static void submitFrame();

private:
    static Renderer3DData m_Data;
};

}
#endif
