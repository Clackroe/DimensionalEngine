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
};

struct RendererInfo {
};

struct Renderer {

    static void Init(const RendererInfo& data);
    static void Shutdown();

    static nvrhi::BindingLayoutHandle GetConstantBindingLayout();
    static nvrhi::BindingSetHandle GetConstantBindingSet();
};
}

#endif // RENDERER_HPP
