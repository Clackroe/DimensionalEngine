
#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "Rendering/OpenGL/OpenGL_Shader.hpp"
#include <Rendering/ShaderManager.hpp>

namespace Dimensional {

UUID ShaderManager::CreateShader(const ShaderData& data)
{
    UUID id;
    GraphicsShader gShader = {};

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        gShader.glShader = OpenGLShader::Create(data);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }

    s_ShaderMap[id] = gShader;
    return id;
}

void ShaderManager::Bind(UUID id)
{
    bool exists = s_ShaderMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Bind non-existant shader");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLShader shad = s_ShaderMap.at(id).glShader;
        shad.Bind();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}
void ShaderManager::UnBind(UUID id)
{
    bool exists = s_ShaderMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to UnBind non-existant shader");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLShader shad = s_ShaderMap.at(id).glShader;
        shad.UnBind();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }

} // Maybe unecessary

void ShaderManager::DeleteShader(UUID id)
{
    bool exists = s_ShaderMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Delete non-existant shader");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLShader shad = s_ShaderMap.at(id).glShader;
        shad.Destroy();
        s_ShaderMap.erase(id);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

UMap<UUID, GraphicsShader> ShaderManager::s_ShaderMap;

}
