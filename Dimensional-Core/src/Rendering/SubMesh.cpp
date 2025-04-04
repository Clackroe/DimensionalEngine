#include <Rendering/SubMesh.hpp>
#include <Rendering/SubMeshManager.hpp>
namespace Dimensional {

Ref<SubMesh> SubMesh::Create(const SubMeshData& data)
{
    Ref<SubMesh> smesh = Ref<SubMesh>(new SubMesh());
    smesh->m_UUID = SubMeshManager::CreateSubmesh(data);
    return smesh;
}

SubMesh::~SubMesh()
{
    SubMeshManager::DeleteSubmesh(m_UUID);
}
}
