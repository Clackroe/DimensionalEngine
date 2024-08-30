#ifndef DM_PORTALLAYER_H
#define DM_PORTALLAYER_H
#include "ToolPanels/SceneHierarchy.hpp"
#include <dimensional.hpp>
namespace Dimensional {
class PortalLayer : public Layer {
public:
    PortalLayer() = default;
    virtual void OnAttatch() override;
    virtual void OnDetatch() override;
    virtual void OnUpdate() override;
    virtual void OnImGuiRender() override;

private:
    SceneHierarchy m_HierarchyPanel;

private:
    EditorCamera m_EditorCamera;

    Ref<Scene> m_ActiveScene;

    // Temp
    Ref<Material> materialTest = CreateRef<Material>();
    MaterialSettings ms;
    Ref<Material> materialReal;

    //
};
}
#endif
