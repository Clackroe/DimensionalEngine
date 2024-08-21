#ifndef DM_ELEMENTBUFFER_H
#define DM_ELEMENTBUFFER_H
namespace Dimensional {
class ElementBuffer {
public:
    ElementBuffer(const u32* data, u32 count);
    ~ElementBuffer();

    void Bind() const;
    void Unbind() const;

    u32 getCount() { return m_Count; }

private:
    u32 m_GLId;
    u32 m_Count;
};
}

#endif
