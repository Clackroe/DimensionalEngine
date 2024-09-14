#ifndef DM_IBL_MAP
#define DM_IBL_MAP

#include <Rendering/CubeMap.hpp>
#include <Rendering/FrameBuffer.hpp>
#include <Rendering/Shader.hpp>
#include <core.hpp>
namespace Dimensional {

class IBLMap {
public:
    IBLMap(Ref<CubeMap> map);

    void bind(u32 slotIBLMap, u32 slotBRDF);
    u32 getID() const { return m_GLId; };
    u32 getBRDFID() const { return m_BRDFId; };

private:
    u32 m_GLId, m_BRDFId;
    u32 m_Width, m_Height;

    Ref<CubeMap> m_CubeMap;

    static Ref<Shader> s_PreComp;
    static Ref<Shader> s_BRDFComp;

    void build();

    void createMainTexture();
    void createBRDFTexture();

    void renderBRDF();
    void renderToCubeMap();

    static u32 s_CaptureFBOIBL, s_DepthIdIBL;
};
}
#endif
