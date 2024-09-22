#ifndef DM_RENDERER_HPP
#define DM_RENDERER_HPP

#include "Input/Input.hpp"
#include "Rendering/CubeMap.hpp"
#include "Rendering/IBLMap.hpp"
#include "Rendering/IrMap.hpp"

#include "Rendering/ElementBuffer.hpp"
#include <Core/Camera.hpp>
#include <Rendering/FrameBuffer.hpp>
#include <Rendering/Material.hpp>
#include <buffer.hpp>
#include <core.hpp>

namespace Dimensional {

// Forward Declare
class Shader;
class Texture;
class Mesh;
class Model;
//

struct CameraData {
    glm::mat4 viewProj;
    glm::vec3 uCameraPosition;

    glm::mat4 view;
    glm::mat4 proj;
};

// struct Light {
//     vec3 position;
//     vec3 direction;
//     vec3 color;
//
//     float intensity;
//     float cutOff; // Spotlight cutoff angle
//     float outerCutOff; // Spotlight outer cutoff angle
//
//     // Attenuation parameters
//     float constant;
//     float linear;
//     float quadratic;
// };
//

struct LightData {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;

    float cutOff;
    float outerCutOff;

    float intensity;
    float constant;
    float linear;
    float quadratic;
};
// struct LightData {
//     glm::vec3 pos;
//     glm::vec3 color;
// };

static FrameBufferSettings fbs = {
    1280,
    720,
    { RGBA16F, Depth }
};

class DMCORE_API Renderer {
public:
    Renderer() {};
    ~Renderer() {};

    void Init();

    // Rendering
    static void renderVAO(VertexArray vao, u32 triangleCount, Ref<Shader>& shader);
    static void renderVAO(const VertexArray& vao, const ElementBuffer& eb, const Ref<Shader>& shader);
    static void renderSphere(Ref<Shader>& shader);
    static void renderCube(Ref<Shader>& shader);

    static void renderCube(Ref<Material>& mat, glm::mat4 transform);
    static void renderSphere(Ref<Material>& mat, glm::mat4 transform);
    static void renderMesh(Mesh& mesh, Ref<Material> material, glm::mat4 transform);
    static void renderModel(Model& model, glm::mat4 transform);
    //
    static void submitLight(LightData data);

    //

    static void beginScene(CameraData data);
    static void endScene();

    // TODO: Create a better way to deal with framebuffers
    static Ref<FrameBuffer> getFrameBuffer() { return m_GetRenderer().m_FrameBuffer; };
    static Ref<IBLMap> getIBL() { return m_GetRenderer().m_IBLMap; }
    //

    // -----

private:
    static Renderer& m_GetRenderer() { return *s_RendererRef; }

    void setupCameraData();
    CameraData m_CameraData;

    void setupLightData();
    std::vector<LightData> m_LightData;

    void generatePrimitives();
    void generateSphere();
    void generateCube();

    // TODO: Add to Mesh map
    Ref<VertexArray> sphereVao;
    Ref<ElementBuffer> sphereEb;

    Ref<VertexArray> cubeVao;
    Ref<ElementBuffer> cubeEb;
    //

    Ref<Shader> m_PBRShader;

    Ref<FrameBuffer> m_FrameBuffer;

    Ref<IrMap> m_IrMap;

    Ref<IBLMap> m_IBLMap;

    Ref<CubeMap> m_CubeMap;
    Ref<Shader> m_CubeMapShader;

    // TEMP
    Ref<Material> m_TempMaterial;
    //

    static Renderer* s_RendererRef;
};
}

#endif
