#ifndef VX_IMGUILAYERH
#define VX_IMGUILAYERH
#include <Core/Layer.hpp>
#include <core.hpp>
namespace Voxler {

class ImGuiLayer : public Layer {
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
