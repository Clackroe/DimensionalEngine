#ifndef DM_UNIFORMBUFFER_HPP
#define DM_UNIFORMBUFFER_HPP

#include <core.hpp>

namespace Dimensional {

constexpr u32 alignST140(u32 size)
{
    return (size + 15) & ~(15);
}
template <typename T>
struct UBOPaddingInfo {
    static constexpr u32 size = sizeof(T);
    static constexpr u32 paddedSize = alignST140(size);
};

// Special one for vectors
template <typename T>
struct UBOPaddingInfo<std::vector<T>> {
    static constexpr u32 size = sizeof(T);
    static constexpr u32 paddedSize = alignST140(size);
};

class UniformBuffer {
public:
    UniformBuffer(u32 sizeBytes, u32 binding);
    ~UniformBuffer();

    void bind();
    void unbind();
    void setData(const void* data, u32 offset, u32 sizeBytes);

private:
    u32 m_GLId, m_Binding, m_SizeBytes;
};

}
#endif
