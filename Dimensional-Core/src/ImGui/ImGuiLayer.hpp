#ifndef DM_IMGUILAYERH
#define DM_IMGUILAYERH
#include <Core/Layer.hpp>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
namespace Dimensional {

class DMCORE_API ImGuiLayer : public Layer {
public:
    ImGuiLayer();
    ~ImGuiLayer() = default;

    virtual void OnAttatch() override;
    virtual void OnDetatch() override;

    void beginFrame();
    void endFrame();
};
}

#endif
