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

static bool made = false;
static unsigned int indexCount;
static void renderSphere(Ref<Shader> shad)
{
    static VertexArray* vao;
    static ElementBuffer* eb;
    // if (!made) {
    vao = new VertexArray();
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSegment, ySegment));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        if (!oddRow) // even rows: y == 0, y == 2; and so on
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        } else {
            for (int x = X_SEGMENTS; x >= 0; --x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i) {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0) {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
        if (uv.size() > 0) {
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
        }
    }
    VertexBuffer vb(data.data(), data.size() * sizeof(float));
    eb = new ElementBuffer(indices.data(), indexCount);
    VertexLayout lb;
    lb.Push<float>(3);
    lb.Push<float>(3);
    lb.Push<float>(2);
    vao->AddBuffer(vb, lb);
    // }
    Renderer::renderVAO(*vao, *eb, shad);
}

static glm::vec3 lightPositions[] = {
    glm::vec3(-10.0f, 10.0f, 10.0f),
    glm::vec3(10.0f, 10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3(10.0f, -10.0f, 10.0f),
};
static glm::vec3 lightColors[] = {
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f)
};

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

        // Ref<Shader> lightShader = Renderer::getShader("Light");
        Ref<Shader> lightShader = Renderer::getShader("PBR");
        // Ref<Shader> normalShader = Renderer::getShader("Test");

        // uniform mat4 viewProj;
        // uniform mat4 model;
        // uniform mat3 normalMatrix;
        //
        // uniform vec3 uCameraPosition;
        //
        // uniform vec3 uAlbedo;
        // uniform float uMetallic;
        // uniform float uRoughness;
        // uniform float uAO;
        //
        // // Lights
        // uniform vec3 uLightPositions[4];
        // uniform vec3 uLightColors[4];

        lightShader->use();
        glm::vec3 p = cam.calcPos();

        lightShader->setMat4("viewProj", cam.getViewProj());
        lightShader->setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 0.0f)), glm::vec3(10.0f, 0.5f, 10.0f)));

        lightShader->setVec3("uCameraPosition", p.x, p.y, p.z);
        lightShader->setVec3("uAlbedo", 0.5f, 0.0f, 0.0f);
        lightShader->setFloat("uAO", 1.0f);

        int nrRows = 7;
        int nrColumns = 7;
        float spacing = 2.5;
        glm::mat4 model = glm::mat4(1.0f);
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
            lightShader->setVec3("uLightPositions[" + std::to_string(i) + "]", lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
            lightShader->setVec3("uLightColors[" + std::to_string(i) + "]", lightColors[i].x, lightColors[i].y, lightColors[i].z);
        }

        for (int row = 0; row < nrRows; ++row) {
            lightShader->setFloat("uMetallic", (float)row / (float)nrRows);
            for (int col = 0; col < nrColumns; ++col) {
                // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
                // on direct lighting.
                lightShader->setFloat("uRoughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

                model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3((col - (nrColumns / 2)) * spacing, (row - (nrRows / 2)) * spacing, 0.0f));
                lightShader->setMat4("model", model);
                lightShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
                renderSphere(lightShader);
            }
        }

        lightShader->setVec3("uAlbedo", 1.0f, 1.0f, 1.0f);
        for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
            glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(Time::getTime() * 5.0) * 5.0, 0.0, 0.0);
            lightPositions[i] = newPos;
            lightShader->setVec3("uLightPositions[" + std::to_string(i) + "]", newPos.x, newPos.y, newPos.z);
            lightShader->setVec3("uLightColors[" + std::to_string(i) + "]", lightColors[i].x, lightColors[i].y, lightColors[i].z);

            model = glm::mat4(1.0f);
            model = glm::translate(model, newPos);
            model = glm::scale(model, glm::vec3(0.5f));
            lightShader->setMat4("model", model);
            lightShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
            renderSphere(lightShader);
        }

        // Renderer::renderVAO(lightVao, 36, lightShader);

        //
        //
        // VertexArray vao;
        // VertexBuffer vb(vertices, sizeof(vertices));
        //
        // VertexLayout vLayout;
        // vLayout.Push<float>(3);
        // vLayout.Push<float>(3);
        //
        // vao.AddBuffer(vb, vLayout);
        //
        // normalShader->use();
        // normalShader->setMat4("viewProj", cam.getViewProj());
        // normalShader->setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f)));
        //
        // Renderer::renderVAO(vao, 36, normalShader);
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
        Renderer::createShader((engineAssetDirectory + "/Shaders/Light.glsl"));
        Renderer::createShader((engineAssetDirectory + "/Shaders/PBR.glsl"));
        Renderer::createShader((engineAssetDirectory + "/Shaders/Test.glsl"));
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
