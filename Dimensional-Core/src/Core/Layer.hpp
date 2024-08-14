#ifndef DM_LAYERH
#define DM_LAYERH
#include <core.hpp>

namespace Dimensional {
class Layer {
public:
    Layer(const std::string& name = "Layer - Default");
    virtual ~Layer() = default;

    virtual void OnAttatch() { }
    virtual void OnDetatch() { }
    virtual void OnUpdate() { }
    virtual void OnImGuiRender() { }

    inline const std::string& getName() const { return m_Name; };

protected:
    // NOTE: Potentially remove from release builds
    std::string m_Name;
};
}

#endif
