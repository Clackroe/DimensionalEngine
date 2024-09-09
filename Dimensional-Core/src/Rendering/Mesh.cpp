#include <Rendering/Mesh.hpp>

namespace Dimensional {
Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<u32>& indices, std::vector<TextureWrapper>& textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    setup();
}

Mesh::~Mesh() { }

void Mesh::setup()
{
    vao = CreateRef<VertexArray>();
    eb = CreateRef<ElementBuffer>(indices.data(), indices.size());
    VertexBuffer vb(vertices.data(), vertices.size() * sizeof(Vertex));
    VertexLayout vl;
    vl.Push<float>(3);
    vl.Push<float>(3);
    vl.Push<float>(2);
    vl.Push<float>(3);
    vl.Push<float>(3);

    vao->AddBuffer(vb, vl);
}
};
