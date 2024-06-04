#ifndef VX_IMGUILAYERH
#define VX_IMGUILAYERH
#include <Core/Layer.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace Voxler {

class ImGuiLayer : Layer {
    ImGuiLayer();
    ~ImGuiLayer() = default;

    virtual void OnAttatch() override;
    virtual void OnDetatch() override;
    virtual void OnUpdate() override;

    void beginFrame();
    void endFrame();
};
}

#endif
