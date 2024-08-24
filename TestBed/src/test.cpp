#include <dimensional.hpp>

#include <Entry.hpp>
namespace Dimensional {

// Move to Editor once created
static EditorCamera cam;
//
static float ambient[3] = { 0.0f, 0.0f, 0.0f };
static float diffuse[3] = { 0.0f, 0.0f, 0.0f };
static float specular[3] = { 0.0f, 0.0f, 0.0f };
static float shininess = 0.0f;

class TestLayer : public Layer {

    virtual void OnAttatch() override
    {
        DM_INFO("TestLayer Attached!!");

        cam = EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    }
    virtual void OnDetatch() override { }
    virtual void OnUpdate() override
    {
        Renderer::beginScene();

        cam.Update();

        float vertices[] = {
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
        };

        // LIGHT

        VertexArray lightVao;
        VertexBuffer lvb(vertices, sizeof(vertices));
        VertexLayout lLayout;
        lLayout.Push<float>(3);
        lLayout.Push<float>(3);
        lightVao.AddBuffer(lvb, lLayout);

        Ref<Shader> lightShader = Renderer::getShader("lightFrag");
        Ref<Shader> normalShader = Renderer::getShader("testFrag");

        lightShader->use();
        lightShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        lightShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightShader->setVec3("lightPos", 1.0f, 1.0f, 1.0f);
        glm::vec3 p = cam.calcPos();
        lightShader->setVec3("viewPos", p.x, p.y, p.z);
        lightShader->setMat4("viewProj", cam.getViewProj());

        lightShader->setVec3("material.ambient", ambient[0], ambient[1], ambient[2]);
        lightShader->setVec3("material.diffuse", diffuse[0], diffuse[1], diffuse[2]);
        lightShader->setVec3("material.specular", specular[0], specular[1], specular[2]);
        lightShader->setFloat("material.shininess", shininess);

        lightShader->setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 0.0f)), glm::vec3(10.0f, 0.5f, 10.0f)));

        // Ref<Texture> tex = Renderer::createTexture((engineAssetDirectory + "/Textures/Wood.jpg"), false);

        Renderer::renderVAO(lightVao, 36, lightShader);

        //

        VertexArray vao;
        VertexBuffer vb(vertices, sizeof(vertices));

        VertexLayout vLayout;
        vLayout.Push<float>(3);
        vLayout.Push<float>(3);

        vao.AddBuffer(vb, vLayout);

        normalShader->use();
        normalShader->setMat4("viewProj", cam.getViewProj());
        normalShader->setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f)));

        Renderer::renderVAO(vao, 36, normalShader);
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

        //----

        ImGui::Begin("Properties");
        ImGui::SliderFloat3("Ambient", &ambient[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Diffuse", &diffuse[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Specular", &specular[0], 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess", &shininess, 0.0f, 500.0f);
        ImGui::End();

        ImGui::Begin("Viewport");
        u32 id = Renderer::getFrameBufferColorID();
        ImGui::Image(reinterpret_cast<ImTextureID>(id), ImVec2 { 1280, 720 }, { 0, 1 }, { 1, 0 });
        ImGui::End();
    }
};

class TestBed : public Application {
public:
    TestBed()
        : Application()
    {

        PushLayer(new TestLayer());
        Renderer::createShader((engineAssetDirectory + "/Shaders/lightVert.glsl"), (engineAssetDirectory + "/Shaders/lightFrag.glsl"));
        Renderer::createShader(engineAssetDirectory + "/Shaders/testVert.glsl", engineAssetDirectory + "/Shaders/testFrag.glsl");
    }

    ~TestBed()
    {
    }
};

Application* EXT_InitApplication()
{
    Application* app = new TestBed();
    ImGui::SetCurrentContext(app->getImGuiContext());
    return app;
}
}
