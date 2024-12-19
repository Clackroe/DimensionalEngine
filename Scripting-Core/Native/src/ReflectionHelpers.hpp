#ifndef DM_SC_REGISTRATIONHELP
#define DM_SC_REGISTRATIONHELP

#include "NativeScripting.hpp"
#include <CoreScriptLib.hpp>
#include <EngineAPI.hpp>
#include <NativeScriptableEntity.hpp>

#include <cstring>

namespace ScriptingCore {

template <typename Class, typename = void>
struct has_create : std::false_type { };

template <typename Class>
struct has_create<Class, std::void_t<decltype(&Class::create)>> : std::true_type { };

template <typename Class, typename = void>
struct has_update : std::false_type { };

template <typename Class>
struct has_update<Class, std::void_t<decltype(&Class::update)>> : std::true_type { };

template <typename Class, typename = void>
struct has_destroy : std::false_type { };

template <typename Class>
struct has_destroy<Class, std::void_t<decltype(&Class::destroy)>> : std::true_type { };

#ifdef __GNUC__ // GCC or Clang
#include <cxxabi.h>
inline std::string demangle(const char* name)
{
    int status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result(demangled ? demangled : name);
    free(demangled);
    return result;
}
#else
#include <string>
inline std::string demangle(const char* name)
{
    return name;
}
#endif
template <typename Class>
inline void registerClass(NativeScriptRegistry* reg)
{
    ReflectedData data {};
    data.className = demangle(typeid(Class).name());

    data.classFactory = [](uint64_t id) { return new Class(id); };
    data.classDestructor = [](NativeScriptableEntity* pointer) {
        delete static_cast<Class*>(pointer);
    };

    data.onUpdate = [](NativeScriptableEntity* pointer) {
        Class* typedPointer = static_cast<Class*>(pointer);
        if constexpr (has_update<Class>::value) {
            typedPointer->update();
        }
    };

    data.onCreate = [](NativeScriptableEntity* pointer) {
        Class* typedPointer = static_cast<Class*>(pointer);
        if constexpr (has_create<Class>::value) {
            typedPointer->create();
        }
    };

    data.onDestroy = [](NativeScriptableEntity* pointer) {
        Class* typedPointer = static_cast<Class*>(pointer);
        if constexpr (has_destroy<Class>::value) {
            typedPointer->destroy();
        }
    };
    data.memberData = Class::registerMembers();
    reg->insert({ data.className, data });
}

template <typename Class>
struct ClassRegistrar {
    static bool registered;

    ClassRegistrar()
    {
        ScriptCoreLink::getRegFuncs().push_back([](NativeScriptRegistry* reg) { registerClass<Class>(reg); });
    }
};

template <typename Class>
bool ClassRegistrar<Class>::registered = [] {
    static ClassRegistrar<Class> instance;
    return true;
}();

#define REGISTER_SCRIPT(CLASS)                          \
    namespace ScriptingCore {                           \
        static ClassRegistrar<CLASS> registrar_##CLASS; \
    }

#define DM_GENERATED_BODY(Class)                                                \
public:                                                                         \
    static std::vector<std::function<MemberData()>>& getMemberRegFuncs()        \
    {                                                                           \
        static std::vector<std::function<MemberData()>> s_RegisterMembersFuncs; \
        return s_RegisterMembersFuncs;                                          \
    }                                                                           \
    static std::vector<MemberData> registerMembers()                            \
    {                                                                           \
        std::vector<MemberData> members;                                        \
        for (auto& func : getMemberRegFuncs()) {                                \
            MemberData d = func();                                              \
            members.push_back(d);                                               \
        }                                                                       \
        return members;                                                         \
    }

#define DM_PROPERTY(Class, type, name, defaultValue)                            \
    type name = defaultValue;                                                   \
    struct _MemberRegister_##name {                                             \
        _MemberRegister_##name()                                                \
        {                                                                       \
            getMemberRegFuncs().push_back([]() -> MemberData {                  \
                MemberData data;                                                \
                data.varName = #name;                                           \
                data.getter = [](NativeScriptableEntity* entity) -> void* {     \
                    auto* obj = static_cast<Class*>(entity);                    \
                    return &(obj->name);                                        \
                };                                                              \
                data.dataType = Dimensional::g_StringToScriptMember.at(#type);  \
                data.setter = [](NativeScriptableEntity* entity, void* value) { \
                    auto* obj = static_cast<Class*>(entity);                    \
                    obj->name = *static_cast<type*>(value);                     \
                };                                                              \
                type temp_##name = defaultValue;                                \
                memcpy(data.defaultVal, &temp_##name, MAX_MEMBERDATA_SIZE);     \
                return data;                                                    \
            });                                                                 \
        }                                                                       \
    };                                                                          \
    static _MemberRegister_##name _memberRegister_instance_##name;

}

#define REGISTER_PROPERTY(Class, name) \
    Class::_MemberRegister_##name Class::_memberRegister_instance_##name;

#endif
