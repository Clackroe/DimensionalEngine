#ifndef DATA_STRUCTS_HPP
#define DATA_STRUCTS_HPP
#include <map>
#include <string>
#include <vector>

enum class EVENTS {
    UPDATE,
    DESTROY,
    CREATE
};
const std::map<EVENTS, std::string> g_EventsMap = {
    { EVENTS::UPDATE, "update" },
    { EVENTS::DESTROY, "destroy" },
    { EVENTS::CREATE, "create" }
};

struct PropertyMeta {
    std::string name;
    std::string type;
    std::string def;
};

struct ClassMeta {
    std::string name;
    std::string path;
    bool hasUpdate = false;
    bool hasDestroy = false;
    bool hasCreate = false;
    std::vector<PropertyMeta> props;
};

#endif // DATA_STRUCTS_HPP
