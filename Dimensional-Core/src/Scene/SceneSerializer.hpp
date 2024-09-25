#ifndef DM_SCENESERIALIZER_HPP
#define DM_SCENESERIALIZER_HPP
#include <core.hpp>

#include <Scene/Components.hpp>
#include <Scene/Scene.hpp>
namespace Dimensional {
class DMCORE_API SceneSerializer {
public:
    static void Serialize(std::filesystem::path savePath, const Ref<Scene> scene);
    static void Deserialize(std::filesystem::path file, Ref<Scene>& scene);

private:
};
}
#endif
