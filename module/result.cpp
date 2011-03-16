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

#include "result.h"

MySQL_Result::MySQL_Result( MYSQL_RES* res )
{
	m_result = res;
}

MySQL_Result::~MySQL_Result( )
{
	if( !Empty( ) )
		mysql_free_result( m_result );
}

const bool MySQL_Result::Empty( )
{
	return m_result == 0;
}

MYSQL_ROW MySQL_Result::FetchRow( )
{
	if( !Empty( ) )
		return mysql_fetch_row( m_result );
	return 0;
}

unsigned int MySQL_Result::FieldSeek( unsigned int offset )
{
	return mysql_field_seek( m_result, offset );
}

MYSQL_FIELD* MySQL_Result::FetchField( )
{
	return mysql_fetch_field( m_result );
}
