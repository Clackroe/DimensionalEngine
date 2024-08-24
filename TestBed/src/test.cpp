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

    FrameBufferSettings fbs = {
        1280,
        720,
    };
    FrameBuffer m_FB = FrameBuffer(fbs);

    virtual void OnAttatch() override
    {
        DM_INFO("TestLayer Attached!!");

        cam = EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
        m_FB.Bind();
    }
    virtual void OnDetatch() override { }
    virtual void OnUpdate() override
    {
        m_FB.Bind();

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

        // ElementBuffer eb(indices, sizeof(indices) / sizeof(u32));

        VertexLayout vLayout;
        vLayout.Push<float>(3);
        vLayout.Push<float>(3);

        vao.AddBuffer(vb, vLayout);

        normalShader->use();
        normalShader->setMat4("viewProj", cam.getViewProj());
        normalShader->setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f)));

        Renderer::renderVAO(vao, 36, normalShader);

        if (Input::IsKeyDown(Key::Escape)) {
            Application::getApp().stopApplication();
        }

        m_FB.Unbind();
    }
    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Test");
        ImGui::SliderFloat3("Ambient", &ambient[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Diffuse", &diffuse[0], 0.0f, 1.0f);
        ImGui::SliderFloat3("Specular", &specular[0], 0.0f, 1.0f);
        ImGui::SliderFloat("Shininess", &shininess, 0.0f, 500.0f);
        ImGui::End();

        ImGui::Begin("Epic Gamer");
        ImGui::Image(reinterpret_cast<ImTextureID>(m_FB.m_ColorGLId), ImVec2 { 1280, 720 });
        ImGui::End();
    }
};

class TestBed : public Application {
public:
    TestBed() : Application()
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
