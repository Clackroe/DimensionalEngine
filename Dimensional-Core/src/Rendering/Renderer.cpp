#include "Log/log.hpp"
#include "Rendering/OpenGL/OpenGL_Renderer.hpp"
#include <Rendering/Renderer.hpp>

#include <Core/Window.hpp>

namespace Dimensional {

RendererAPI Renderer::s_API;

struct RendererData {
};
static RendererData s_RendererData;

void Renderer::DrawIndexed(Ref<VAO> vao, Ref<Shader> shader)
{

    switch (s_API.api) {
    case GraphicsAPI::OPENGL:
        OpenGLRenderer::DrawIndexed(vao, shader);
        break;
    case GraphicsAPI::UNKOWN:
        DM_CORE_WARN("Tried to set Window Hints with Uninitialized API");
        break;
    }
}

void Renderer::SetClearColor(glm::vec3 color)
{
    switch (s_API.api) {
    case GraphicsAPI::OPENGL:
        OpenGLRenderer::SetClearColor(color);
        break;
    case GraphicsAPI::UNKOWN:
        DM_CORE_WARN("Tried to set Clear Color with Uninitialized API");
        break;
    }
}

void Renderer::ClearScreen(ClearBuffer cb)
{
    switch (s_API.api) {
    case GraphicsAPI::OPENGL:
        OpenGLRenderer::ClearScreen(cb);
        break;
    case GraphicsAPI::UNKOWN:
        DM_CORE_WARN("Tried to set Window Hints with Uninitialized API");
        break;
    }
}

void Renderer::SetWindowHints()
{
    switch (s_API.api) {
    case GraphicsAPI::OPENGL:
        OpenGLRenderer::SetWindowHints();
        break;
    case GraphicsAPI::UNKOWN:
        DM_CORE_WARN("Tried to set Window Hints with Uninitialized API");
        break;
    }
}

void Renderer::SetAPI(GraphicsAPI api)
{
    switch (api) {
    case GraphicsAPI::OPENGL:
        s_API = OpenGLRenderer::GetAPI();
        break;
    case GraphicsAPI::UNKOWN:
        DM_CORE_WARN("Tried to set GraphicsAPI to Unkown? Are you OK?");
        break;
    }
}

void Renderer::Init(GraphicsAPI api, Window& window)
{
    if (s_API.api != GraphicsAPI::UNKOWN && s_API.Shutdown) {
        s_API.Shutdown();
    }
    Renderer::SetAPI(api);
    s_API.Init(window);
}

void Renderer::Shutdown()
{
    if (s_API.api != GraphicsAPI::UNKOWN) {
        s_API.Shutdown();
        return;
    }
    DM_CORE_WARN("Tried to shutdown already shutdown or Unkown Renderer");
}

void Renderer::PushCommand()
{
}

}
