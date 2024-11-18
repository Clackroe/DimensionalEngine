#ifndef DM_SC_REGISTRATIONHELP
#define DM_SC_REGISTRATIONHELP

#include <CoreScriptLib.hpp>
#include <EngineAPI.hpp>
#include <NativeScriptableEntity.hpp>

#include <cstring>

extern NativeScriptRegistry g_ScriptRegistry;
extern std::vector<std::function<void()>> g_RegistrationFunctions;

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
void registerClass()
{
    ScriptableEntityData data {};
    data.className = demangle(typeid(Class).name());

    data.classFactory
        = [](uint64_t id) { return new Class(id); };
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
    g_ScriptRegistry.scriptRegistry[data.className] = data;
}

template <typename Class>
struct ClassRegistrar {
    static bool registered;

    ClassRegistrar()
    {
        g_RegistrationFunctions.push_back([]() { registerClass<Class>(); });
    }
};

template <typename Class>
bool ClassRegistrar<Class>::registered = [] {
    static ClassRegistrar<Class> instance;
    return true;
}();

#define REGISTER_SCRIPT(CLASS) static ClassRegistrar<CLASS> registrar_##CLASS;

#define DM_GENERATED_BODY(Class)                                                   \
public:                                                                            \
    inline static std::vector<std::function<MemberData()>> s_RegisterMembersFuncs; \
    static std::vector<MemberData> registerMembers()                               \
    {                                                                              \
        std::vector<MemberData> members;                                           \
        for (auto& func : s_RegisterMembersFuncs) {                                \
            MemberData d = func();                                                 \
            members.push_back(d);                                                  \
        }                                                                          \
        return members;                                                            \
    }                                                                              \
    static void ReflectMembers()                                                   \
    {                                                                              \
    }

#define DM_PROPERTY(Class, type, name, defaultValue)                                       \
    type name = defaultValue;                                                              \
    struct _MemberRegister_##name {                                                        \
        _MemberRegister_##name()                                                           \
        {                                                                                  \
            size_t offset = offsetof(Class, name);                                         \
            s_RegisterMembersFuncs.push_back([offset]() -> MemberData {                    \
                MemberData data;                                                           \
                data.varName = #name;                                                      \
                data.offsetBytes = offset;                                                 \
                data.dataType = g_SupportedMemebersToType.at(#type);                       \
                data.getter = [offset](NativeScriptableEntity* entity) -> void* {          \
                    auto* obj = static_cast<Class*>(entity);                               \
                    return reinterpret_cast<void*>(reinterpret_cast<char*>(obj) + offset); \
                };                                                                         \
                data.setter = [offset](NativeScriptableEntity* entity, void* value) {      \
                    auto* obj = static_cast<Class*>(entity);                               \
                    auto* member = reinterpret_cast<void*>(                                \
                        reinterpret_cast<char*>(obj) + offset);                            \
                    std::memcpy(member, value, sizeof(type));                              \
                };                                                                         \
                return data;                                                               \
            });                                                                            \
        }                                                                                  \
    };                                                                                     \
    inline static _MemberRegister_##name _memberRegister_instance_##name;

#endif
