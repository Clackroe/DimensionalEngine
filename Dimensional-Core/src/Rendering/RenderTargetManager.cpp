#include "Rendering/OpenGL/OpenGL_RenderTarget.hpp"
#include <Core/Application.hpp>
#include <Rendering/RenderTargetManager.hpp>

namespace Dimensional {
UMap<UUID, GraphicsRenderTarget> RenderTargetManager::s_RTMap;

UUID RenderTargetManager::CreateRenderTarget(const RenderTargetData& data)
{
    UUID id;
    GraphicsRenderTarget gTarget = {};

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        s_RTMap[id].glTarget = OpenGLRenderTarget::Create(data);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }

    return id;
}

void RenderTargetManager::Bind(UUID id)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Bind non-existant RenderTarget");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderTarget target = std::get<OpenGLRenderTarget>(s_RTMap.at(id).glTarget);
        target.Bind();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void RenderTargetManager::BindAttachment(UUID id, u32 index, u32 slot)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to BindAttachment on non-existant RenderTarget");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderTarget target = std::get<OpenGLRenderTarget>(s_RTMap.at(id).glTarget);
        target.BindAttachment(index, slot);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void RenderTargetManager::UnBind(UUID id)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to UnBind non-existant RenderTarget");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderTarget target = std::get<OpenGLRenderTarget>(s_RTMap.at(id).glTarget);
        target.UnBind();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void RenderTargetManager::SetSize(UUID id, u32 width, u32 height)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to SetSize non-existant RenderTarget");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderTarget target = std::get<OpenGLRenderTarget>(s_RTMap.at(id).glTarget);
        target.SetSize(width, height);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}
void RenderTargetManager::ReBuild(UUID id)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to ReBuild non-existant RenderTarget");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderTarget target = std::get<OpenGLRenderTarget>(s_RTMap.at(id).glTarget);
        target.ReBuild();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

Ref<Texture2D> RenderTargetManager::GetAttachment(UUID id, u32 index)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to GetAttachment on non-existant RenderTarget");
        return nullptr;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderTarget target = std::get<OpenGLRenderTarget>(s_RTMap.at(id).glTarget);
        return target.GetAttachment(index);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
    return nullptr;
}

void RenderTargetManager::Destroy(UUID id)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Destroy non-existant RenderTarget");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderTarget target = std::get<OpenGLRenderTarget>(s_RTMap.at(id).glTarget);
        target.Destroy();
        s_RTMap.erase(id);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

}
