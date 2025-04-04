
#include "Rendering/SubMesh.hpp"
namespace Dimensional {

struct DrawElementsIndirectCommand {
    uint count;
    uint instanceCount;
    uint firstIndex;
    int baseVertex;
    uint baseInstance;
};

struct OpenGLSubMesh {
    static OpenGLSubMesh Create(const SubMeshData& data);

    void Destroy();

private:
    u32 count; // num indices

    u32 firstIndex; // first index in element buffer?

    int baseVertex; // First index of mesh

    u32 baseInstance;
};
}
