
#include "Rendering/RenderTarget.hpp"
#include "Rendering/RenderTargetManager.hpp"
namespace Dimensional {

Ref<RenderTarget> RenderTarget::Create(const RenderTargetData& data)
{

    Ref<RenderTarget> target = Ref<RenderTarget>(new RenderTarget());
    target->m_UUID = RenderTargetManager::CreateRenderTarget(data);
    return target;
}

void RenderTarget::Bind()
{
    RenderTargetManager::Bind(m_UUID);
}
void RenderTarget::UnBind()
{
    RenderTargetManager::UnBind(m_UUID);
}

void RenderTarget::SetSize(u32 width, u32 height)
{
    RenderTargetManager::SetSize(m_UUID, width, height);
}
void RenderTarget::ReBuild()
{
    RenderTargetManager::ReBuild(m_UUID);
}

void RenderTarget::Destroy()
{
    RenderTargetManager::Destroy(m_UUID);
}

}
