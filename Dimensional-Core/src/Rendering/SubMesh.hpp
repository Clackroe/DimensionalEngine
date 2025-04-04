#ifndef SUBMESH_HPP
#define SUBMESH_HPP

#include "Core/UUID.hpp"
#include <vector>
namespace Dimensional {

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 BiTangent;
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
