#include <DefinesScriptLib.hpp>
#include <EngineAPI.hpp>
#include <NativeScripting.hpp>
#include <ReflectionHelpers.hpp>
#include <iostream>

namespace ScriptingCore {



void registerScripts(NativeScriptRegistry* reg)
{

    std::cout << "Registering Scripts..." << std::endl;

    std::cout << "Sizeof FuncList registerScripts: " << ScriptCoreLink::s_RegistrationFunctions.size() << std::endl;
    std::cout << "Sizeof FuncList registerScripts: " << ScriptCoreLink::getRegFuncs().size() << std::endl;
    for (auto& func : ScriptCoreLink::s_RegistrationFunctions) {
		std::cout << "----Registering Script----" << std::endl;
        func(reg);
    }
}

 void InitializeINT(EngineAPI* eAPI, ComponentAPI* compAPI, NativeScriptRegistry* registry)
{
    std::cout << "Test1: FROM LIBRARY ------ Hello There Again!" << std::endl;
    std::cout << "Sizeof FuncList Initialize: " << ScriptCoreLink::s_RegistrationFunctions.size() << std::endl;
    ScriptCoreLink::Init(eAPI, compAPI);
    registerScripts(registry);
}

void CleanupINT()
{
    std::cout << "Cleaning Up Library" << std::endl;
    ScriptCoreLink::ShutDown();
}
}
