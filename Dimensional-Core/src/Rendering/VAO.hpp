#ifndef VAO_AGNO_HPP
#define VAO_AGNO_HPP
#include "Core/UUID.hpp"
#include "core.hpp"
#include <vector>
namespace Dimensional {

enum class AttributeType {
    FLOAT,
    INT,
    U32,
    BYTE,
    UBYTE,
};

static UMap<AttributeType, size_t> AttributeTypeToSize = {
    { AttributeType::FLOAT, sizeof(float) },
    { AttributeType::INT, sizeof(int) },
    { AttributeType::U32, sizeof(u32) },
    { AttributeType::BYTE, sizeof(char) },
    { AttributeType::UBYTE, sizeof(unsigned char) },
};

struct VertexAttribute {
    AttributeType type;
    u32 elementsCnt;
    bool normalized;
};

struct VAOData {
    const char* data;
    u32 dataSizeBytes;

    std::vector<VertexAttribute> layout;
    std::vector<u32> indexBuffer;
};

struct VAO {

    ~VAO();

    static Ref<VAO> Create();

    void Bind();
    void UnBind(); // Maybe unecessary

    void SetData(const VAOData& data);

    UUID GetUUID() { return m_UUID; }
    u32 GetElementCount() { return m_ElementCount; };

private:
    UUID m_UUID;
    u32 m_ElementCount = 0;
    VAO() = default;
};
}
#endif // VAO_AGNO_HPP
