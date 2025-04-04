
#ifndef SUBMESH_MANAGER_HPP
#define SUBMESH_MANAGER_HPP

#include "Core/UUID.hpp"
#include "Rendering/SubMesh.hpp"
#include <Rendering/OpenGL/OpenGL_Submesh.hpp>
#include <variant>

namespace Dimensional {

struct GraphicsSubmesh {
    std::variant<std::monostate, OpenGLSubMesh> glSubmesh;
};

// Maybe have set data, could add for custom memory only meshes
struct SubMeshManager {
    static UUID CreateSubmesh(const SubMeshData& data);

    static OpenGLSubMesh GetOpenGLSubmesh(UUID id);
    static void DeleteSubmesh(UUID id);

private:
    static UMap<UUID, GraphicsSubmesh> s_SubmeshMap;
};

}

#endif // SUBMESH_MANAGER_HPP
