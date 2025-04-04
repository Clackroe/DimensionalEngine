#include "Rendering/GPUBufferManager.hpp"
#include "Core/UUID.hpp"
#include "Rendering/OpenGL/OpenGL_Buffer.hpp"
#include <Core/Application.hpp>

namespace Dimensional {

UMap<UUID, GraphicsGPUBuffer> GPUBufferManager::s_GPUBuffMap;

UUID GPUBufferManager::CreateGPUBuffer(const GPUBufferData& data)
{
    UUID id;
    GraphicsGPUBuffer gTarget = {};

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        s_GPUBuffMap[id].glBuffer = OpenGLGPUBuffer::Create(data);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }

    return id;
}

void GPUBufferManager::Bind(UUID id, u32 slot)
{
    bool exists = s_GPUBuffMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Bind non-existan GPUBuffer");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLGPUBuffer buff = std::get<OpenGLGPUBuffer>(s_GPUBuffMap.at(id).glBuffer);
        buff.Bind(slot);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void GPUBufferManager::Resize(UUID id, size_t sizeBytes)
{
    bool exists = s_GPUBuffMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Resuze on non-existan GPUBuffer");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLGPUBuffer buff = std::get<OpenGLGPUBuffer>(s_GPUBuffMap.at(id).glBuffer);
        buff.Resize(sizeBytes);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void GPUBufferManager::SetData(UUID id, const void* data, size_t offset, size_t sizeBytes)
{
    bool exists = s_GPUBuffMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to SetData on non-existan GPUBuffer");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLGPUBuffer buff = std::get<OpenGLGPUBuffer>(s_GPUBuffMap.at(id).glBuffer);
        buff.SetData(data, offset, sizeBytes);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void GPUBufferManager::Destroy(UUID id)
{
    bool exists = s_GPUBuffMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Destroy non-existan GPUBuffer");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLGPUBuffer buff = std::get<OpenGLGPUBuffer>(s_GPUBuffMap.at(id).glBuffer);
        buff.Destroy();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

}
