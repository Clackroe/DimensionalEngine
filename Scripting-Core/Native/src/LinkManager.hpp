#ifndef LINK_MANAGER_HPP
#define LINK_MANAGER_HPP

#include <DefinesScriptLib.hpp>

#include <EngineAPI.hpp>



namespace ScriptingCore {


	 void InitializeINT(EngineAPI* eAPI, ComponentAPI* compAPI, NativeScriptRegistry* registry);

	void CleanupINT();
}

#endif // LINK_MANAGER_HPP