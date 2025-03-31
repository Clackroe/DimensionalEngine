// #ifndef DM_PORTALLAYER_H
// #define DM_PORTALLAYER_H
// #include "ToolPanels/SceneHierarchy.hpp"
// #include <dimensional.hpp>
// namespace Dimensional {
//
// enum class EditorState {
//     EDIT,
//     PLAY
// };
//
// class PortalLayer : public Layer {
// public:
//     PortalLayer() = default;
//     virtual void OnAttatch() override;
//     virtual void OnDetatch() override;
//     virtual void OnUpdate() override;
//     virtual void OnImGuiRender() override;
//
//     void startRuntime();
//     void endRuntime();
//
//     void openScene(AssetHandle sceneHandle);
//     void saveCurrentScene();
//
// private:
//     void setActiveScene(Ref<Scene> sc);
//
//     SceneHierarchy m_HierarchyPanel;
//
//     EditorState m_State = EditorState::EDIT;
//
// private:
//     EditorCamera m_EditorCamera;
//
//     glm::vec2 m_ViewPortSize;
//
//     // Ref<SceneRenderer> m_SceneRenderer;
//
//     AssetHandle m_ActiveSceneHandle = 0;
//     Ref<Scene> m_ActiveScene = nullptr;
//     Ref<Scene> m_EditorScene = nullptr;
//     Ref<Scene> m_RuntimeScene = nullptr;
// };
// }
// #endif
