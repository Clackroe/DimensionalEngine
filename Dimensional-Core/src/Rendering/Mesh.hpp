#ifndef DM_MESH_H
#define DM_MESH_H
#include <buffer.hpp>
#include <core.hpp>
namespace Dimensional {

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 BiTangent;
};

class DMCORE_API Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, std::vector<u32>& indices);
    ~Mesh();

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

private:
    Ref<VertexArray> vao;
    Ref<ElementBuffer> eb;

    void setup();
    friend class Renderer;
};
}
#endif
