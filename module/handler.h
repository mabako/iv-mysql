/*
 * Copyright (c) 2010-2011, mabako
 *
 * Based on mta-mysql module for Multi Theft Auto: San Andreas
 * Copyright (c) 2008, Alberto Alonso
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

#ifndef __HANDLER_H
#define __HANDLER_H

#include <string.h>
#ifdef _WIN32
	#include <WinSock2.h>
#endif
#include <mysql.h>
#include "result.h"

class MySQL
{
public:
	MySQL( const char* hostname, const char* username, const char* password, const char* database );
	~MySQL( );

	void Close( );

	const bool			OK( ) { return m_ok; }
	const unsigned int	Errno( ) { return mysql_errno( m_handle ); }
	const char*			Error( ) { return mysql_error( m_handle ); }

	const size_t		EscapeString( const char* str, char* dest, size_t len );
	const bool			Ping( );

	MySQL_Result*		Query( const char* query );
	const int			InsertID( );
	const int			AffectedRows( );
private:
	MYSQL* m_handle;
	bool m_ok;
};

#endif
