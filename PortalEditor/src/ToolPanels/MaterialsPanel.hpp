#ifndef DM_MATERIALSPANEL_HPP
#define DM_MATERIALSPANEL_HPP
#include <Assets/AssetManager.hpp>
#include <core.hpp>
namespace Dimensional {

class MaterialsPanel {
public:
    MaterialsPanel() = default;
    ~MaterialsPanel() = default;

    void renderImGui();
};
}
#endif
