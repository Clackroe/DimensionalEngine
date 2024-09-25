#ifndef DM_UUID_H
#define DM_UUID_H
#include <core.hpp>

namespace Dimensional {
class DMCORE_API UUID {
public:
    UUID();
    UUID(u64 id);
    UUID(const UUID&) = default;

    operator u64() const { return m_UUID; }
    bool operator==(const UUID& other) const;
    bool operator!=(const UUID& other) const;

private:
    u64 m_UUID;
};
}

namespace std {
template <typename T>
struct hash;

template <>
struct hash<Dimensional::UUID> {
    std::size_t operator()(const Dimensional::UUID& id) const
    {
        return (u64)id;
    }
};
}

#endif
