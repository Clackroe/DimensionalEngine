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

    void entityComponents(Entity entity);

    template <typename CType, typename CSpecificFunction>
    void componentNode(const std::string& name, Entity entity, CSpecificFunction function, bool canBeRemoved = true);

    Ref<Scene> m_SceneContext;
    Entity m_SelectedEntity;
};
}
#endif
