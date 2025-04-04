#ifndef MESH_HPP
#define MESH_HPP

#include "Rendering/SubMesh.hpp"
#include <vector>
namespace Dimensional {

struct Mesh {

    // std::vector<UUID> materials;

    std::vector<SubMesh> subMeshes;
};
}

#endif // MESH_HPP
