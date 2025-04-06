#ifndef RENDERPASS_AGNOST_HPP
#define RENDERPASS_AGNOST_HPP

namespace Dimensional {

struct RenderPassData {
};

struct RenderPass {
    static Ref<RenderPass> Create(const RenderPassData& data);

    static void Destroy();

private:
    RenderPass() = default;
};
}
#endif // RENDERPASS_AGNOST_HPP
