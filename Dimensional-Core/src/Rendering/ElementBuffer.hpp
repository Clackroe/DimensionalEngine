#ifndef DM_ELEMENTBUFFER_H
#define DM_ELEMENTBUFFER_H
#include <core.hpp>
namespace Dimensional {
class DMCORE_API ElementBuffer {
public:
    ElementBuffer(const u32* data, u32 count);
    ElementBuffer() = default;
    ~ElementBuffer();

    void Bind() const;
    void Unbind() const;

    u32 getCount() const { return m_Count; }

private:
    u32 m_GLId;
    u32 m_Count;
};
}

#endif
