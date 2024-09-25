#ifndef DM_MODEL_SERIALIZER
#define DM_MODEL_SERIALIZER
#include <Assets/AssetManager.hpp>
#include <Rendering/Model.hpp>
#include <core.hpp>

namespace Dimensional {
class DMCORE_API ModelSerializer {
public:
    static void Serialize(std::filesystem::path savePath, Ref<Model> model);

    static Ref<Model> Deserialize(std::filesystem::path file);
};
}

#endif
