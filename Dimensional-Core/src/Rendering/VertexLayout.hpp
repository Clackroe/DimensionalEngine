#ifndef DM_VERTEXLAYOUT_H
#define DM_VERTEXLAYOUT_H
#include <core.hpp>
namespace Dimensional {

// Forward declaration of the GL constants
extern const u32 GL_FLOAT;
extern const u32 GL_UNSIGNED_INT;
extern const u32 GL_UNSIGNED_BYTE;

struct DMCORE_API vbElement {
    u32 type;
    u32 count;
    u8 normalized;

    // Declare the static method
    static u32 GetSizeOfType(u32 type);
};

class DMCORE_API VertexLayout {
public:
    VertexLayout();
    ~VertexLayout();

    template <typename T>
    void Push(u32 count);

#ifdef DM_WINDOWS

    template <>
    void Push<float>(u32 count);

    template <>
    void Push<u32>(u32 count);

    template <>
    void Push<u8>(unsigned int count);
#endif

    inline const std::vector<vbElement>& getElements() const { return m_Elements; }
    inline unsigned int getStride() const { return m_Stride; }

private:
    std::vector<vbElement> m_Elements;
    u32 m_Stride;
};
}

#endif
