
#include "Rendering/SubMesh.hpp"
namespace Dimensional {

struct SubMeshIndexData {
    u32 count; // num indices
    u32 firstIndex; // first index in element buffer
    int baseVertex; // First index of mesh
    u32 baseInstance;
};

struct OpenGLSubMesh {
    static OpenGLSubMesh Create(const SubMeshData& data);

    void Destroy();

private:
    SubMeshIndexData m_SubMeshData;
};
}
