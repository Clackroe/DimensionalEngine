#include <dimensional.hpp>

#include <Entry.hpp>
namespace Dimensional {

// Move to Editor once created
static EditorCamera cam;
//

static unsigned int indexCount;

static Model modelGun;

static glm::vec3 lightPositions[] = {
    glm::vec3(0.0f, 0.0f, 10.0f),
};
static glm::vec3 lightColors[] = {
    glm::vec3(150.0f, 100.0f, 150.0f),
};

class PortalLayer : public Layer {

    virtual void OnAttatch() override
    {
        DM_INFO("Portal Initialized");

        cam = EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    }
    virtual void OnDetatch() override { }
    virtual void OnUpdate() override
    {
        Renderer::beginScene();

        cam.Update();

        Ref<Shader> lightShader = Renderer::getShader("PBR1");

        lightShader->use();
        glm::vec3 p = cam.calcPos();

        lightShader->setMat4("viewProj", cam.getViewProj());

        lightShader->setInt("albedoMap", 0);
        lightShader->setInt("normalMap", 1);
        lightShader->setInt("metallicMap", 2);
        lightShader->setInt("roughnessMap", 3);
        lightShader->setInt("aoMap", 4);

        lightShader->setVec3("uCameraPosition", p.x, p.y, p.z);

        // int nrRows = 7;
        // int nrColumns = 7;
        // float spacing = 2.5;
        glm::mat4 model = glm::mat4(1.0f);
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
            lightShader->setVec3("uLightPositions[" + std::to_string(i) + "]", lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
            lightShader->setVec3("uLightColors[" + std::to_string(i) + "]", lightColors[i].x, lightColors[i].y, lightColors[i].z);
        }
        //
        // for (int row = 0; row < nrRows; ++row) {
        //     for (int col = 0; col < nrColumns; ++col) {
        //         model = glm::mat4(1.0f);
        //         model = glm::translate(model, glm::vec3((col - ((float)nrColumns / 2)) * spacing, (row - ((float)nrRows / 2)) * spacing, 0.0f));
        //         lightShader->setMat4("model", model);
        //         lightShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        //         Renderer::renderSphere(lightShader);
        //     }
        // }
        //

        glm::mat4 m = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        lightShader->setMat4("model", m);
        lightShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(m))));
        Renderer::renderModel(modelGun, lightShader);
        lightShader->setMat4("model", glm::translate(m, glm::vec3(4.0f)));
        Renderer::renderCube(lightShader);
        //
        // Renderer::renderCube(lightShader);
        //
        Renderer::endScene();

        if (Input::IsKeyDown(Key::Escape)) {
            Application::getApp().stopApplication();
        }
    }
    virtual void OnImGuiRender() override
    {

        // Note: Switch this to true to enable dockspace
        static bool useDocking = true;
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &useDocking, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        // Viewport
        ImGui::Begin("Viewport");
        u32 id = Renderer::getFrameBufferColorID();
        ImGui::Image(reinterpret_cast<ImTextureID>(id), ImVec2 { 1280, 720 }, { 0, 1 }, { 1, 0 });
        ImGui::End();
        //

        ImGui::End();
    }
};

class PortalApp : public Application {
public:
    PortalApp()
        : Application()
    {

        PushLayer(new PortalLayer());
        Renderer::createShader((engineAssetDirectory + "/Shaders/PBR1.glsl"));

        // Renderer::createTexture((engineAssetDirectory + "/Textures/Albedo.png"), false)->bind(0);
        // Renderer::createTexture((engineAssetDirectory + "/Textures/Normal.png"), false)->bind(1);
        // Renderer::createTexture((engineAssetDirectory + "/Textures/Metallic.png"), false)->bind(2);
        // Renderer::createTexture((engineAssetDirectory + "/Textures/Roughness.png"), false)->bind(3);

        Renderer::createTexture((engineAssetDirectory + "/Textures/AO.png"), false)->bind(4);

        Renderer::createTexture((engineAssetDirectory + "/Models/AK_103_Base_Color.png"), false)->bind(0);
        Renderer::createTexture((engineAssetDirectory + "/Models/AK_103_Metallic.png"), false)->bind(1);
        Renderer::createTexture((engineAssetDirectory + "/Models/AK_103_Normal.png"), false)->bind(2);
        Renderer::createTexture((engineAssetDirectory + "/Models/AK_103_Roughness.png"), false)->bind(3);

        modelGun.Init((engineAssetDirectory + "/Models/test.obj"));
        // modelGun.Init((engineAssetDirectory + "/Models/survival-guitar-backpack/source/Survival_BackPack_2/Survival_BackPack_2.fbx"));
        // modelGun.Init((engineAssetDirectory + "/Models/survival_guitar_backpack/scene.gltf"));
    }

    ~PortalApp()
    {
    }
};

Application* EXT_InitApplication()
{
    Application* app = new PortalApp();
    ImGui::SetCurrentContext(app->getImGuiContext());
    return app;
}
}
