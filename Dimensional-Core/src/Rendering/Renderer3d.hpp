#ifndef DM_RENDERER3D_HPP
#define DM_RENDERER3D_HPP

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <Rendering/Material.hpp>
#include <Rendering/Mesh.hpp>
#include <Rendering/Model.hpp>
#include <Rendering/Shader.hpp>

#include <buffer.hpp>

namespace Dimensional {
class DMCORE_API Renderer3D {
public:
    static void Init();

    static void renderMesh(Ref<Mesh> mesh, Ref<Material> material, glm::mat4 transform);
    static void renderMesh(Ref<Mesh> mesh, Ref<Shader> shader, glm::mat4 transform);
    static void renderModel(Ref<Model> model, glm::mat4 transform, std::vector<AssetHandle> matOverrirdes = std::vector<AssetHandle>());
    static void renderModel(Ref<Model> model, glm::mat4 transform, Ref<Shader> shader);

    static void renderCube(Ref<Shader> shader, glm::mat4 transform = glm::identity<glm::mat4>());

private:
};

}
#endif
