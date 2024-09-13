#ifndef DM_IR_MAP
#define DM_IR_MAP

#include <Rendering/CubeMap.hpp>
#include <Rendering/FrameBuffer.hpp>
#include <Rendering/Shader.hpp>
#include <core.hpp>
namespace Dimensional {

class DMCORE_API IrMap {
public:
    IrMap(Ref<CubeMap> map);

    void bind(u32 slot);

private:
    u32 m_GLId;
    u32 m_Width, m_Height;

    Ref<CubeMap> m_CubeMap;

    static Ref<Shader> s_ConvShader;

    void build();

    void createMainTexture();
    void renderToCubeMap();

    static u32 s_CaptureFBOIBL, s_DepthIdIBL;
};
}
#endif
