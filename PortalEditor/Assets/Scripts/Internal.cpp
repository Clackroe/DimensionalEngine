
#include <iostream>
#include <LinkManager.hpp>









	extern "C" __declspec(dllexport) void Initialize(EngineAPI * eAPI, ComponentAPI * compAPI, NativeScriptRegistry * registry) {
		ScriptingCore::InitializeINT(eAPI, compAPI, registry);
	}

	extern "C" __declspec(dllexport) void Cleanup() {

		ScriptingCore::CleanupINT();
	}



__declspec(dllexport) void test()
{
    std::cout << "WOW" << std::endl;
}


