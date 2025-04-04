#include "Rendering/SubMeshManager.hpp"
#include <Core/Application.hpp>

namespace Dimensional {

UMap<UUID, GraphicsSubmesh> SubMeshManager::s_SubmeshMap;

UUID SubMeshManager::CreateSubmesh(const SubMeshData& data)
{
    UUID id;

    GraphicsSubmesh gSubMesh = {};

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        gSubMesh.glSubmesh = OpenGLSubMesh::Create(data);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }

    s_SubmeshMap[id] = gSubMesh;
    return id;
}

OpenGLSubMesh SubMeshManager::GetOpenGLSubmesh(UUID id)
{
    bool exists = s_SubmeshMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Get OpenGL SubMesh on non-existant SubMesh");
        return OpenGLSubMesh();
    };
    return std::get<OpenGLSubMesh>(s_SubmeshMap.at(id).glSubmesh);
}
void SubMeshManager::DeleteSubmesh(UUID id)
{
    bool exists = s_SubmeshMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Delete non-existant submesh");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLSubMesh mesh = std::get<OpenGLSubMesh>(s_SubmeshMap.at(id).glSubmesh);
        mesh.Destroy();
        s_SubmeshMap.erase(id);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

}
