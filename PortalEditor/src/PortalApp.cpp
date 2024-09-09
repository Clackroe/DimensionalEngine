#include <PortalEditor.hpp>
#include <dimensional.hpp>

#include <Entry.hpp>
namespace Dimensional {

class PortalApp : public Application {
public:
    PortalApp()
        : Application()
    {
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Textures/Bell_BaseColor.png"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Textures/Bell_Normal.png"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Textures/Bell_Metallic.png"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Textures/Bell_Roughness.png"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Textures/AO.png"), false);

        PushLayer(new PortalLayer());
    }

    ~PortalApp()
    {
    }
};

Application* EXT_InitApplication()
{
    Application* app = new PortalApp();
    ImGui::SetCurrentContext(app->getImGuiContext());
    return app;
}
}
