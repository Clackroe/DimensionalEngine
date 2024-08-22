#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Rendering/ElementBuffer.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexArray.hpp"
#include "Rendering/VertexLayout.hpp"
#include "imgui.h"
#include <Core/Application.hpp>

#include <buffer.hpp>

#include <Core/Time.hpp>

#include <Core/EditorCamera.hpp>

#include <glad.h>

namespace Dimensional {

Application* Application::s_Application = nullptr;

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

        // ImGui::Begin("Test");
        // ImGui::Text("HELLOOOWOWOWOWO");
        // ImGui::End();
        //
        // static bool t = true;
        // ImGui::ShowDemoWindow(&t);

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

void Application::m_Render()
{

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

    // unsigned int indices[] = {
    //     // note that we start from 0!
    //     0, 1, 3, // first Triangle
    //     1, 2, 3 // second Triangle
    // };

    // LIGHT

    VertexArray lightVao;
    VertexBuffer lvb(vertices, sizeof(vertices));
    VertexLayout lLayout;
    lLayout.Push<float>(3);
    lLayout.Push<float>(3);
    lightVao.AddBuffer(lvb, lLayout);

    Ref<Shader> lightShader = Renderer::createShader((engineAssetDirectory + "/Shaders/lightVert.glsl"), (engineAssetDirectory + "/Shaders/lightFrag.glsl"));
    lightShader->use();
    lightShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    lightShader->setVec3("lightPos", 1.0f, 1.0f, 1.0f);
    glm::vec3 p = cam.calcPos();
    lightShader->setVec3("viewPos", p.x, p.y, p.z);
    lightShader->setMat4("viewProj", cam.getViewProj());
    lightShader->setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 0.0f)), glm::vec3(10.0f, 0.5f, 10.0f)));
    Renderer::renderVAO(lightVao, 36, lightShader);

    //

    Ref<Texture> tex = Renderer::createTexture((engineAssetDirectory + "/Textures/Wood.jpg"), false);

    VertexArray vao;
    VertexBuffer vb(vertices, sizeof(vertices));

    // ElementBuffer eb(indices, sizeof(indices) / sizeof(u32));

    VertexLayout vLayout;
    vLayout.Push<float>(3);
    vLayout.Push<float>(3);

    vao.AddBuffer(vb, vLayout);

    Ref<Shader> normalShader = Renderer::createShader(engineAssetDirectory + "/Shaders/testVert.glsl", engineAssetDirectory + "/Shaders/testFrag.glsl");
    normalShader->use();
    normalShader->setMat4("viewProj", cam.getViewProj());
    normalShader->setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(1.0f)));

    Renderer::renderVAO(vao, 36, normalShader);
}

}
