#ifndef DM_RENDERGRAPH_HPP
#define DM_RENDERGRAPH_HPP

#include "Log/log.hpp"
#include "Rendering/FrameBuffer.hpp"
#include "Rendering/Renderer3d.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include <Rendering/RendererAPI.hpp>
#include <core.hpp>
#include <functional>

namespace Dimensional {

struct RenderPass {
    std::string debugName = "Untitled RenderPass";
    std::map<u32, Texture> inputTextures; // BindingIndex to Texture
    Ref<FrameBuffer> frameBuffer = nullptr;

    Ref<Shader> renderShader = nullptr;

    bool depthTest = true;
    bool faceCulling = true;

    // TODO: Do the same for blending

    DepthFunc depthFunction = DepthFunc::DEFAULT;
    FaceCulling culledFace = FaceCulling::DEFAULT;
    ClearBuffer clearBuffer = ClearBuffer::DEFAULT;

    std::function<void(Ref<Shader>, Ref<FrameBuffer>)> optionalSetupFunc;
    std::function<void(Ref<Shader>, Ref<FrameBuffer>)> renderFunc;
    std::function<void(Ref<Shader>, Ref<FrameBuffer>)> optionalCleanupFunc;
};

struct RenderGraph {
    std::vector<RenderPass> renderPasses;

    void pushRenderPass(RenderPass pass) { renderPasses.push_back(std::move(pass)); };

    void execute()
    {
        for (RenderPass& p : renderPasses) {
            RendererAPI& r = RendererAPI::getInstance();
            if (p.optionalSetupFunc) {
                p.optionalSetupFunc(p.renderShader, p.frameBuffer);
            }

            if (p.renderShader) {
                p.renderShader->use();
            } else {
                // DM_CORE_WARN("NO SHADER PROVIDED FOR RENDERPASS: {}", p.debugName);
            }

            if (p.frameBuffer) {
                p.frameBuffer->Bind();
            } else {
                DM_CORE_WARN("NO FRAMEBUFFER PROVIDED FOR RENDERPASS: {}", p.debugName);
            }

            for (auto& [bindID, texture] : p.inputTextures) {
                texture.bind(bindID);
            }

            if (p.depthTest) {
                r.enableDepthTest();
            } else {
                r.disableCulling();
            }
            if (p.faceCulling) {
                r.enableCulling();
            } else {
                r.disableCulling();
            }
            r.setDepthFunc(p.depthFunction);
            r.setCulling(p.culledFace);

            r.clearBuffer(p.clearBuffer);

            DM_CORE_ASSERT(p.renderFunc, "NO RENDER FUNC PROVIDED TO RENDER PASS " + p.debugName)
            p.renderFunc(p.renderShader, p.frameBuffer);

            r.setCulling(FaceCulling::DEFAULT);
            r.setDepthFunc(DepthFunc::DEFAULT);
            r.enableCulling();
            r.enableDepthTest();

            if (p.frameBuffer) {
                p.frameBuffer->Unbind();
            }

            if (p.optionalCleanupFunc) {
                p.optionalCleanupFunc(p.renderShader, p.frameBuffer);
            }
        }
    }
};
}

#endif
