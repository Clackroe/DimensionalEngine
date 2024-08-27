#ifndef DM_MESH_H
#define DM_MESH_H
#include "Core/Assets/Asset.hpp"
#include <Rendering/Renderer.hpp>
#include <Rendering/Texture.hpp>
#include <buffer.hpp>
#include <core.hpp>
namespace Dimensional {
struct TextureWrapper {
    std::string name;
    std::string type;
};
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class DMCORE_API Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, std::vector<u32>& indices, std::vector<TextureWrapper>& textures);
    ~Mesh();

    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    std::vector<TextureWrapper> textures;

private:
    Ref<VertexArray> vao;
    Ref<ElementBuffer> eb;

    void setup();
    friend class Renderer;
};
}
#endif
