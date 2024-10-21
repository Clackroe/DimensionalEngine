#ifndef DM_UNIFORMBUFFER_HPP
#define DM_UNIFORMBUFFER_HPP

#include <core.hpp>

namespace Dimensional {

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
