#include "Rendering/OpenGL/OpenGL_Renderer.hpp"
#include "Rendering/SubMeshManager.hpp"
namespace Dimensional {

OpenGLSubMesh OpenGLSubMesh::Create(const SubMeshData& data)
{
    OpenGLSubMesh mesh;

    mesh.m_SubMeshData = OpenGLRenderer::AddSubMesh(data);

    return mesh;
}

void OpenGLSubMesh::Destroy()
{
}

}
