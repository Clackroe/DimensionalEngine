#ifndef DM_CUBEMAP_HPP
#define DM_CUBEMAP_HPP
#include <Rendering/FrameBuffer.hpp>
#include <Rendering/Shader.hpp>
#include <Rendering/Texture.hpp>
#include <core.hpp>
namespace Dimensional {

class DMCORE_API CubeMap {
public:
    CubeMap(std::string path, u32 w = 512, u32 h = 512);

    void bind(u32 slot);

private:
    u32 m_GLId;
    u32 m_Width, m_Height;

    Ref<Texture> m_EnvironmentTexture;

    static Ref<Shader> s_EquirectToCubeMap;

    void createMainTexture();
    void renderToCubeMap();

    static u32 s_CaptureFBO, s_DepthId;

    friend class IBLMap;
};
}
#endif
