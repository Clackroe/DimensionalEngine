#ifndef DM_IMGUILAYERH
#define DM_IMGUILAYERH
#include <Core/Layer.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
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
