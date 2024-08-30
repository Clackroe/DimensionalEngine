#ifndef DM_SCENEHIERARCHY_H
#define DM_SCENEHIERARCHY_H
#include <dimensional.hpp>
namespace Dimensional {
class SceneHierarchy {
public:
    SceneHierarchy() = default;
    SceneHierarchy(Ref<Scene> scene);

    void setSceneContext(Ref<Scene> scene);

    void renderImGui();

    Entity getSelectedEntity() const { return m_SelectedEntity; }

private:
    void entityTreeNode(Entity entity);
    void propertiesPanel();

    void entityComponenets(Entity entity);

    Ref<Scene> m_SceneContext;
    Entity m_SelectedEntity;
};
}
#endif
