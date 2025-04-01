#include "Rendering/GPUBufferManager.hpp"
#include "Core/UUID.hpp"
#include "Rendering/OpenGL/OpenGL_Buffer.hpp"
#include <Core/Application.hpp>

namespace Dimensional {

UMap<UUID, GraphicsGPUBuffer> GPUBufferManager::s_RTMap;

UUID GPUBufferManager::CreateGPUBuffer(const GPUBufferData& data)
{
    UUID id;
    GraphicsGPUBuffer gTarget = {};

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        s_RTMap[id].glBuffer = OpenGLGPUBuffer::Create(data);
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
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Bind non-existan GPUBuffer");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLGPUBuffer buff = s_RTMap.at(id).glBuffer;
        buff.Bind(slot);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void GPUBufferManager::SetData(UUID id, const void* data, size_t sizeBytes)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to SetData on non-existan GPUBuffer");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLGPUBuffer buff = s_RTMap.at(id).glBuffer;
        buff.SetData(data, sizeBytes);
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

void GPUBufferManager::Destroy(UUID id)
{
    bool exists = s_RTMap.contains(id);
    if (!exists) {
        DM_CORE_WARN("Tried to Destroy non-existan GPUBuffer");
        return;
    };

    switch (Application::getGraphicsAPI()) {
    case GraphicsAPI::OPENGL: {
        OpenGLGPUBuffer buff = s_RTMap.at(id).glBuffer;
        buff.Destroy();
        break;
    }
    case GraphicsAPI::UNKOWN: {
        break;
    }
    }
}

}
