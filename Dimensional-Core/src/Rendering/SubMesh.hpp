#ifndef SUBMESH_HPP
#define SUBMESH_HPP

#include "Core/UUID.hpp"
#include <vector>
namespace Dimensional {

struct alignas(16) Vertex {
    glm::vec3 Position; // offset 0
    float pad0;

    glm::vec3 Normal; // offset 16
    float pad1;

    glm::vec3 Tangent; // offset 32
    float pad2;

    glm::vec3 BiTangent; // offset 48
    float pad3;

    glm::vec2 TexCoords; // offset 64
    glm::vec2 pad4; // pad to 16-byte alignment
};

struct SubMeshData {
    std::vector<Vertex> vertices;
    std::vector<u32> indices;
};
struct SubMesh {
    static Ref<SubMesh> Create(const SubMeshData& data);
    ~SubMesh();

private:
    SubMesh() = default;

    UUID m_UUID;
};

}

#endif // SUBMESH_HPP
