#ifndef DM_SC_REGISTRATIONHELP
#define DM_SC_REGISTRATIONHELP

#include <CoreScriptLib.hpp>
#include <EngineAPI.hpp>
#include <NativeScriptableEntity.hpp>

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
        = [](int id) { return new Class(id); };
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
#endif