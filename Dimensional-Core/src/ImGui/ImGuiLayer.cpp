#include "ImGui/ImGuiLayer.hpp"

#include <Core/Application.hpp>

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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    ImGui_ImplGlfw_InitForOpenGL(Application::getApp().getWindowDM().getGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 450");
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
    io.DisplaySize = ImVec2((float)application.getWindowDM().getWidth(), (float)application.getWindowDM().getWidth());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

}
