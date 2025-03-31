#include "Rendering/VAOManager.hpp"
#include "Core/Application.hpp"
#include "Log/log.hpp"
#include "Rendering/OpenGL/OpenGL_Buffer.hpp"
#include <Rendering/VAO.hpp>
namespace Dimensional {

UMap<UUID, GraphicsVAO> VAOManager::s_VAOMap;

UUID VAOManager::CreateVAO()
{
    UUID id;
    GraphicsVAO vao = {};

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        vao.glVAO = OpenGLVAO::Create();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        return 0;
        break;
    }
    }

    s_VAOMap[id] = vao;
    return id;
}
void VAOManager::Bind(UUID id)
{
    bool exists = s_VAOMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Bind non existant VAO...");
        return;
    }

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLVAO vao = s_VAOMap.at(id).glVAO;
        vao.Bind();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}
void VAOManager::UnBind(UUID id)
{
    bool exists = s_VAOMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to UnBind non existant VAO...");
        return;
    }
    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLVAO vao = s_VAOMap.at(id).glVAO;
        vao.UnBind();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

// should defer deletion using a command queue of some sort.
void VAOManager::SetData(UUID id, const VAOData& data)
{
    bool exists = s_VAOMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to UnBind non existant VAO...");
        return;
    }
    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLVAO vao = s_VAOMap.at(id).glVAO;
        vao.SetData(data);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void VAOManager::DeleteVAO(UUID id)
{

    bool exists = s_VAOMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to UnBind non existant VAO...");
        return;
    }

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLVAO vao = s_VAOMap.at(id).glVAO;
        vao.Destroy();
        s_VAOMap.erase(id);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

}
