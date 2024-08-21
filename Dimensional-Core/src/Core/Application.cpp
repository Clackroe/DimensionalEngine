#include "Core/Shader.hpp"
#include "Core/Texture.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Rendering/Renderer.hpp"
#include <Core/Application.hpp>

#include <Core/Time.hpp>

#include <Core/EditorCamera.hpp>

#include <glad.h>

namespace Dimensional {

Application* Application::s_Application = nullptr;

static void temp_init();

// Move to Editor once created
static EditorCamera cam;
//

Application::Application(const std::string& title, u32 width, u32 height)
{
    // Move to Editor once created
    cam = EditorCamera(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    //

    DM_CORE_ASSERT(!s_Application, "Application already created!! Aborting.");
    s_Application = this;

    initializeSubSystems();

    m_Window = CreateScope<Window>((WindowSettings) { width, height, title });

    m_ImGuiOverlay = new ImGuiLayer();
    m_LayerStack.pushOverlay(m_ImGuiOverlay);

    DM_CORE_INFO("Platform: {0}", DM_PLATFORM);

    temp_init();
}

void Application::runApplication()
{
    while (m_Running) {
        Time::Update();

        EventSystem::ProcessEvents();

        cam.Update();

        //------Update Layers-------
        for (Layer* layer : m_LayerStack) {
            layer->OnUpdate();
        }

        //------Update imgui Layers-------
        m_ImGuiOverlay->beginFrame();

        for (Layer* layer : m_LayerStack) {
            layer->OnImGuiRender();
        }

        m_ImGuiOverlay->endFrame();

        //------
        m_Window->update();
        m_Render();
    }
}

void Application::initializeSubSystems()
{
    Log::Init();
    m_EventSystem.Init();
    m_Input.Init();
    m_Renderer.Init();
}

static void temp_init()
{
}

void Application::m_Render()
{

    Hash shaderHash = Renderer::createShader(engineAssetDirectory + "/Shaders/testVert.glsl", engineAssetDirectory + "/Shaders/testFrag.glsl");
    Renderer::setShader(shaderHash);

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
    };

    float texCoords[] = {
        0.0f, 0.0f, // lower-left corner
        1.0f, 0.0f, // lower-right corner
        0.5f, 1.0f // top-center corner
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3 // second Triangle
    };

    Hash texHash = Renderer::createTexture((engineAssetDirectory + "/Textures/Wood.jpg"), false);
    Ref<Texture> tex = Renderer::getTexture(texHash);

    unsigned int VBO,
        VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    Renderer::getCurrentShader()->setMat4("viewProj", cam.getViewProj());

    Renderer::getCurrentShader()->use();
    tex->bind(0);

    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

}
