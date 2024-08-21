#include "Core/Shader.hpp"
#include "Core/Texture.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "Log/log.hpp"
#include "Rendering/ElementBuffer.hpp"
#include "Rendering/Renderer.hpp"
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

    VertexArray vao;
    VertexBuffer vb(vertices, sizeof(vertices));

    ElementBuffer eb(indices, sizeof(indices) / sizeof(u32));

    VertexLayout vLayout;
    vLayout.Push<float>(3);
    vLayout.Push<float>(3);
    vLayout.Push<float>(2);

    vao.AddBuffer(vb, vLayout);

    Renderer::getCurrentShader()->setMat4("viewProj", cam.getViewProj());

    Renderer::getCurrentShader()->use();
    tex->bind(0);

    vao.Bind();
    eb.Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

}
