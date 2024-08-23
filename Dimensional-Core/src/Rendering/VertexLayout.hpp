#ifndef DM_VERTEXLAYOUT_H
#define DM_VERTEXLAYOUT_H
namespace Dimensional {

struct vbElement;

class VertexLayout {
public:
    VertexLayout();
    ~VertexLayout();

    template <typename T>
    void Push(u32 count);

    inline const std::vector<vbElement>& getElements() const { return m_Elements; }
    inline unsigned int getStride() const { return m_Stride; }

private:
    std::vector<vbElement> m_Elements;
    u32 m_Stride;
};
}

#endif
