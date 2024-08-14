#include "ImGui/ImGuiLayer.hpp"

#include <Core/Application.hpp>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <GLFW/glfw3.h>
namespace Dimensional {

ImGuiLayer::ImGuiLayer()
    : Layer("ImGuiOverlay")
{
}

void ImGuiLayer::OnAttatch()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    ImGui_ImplGlfw_InitForOpenGL(Application::getApp().getWindowVX().getGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 410");
}
void ImGuiLayer::OnDetatch()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void ImGuiLayer::endFrame()
{
    // Resize the window if
    ImGuiIO& io = ImGui::GetIO();
    Application& application = Application::getApp();
    io.DisplaySize = ImVec2((float)application.getWindowVX().getWidth(), (float)application.getWindowVX().getWidth());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}
