#include <PortalEditor.hpp>
#include <dimensional.hpp>

#include <Entry.hpp>
namespace Dimensional {

class PortalApp : public Application {
public:
    PortalApp()
        : Application("Portal Editor", 1920, 1080)
    {
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Textures/Bell_BaseColor.png"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Textures/Bell_Normal.png"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Textures/Bell_Metallic.png"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Textures/Bell_Roughness.png"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Textures/AO.png"), false);

        AssetManager::loadTexture((engineAssetDirectory + "/Models/Sword/swordAlbedo.jpg"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Sword/swordNormal.jpg"), false);
        AssetManager::loadTexture((engineAssetDirectory + "/Models/Sword/swordRough.jpg"), false);
        // AssetManager::loadTexture((engineAssetDirectory + "/Models/Sword/rough.png"), false);

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
