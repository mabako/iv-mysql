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

#include "handler.h"

MySQL::MySQL( const char* hostname, const char* username, const char* password, const char* database )
{
	m_ok = false;
	if( ( m_handle = mysql_init(0) ) )
	{
		my_bool reconnect = false;
		mysql_options( m_handle, MYSQL_OPT_RECONNECT, &reconnect );

		if( mysql_real_connect( m_handle, hostname, username, password, database, 3306, 0, CLIENT_COMPRESS | CLIENT_SSL ) )
			m_ok = !mysql_select_db( m_handle, database );
	}
}

MySQL::~MySQL( )
{
	Close( );
}

void MySQL::Close( )
{
	if( m_handle )
	{
		mysql_close( m_handle );
		m_handle = 0;
		m_ok = false;
	}
}

const size_t MySQL::EscapeString( const char* str, char* dest, size_t len )
{
	return mysql_real_escape_string( m_handle, dest, str, static_cast< unsigned long >( len ) );
}

const bool MySQL::Ping( )
{
	return mysql_ping( m_handle ) == 0;
}

MySQL_Result* MySQL::Query( const char* query )
{
	MYSQL_RES* res;

	if( mysql_real_query( m_handle, query, static_cast < unsigned long > ( strlen( query ) ) ) )
		return 0;

	res = mysql_store_result( m_handle );
	if( res == 0 && mysql_field_count( m_handle ) > 0 )
		return 0;

	MySQL_Result* result = new MySQL_Result( res );
	return result;
}

const int MySQL::InsertID( )
{
	return static_cast < int > ( mysql_insert_id( m_handle ) );
}

const int MySQL::AffectedRows( )
{
	return static_cast < int > ( mysql_affected_rows( m_handle ) );
}
