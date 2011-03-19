//============== Copyright © 2010 IV:MP Team. All rights reserved. ==============
// File: SDK.cpp
//===============================================================================

#include "SDK.h"
#if defined(WINDOWS) || defined(_WIN32)
	#include <Windows.h>
#endif

FuncContainer_t FuncContainer;

EXPORT void SetupFunctions(FuncContainer_t * pContainer)
{
	FuncContainer = *pContainer;
}

EXPORT void SetupInterfaces(void * pContainer)
{
}

typedef void (*RegisterSquirrelFunction)(const char* szName, SQFUNCTION pFunction);

void RegisterFunction(HSQUIRRELVM pVM, const char * szName, SQFUNCTION pfnFunction)
{
	sq_pushroottable(pVM);
	sq_pushstring(pVM, szName, -1);
	sq_newclosure(pVM, pfnFunction, 0);
	sq_createslot(pVM, -3);
	sq_pop(pVM, 1);

#if defined(WINDOWS) || defined(_WIN32)
	HMODULE hModLua = GetModuleHandle("iv-modlua.dll");
	if(hModLua != NULL)
	{
		RegisterSquirrelFunction fn = (RegisterSquirrelFunction)GetProcAddress(hModLua, "RegisterSquirrelFunction");
		if(fn != NULL)
		{
			fn(szName, pfnFunction);
		}
	}
#endif
}
