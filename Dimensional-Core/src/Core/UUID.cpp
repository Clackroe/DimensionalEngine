#include <Core/UUID.hpp>
#include <random>
namespace Dimensional {

static ::std::random_device rd;
static ::std::mt19937_64 gen(rd());
static ::std::uniform_int_distribution<u64> dis;

UUID::UUID()
    : m_UUID(dis(gen))
{
}

UUID::UUID(u64 iUUID)
    : m_UUID(iUUID)
{
}

bool UUID::operator==(const UUID& other) const
{
    return m_UUID == other.m_UUID;
}

bool UUID::operator!=(const UUID& other) const
{
    return m_UUID != other.m_UUID;
}
}
