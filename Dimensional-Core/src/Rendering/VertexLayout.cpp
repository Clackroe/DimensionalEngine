#include <Rendering/VertexLayout.hpp>
#include <core.hpp>
#include <glad.h>

namespace Dimensional {

u32 vbElement::GetSizeOfType(u32 type)
{
	switch (type) {
	case GL_FLOAT:
		return 4;
	case GL_UNSIGNED_INT:
		return 4;
	case GL_UNSIGNED_BYTE:
		return 1;
	}
	return 0;
};

VertexLayout::VertexLayout()
    : m_Stride(0)
{
}

VertexLayout::~VertexLayout()
{
}

template <typename T>
void VertexLayout::Push(u32 count)
{
    DM_CORE_ASSERT(false, "UNKNOWN ELEMENT TYPE");
}

template <>
void VertexLayout::Push<float>(u32 count)
{
    m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
    m_Stride += count * vbElement::GetSizeOfType(GL_FLOAT);
}

template <>
void VertexLayout::Push<u32>(u32 count)
{
    m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
    m_Stride += count * vbElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template <>
void VertexLayout::Push<u8>(unsigned int count)
{
    m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
    m_Stride += count * vbElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}
};
