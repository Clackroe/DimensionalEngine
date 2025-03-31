
#include "Rendering/Texture2D.hpp"
#include "Rendering/Texture2DManager.hpp"
namespace Dimensional {

Ref<Texture2D> Texture2D::Create(const Texture2DData& data)
{
    Ref<Texture2D> tex = Ref<Texture2D>(new Texture2D());
    tex->m_UUID = Texture2DManager::CreateTexture2D(data);
    return tex;
}

void Texture2D::Bind(u32 slot)
{
    Texture2DManager::Bind(m_UUID, slot);
}
void Texture2D::UnBind()
{
    Texture2DManager::UnBind(m_UUID);
}

void Texture2D::SetData(u8* data, u32 width, u32 height)
{
    Texture2DManager::SetData(m_UUID, data, width, height);
}
void Texture2D::Resize(u32 nWidth, u32 uHeight)
{
    Texture2DManager::Resize(m_UUID, nWidth, uHeight);
}

void Texture2D::GenerateMipMaps()
{
    Texture2DManager::GenerateMipMaps(m_UUID);
}

void Texture2D::SetFiletering(TextureFiltering tf)
{
    Texture2DManager::SetFiletering(m_UUID, tf);
}
void Texture2D::SetWrap(TextureWrapMode tf)
{
    Texture2DManager::SetWrap(m_UUID, tf);
}
}
