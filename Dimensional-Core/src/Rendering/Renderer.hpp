#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "nvrhi/nvrhi.h"
#include <Core/Window.hpp>
namespace Dimensional {

enum class RESOURCE_DOMAIN {
    CONSTANT = 0,
    FRAME = 1,
    PIPELINE = 2,
    MATERIAL = 3,
    UNKNOWN
};

enum class VERTEX_BUFFER_TYPE {
    VERTEX_DATA = 0, // Maps to slot
    INSTANCE_DATE = 1,
    UNKNOWN
};

struct RendererInfo {
};

struct Renderer {

    static void Init(const RendererInfo& data);
    static void Shutdown();

    static nvrhi::GraphicsState GetDefaultGraphicsState();

    // static void ApplyConstantState(nvrhi::GraphicsState& state);

    static nvrhi::BindingLayoutHandle GetConstantBindingLayout();
    static nvrhi::BindingSetHandle GetConstantBindingSet();
};
}

#endif // RENDERER_HPP
