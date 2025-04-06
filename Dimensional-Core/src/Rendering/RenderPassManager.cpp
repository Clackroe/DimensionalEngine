#include <Core/Application.hpp>
#include <Rendering/RenderPassManager.hpp>

namespace Dimensional {

UMap<UUID, GraphicsRenderPass> RenderPassManager::s_GraphicsRenderPass;

UUID RenderPassManager::CreateGPUBuffer(const RenderPassData& data)
{
    UUID id;
    GraphicsRenderPass gPass = {};

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        s_GraphicsRenderPass[id].glRenderPass = OpenGLRenderPass::Create(data);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }

    return id;
}

void RenderPassManager::Start(UUID id)
{
    bool exists = s_GraphicsRenderPass.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Start non-existant RenderPass");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderPass target = std::get<OpenGLRenderPass>(s_GraphicsRenderPass.at(id).glRenderPass);
        target.Start();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void RenderPassManager::Submit(UUID id, Ref<Mesh> mesh)
{
    bool exists = s_GraphicsRenderPass.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to  DrawSubMesh ond non-existant RenderPass");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderPass target = std::get<OpenGLRenderPass>(s_GraphicsRenderPass.at(id).glRenderPass);
        target.Submit(mesh);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void RenderPassManager::Submit(UUID id, Ref<SubMesh> sMesh)
{
    bool exists = s_GraphicsRenderPass.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to draw SubMesh  on non-existant RenderPass");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderPass target = std::get<OpenGLRenderPass>(s_GraphicsRenderPass.at(id).glRenderPass);
        target.Submit(sMesh);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void RenderPassManager::End(UUID id)
{
    bool exists = s_GraphicsRenderPass.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to End non-existant RenderPass");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderPass target = std::get<OpenGLRenderPass>(s_GraphicsRenderPass.at(id).glRenderPass);
        target.End();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void RenderPassManager::Destroy(UUID id)
{
    bool exists = s_GraphicsRenderPass.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Destroy non-existant RenderTarget");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLRenderPass pass = std::get<OpenGLRenderPass>(s_GraphicsRenderPass.at(id).glRenderPass);
        pass.Destroy();
        s_GraphicsRenderPass.erase(id);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

}
