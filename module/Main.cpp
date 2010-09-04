/*
 * Copyright (c) 2010, mabako
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice, this
 *       list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this
 *       list of conditions and the following disclaimer in the documentation and/or other
 *       materials provided with the distribution.
 *     * Neither the name of the mta-mysql nor the names of its contributors may be used
 *       to endorse or promote products derived from this software without specific prior
 *       written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "SDK/SDK.h"
#include "Functions.h"

const char * g_szModuleName = "MySQL Module";

/*
	This function is called when the module was loaded.

	Return
		true: Show a message confirming the module was loaded
		false: Don't show a message confirming the module was loaded
*/
EXPORT bool InitModule(char * szModuleName)
{
	strcpy(szModuleName, g_szModuleName);
	LogPrintf("MySQL Module 1.00 by mabako loaded.");
	return true;
}

/*
	This function is called when a script is loaded.
*/
EXPORT void ScriptLoad(HSQUIRRELVM S)
{
	RegisterFunction(S, "mysql_connect", sq_mysql_connect);
	RegisterFunction(S, "mysql_close", sq_mysql_close);
	RegisterFunction(S, "mysql_escape_string", sq_mysql_escape_string);
	RegisterFunction(S, "mysql_ping", sq_mysql_ping);
	RegisterFunction(S, "mysql_errno", sq_mysql_errno);
	RegisterFunction(S, "mysql_error", sq_mysql_error);
	RegisterFunction(S, "mysql_query", sq_mysql_query);
	RegisterFunction(S, "mysql_free_result", sq_mysql_free_result);
	RegisterFunction(S, "mysql_fetch_assoc", sq_mysql_fetch_assoc);
	RegisterFunction(S, "mysql_insert_id", sq_mysql_insert_id);
	RegisterFunction(S, "mysql_affected_rows", sq_mysql_affected_rows);
}

/*
	This function is called when a script is unloaded.
*/
EXPORT void ScriptUnload(HSQUIRRELVM S)
{
}

/*
	This function is called for internal server callbacks.
	Usage reserved.

	Return
		true: Callback was handled.
		false: Callback was not handled.
*/
EXPORT bool HandleCallback(eModuleCallback Callback, void *pArgs)
{
	if(Callback == TestCallback)
	{
		return true;
	}

	return false;
}

/*
	This function is called every server pulse.
	It is not recommended to run CPU-intensive code here.
*/
EXPORT void Pulse()
{
	
}
