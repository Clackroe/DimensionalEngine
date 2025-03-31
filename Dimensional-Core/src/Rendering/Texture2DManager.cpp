#include "Rendering/Texture2DManager.hpp"
#include "Rendering/OpenGL/OpenGL_Texture2D.hpp"
#include <Core/Application.hpp>
namespace Dimensional {

UMap<UUID, GraphicsTexture2D> Texture2DManager::s_Texture2DMap;

UUID Texture2DManager::CreateTexture2D(const Texture2DData& data)
{
    UUID id;
    GraphicsTexture2D gTexture2D = {};

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        gTexture2D.glTexture2D = OpenGLTexture2D::Create(data);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }

    s_Texture2DMap[id] = gTexture2D;
    return id;
}

void Texture2DManager::Bind(UUID id, u32 slot)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Bind non-existant Texture2D");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.Bind(slot);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}
void Texture2DManager::UnBind(UUID id)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to UnBind non-existant Texture2D");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.UnBind();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void Texture2DManager::SetData(UUID id, u8* data, u32 width, u32 height)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Set Data non-existant Texture2D");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.SetData(data, width, height);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}
void Texture2DManager::Resize(UUID id, u32 nWidth, u32 nHeight)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Resize non-existant Texture2D");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.Resize(nWidth, nHeight);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void Texture2DManager::GenerateMipMaps(UUID id)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to GenerateMipMaps non-existant Texture2D");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.GenerateMipMaps();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void Texture2DManager::SetFiletering(UUID id, TextureFiltering tf)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to SetFiltering non-existant Texture2D");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.SetFiletering(tf);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}
void Texture2DManager::SetWrap(UUID id, TextureWrapMode tf)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to SetWrap non-existant Texture2D");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.SetWrap(tf);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void Texture2DManager::DeleteTexture2D(UUID id)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Delete non-existant shader");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.Destroy();
        s_Texture2DMap.erase(id);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void Texture2DManager::SetBorderColor(UUID id, glm::vec4 col)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to SetBorderColor on non-existant Texture");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLTexture2D tex = s_Texture2DMap.at(id).glTexture2D;
        tex.SetBorderColor(col);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

OpenGLTexture2D Texture2DManager::GetOpenGLTexture(UUID id)
{
    bool exists = s_Texture2DMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Get OpenGL Texture on non-existant Texture");
        return OpenGLTexture2D();
    };
    return s_Texture2DMap.at(id).glTexture2D;
}

}
