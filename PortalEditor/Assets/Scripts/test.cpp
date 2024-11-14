#include "NativeScriptableEntity.hpp"
#include <NativeScripting.hpp>

std::vector<std::function<void()>> g_RegistrationFunctions;

class Test : public NativeScriptableEntity {
public:
    Test(u64 id)
        : NativeScriptableEntity(id)
    {
    }

    void update()
    {
        g_EngineAPI->LogWarn("ERM2...");
    };

    void create()
    {
        g_EngineAPI->LogWarn("I have been created");
    };

    void destroy()
    {
        g_EngineAPI->LogWarn("Destroyed");
    };
};

class Wow : public NativeScriptableEntity {
public:
    Wow(u64 id)
        : NativeScriptableEntity(id)
    {
    }

    void update()
    {
        g_EngineAPI->LogWarn("WOW2...");
    };

    void destroy()
    {
        g_EngineAPI->LogWarn("Destroyed");
    };
};

REGISTER_SCRIPT(Wow)
REGISTER_SCRIPT(Test)
