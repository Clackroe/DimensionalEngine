
#include "Rendering/VAO.hpp"
#include "Rendering/VAOManager.hpp"
namespace Dimensional {

Ref<VAO> VAO::Create()
{
    Ref<VAO> vao = Ref<VAO>(new VAO());
    vao->m_UUID = VAOManager::CreateVAO();
    return vao;
};

VAO::~VAO()
{
    VAOManager::DeleteVAO(m_UUID);
}

void VAO::SetData(const VAOData& data)
{
    m_ElementCount = data.indexBuffer.size();
    VAOManager::SetData(m_UUID, data);
}

void VAO::Bind()
{
    VAOManager::Bind(m_UUID);
}
void VAO::UnBind()
{
    VAOManager::UnBind(m_UUID);
}

}
