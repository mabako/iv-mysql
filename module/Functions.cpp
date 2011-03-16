/*
 * Copyright (c) 2010-2011, mabako
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
#include "handler.h"
#include "result.h"
#include <string>
#include <stdlib.h>
#include <list>
using namespace std;

list< MySQL* > handlers;
list< MySQL_Result* > results;

/* Easier to write */
#define vm HSQUIRRELVM

/* Checks if the parameter at index idx is a valid MySQL Handler, if so stores it in the 'handler' variable, otherwise returns an error and exits. */
#define CHECK_HANDLER(idx) \
	MySQL* handler = 0; \
	{ \
		SQUserPointer sqTemp; \
		sq_getuserpointer( S, idx+1, &sqTemp ); \
		for( list< MySQL* >::iterator iter = handlers.begin( ); iter != handlers.end( ); ++ iter ) \
		{ \
			if( *iter == sqTemp ) \
			{ \
				handler = (MySQL*)sqTemp; \
				break; \
			} \
		} \
		if( handler == 0 ) \
		{ \
			LogPrintf( "Invalid MySQL-Handler" ); \
			sq_pushbool( S, false ); \
			return 1; \
		} \
	}

/* Checks if the parameter at index idx is a valid MySQL Result, if so stores it in the 'result' variable, otherwise returns an error and exits. */
#define CHECK_RESULT(idx) \
	MySQL_Result* result = 0; \
	{ \
		SQUserPointer sqTemp; \
		sq_getuserpointer( S, idx+1, &sqTemp ); \
		for( list< MySQL_Result* >::iterator iter = results.begin( ); iter != results.end( ); ++ iter ) \
		{ \
			if( *iter == sqTemp ) \
			{ \
				result = (MySQL_Result*)sqTemp; \
				break; \
			} \
		} \
		if( result == 0 ) \
		{ \
			LogPrintf( "Invalid MySQL-Result" ); \
			sq_pushbool( S, false ); \
			return 1; \
		} \
	}

/* Checks if the function has AT LEAST the given number of parameters */
#define CHECK_PARAMS(count) \
	int numparams = sq_gettop(S); \
	if( numparams <= count ) \
	{ \
		LogPrintf( "Invalid Parameter count, need %d, got %d", count, numparams - 1 ); \
		sq_pushbool( S, false ); \
		return 1; \
	}

/* Obtains a string from the parameters */
#define GET_STRING(idx, to) \
	sq_getstring( S, idx+1, &to )


/* mysql_connect( string hostname, string username, string password, string database );
 *   Connects to the given server
 */
int sq_mysql_connect(vm S)
{
	CHECK_PARAMS( 4 );

	const char* hostname;
	const char* username;
	const char* password;
	const char* database;

	GET_STRING( 1, hostname );
	GET_STRING( 2, username );
	GET_STRING( 3, password );
	GET_STRING( 4, database );

	MySQL* handler = new MySQL( hostname, username, password, database );
	if( handler->OK( ) )
	{
		handlers.push_back( handler );
		sq_pushuserpointer( S, handler );
	}
	else
	{
		LogPrintf( "MySQL Connection failed (#%d): %s", handler->Errno( ), handler->Error( ) );
		delete handler;
		sq_pushbool( S, false );
	}
	return 1;
}

/* mysql_close( mysql_handler handler );
 *   Closes the connection to the server
 */
int sq_mysql_close(vm S)
{
	CHECK_PARAMS( 1 );
	CHECK_HANDLER( 1 );

	handler->Close( );
	handlers.remove( handler );
	delete handler;

	sq_pushbool( S, true );
	return 1;
}

/* mysql_escape_string( mysql_handler handler, string text );
 *   Escapes the text
 */
int sq_mysql_escape_string(vm S)
{
	CHECK_PARAMS( 2 );
	CHECK_HANDLER( 1 );

	const char* string;
	GET_STRING( 2, string );

	/* Create a new string to push back */
	size_t len = strlen( string );
	char* result = new char[ ( len << 1 ) + 1 ];
	size_t newlen = handler->EscapeString( string, result, len );
	sq_pushstring( S, result, newlen );
	return 1;
}

/* mysql_ping( mysql_handler handler );
 *   Returns true if the connection is still valid
 */
int sq_mysql_ping(vm S)
{
	CHECK_PARAMS( 1 );
	CHECK_HANDLER( 1 );

	sq_pushbool( S, handler->Ping( ) );
	return 1;
}

/* mysql_errno( mysql_handler handler );
 *   returns the error number (see mysql_error for text)
 */
int sq_mysql_errno(vm S)
{
	CHECK_PARAMS( 1 );
	CHECK_HANDLER( 1 );

	sq_pushinteger( S, handler->Errno( ) );
	return 1;
}

/* mysql_error( mysql_handler handler );
 *   returns the error text (see mysql_errno for number)
 */
int sq_mysql_error(vm S)
{
	CHECK_PARAMS( 1 );
	CHECK_HANDLER( 1 );

	const char* error = handler->Error( );
	sq_pushstring( S, error, strlen( error ) );
	return 1;
}

/* mysql_query( mysql_handler handler, string query )
 *   Executes a MySQL query
 */
int sq_mysql_query(vm S)
{
	CHECK_PARAMS( 2 );
	CHECK_HANDLER( 1 );
	
	const char* query;
	GET_STRING( 2, query );

	MySQL_Result* result = handler->Query( query );
	if( result )
	{
		results.push_back( result );
		sq_pushuserpointer( S, result );
	}
	else
	{
		sq_pushbool( S, false );
	}
	return 1;
}

/* mysql_free_result( mysql_result result );
 *   Frees a previously allocated MySQL Result
 */
int sq_mysql_free_result(vm S)
{
	CHECK_PARAMS( 1 );
	CHECK_RESULT( 1 );

	results.remove( result );
	delete result;

	sq_pushbool( S, true );
	return 1;
}

/* mysql_fetch_assoc( mysql_result result );
 *   Returns the key=value table for a single row in a result
 */
int sq_mysql_fetch_assoc(vm S)
{
	CHECK_PARAMS( 1 );
	CHECK_RESULT( 1 );

	if( !result->Empty( ) )
	{
		MYSQL_ROW row = result->FetchRow( );
		if( row )
		{
			/* Build our table */
			sq_newtable( S );

			result->FieldSeek( 0 );
			MYSQL_FIELD* field;
			int i = 0;
			for( field = result->FetchField( ), i = 0; field != 0; field = result->FetchField( ), i ++ )
			{
				sq_pushstring( S, field->name, field->name_length );
				if( row[i] )
				{
					switch( field->type )
					{
						case MYSQL_TYPE_DECIMAL:
						case MYSQL_TYPE_NEWDECIMAL:
						case MYSQL_TYPE_FLOAT:
						case MYSQL_TYPE_DOUBLE:
							sq_pushfloat( S, static_cast < float > ( atof( row[i] ) ) );
							break;
						case MYSQL_TYPE_TINY:
						case MYSQL_TYPE_SHORT:
						case MYSQL_TYPE_LONG:
						case MYSQL_TYPE_LONGLONG:
						case MYSQL_TYPE_INT24:
						case MYSQL_TYPE_YEAR:
						case MYSQL_TYPE_BIT:
							sq_pushinteger( S, atoi( row[i] ) );
							break;
						case MYSQL_TYPE_NULL:
							sq_pushnull( S );
							break;
						default:
						case MYSQL_TYPE_VARCHAR:
						case MYSQL_TYPE_SET:
						case MYSQL_TYPE_VAR_STRING:
						case MYSQL_TYPE_STRING:
						case MYSQL_TYPE_TIMESTAMP:
						case MYSQL_TYPE_DATE:
						case MYSQL_TYPE_TIME:
						case MYSQL_TYPE_DATETIME:
						case MYSQL_TYPE_NEWDATE:
						case MYSQL_TYPE_TINY_BLOB:
						case MYSQL_TYPE_MEDIUM_BLOB:
						case MYSQL_TYPE_LONG_BLOB:
						case MYSQL_TYPE_BLOB:
							sq_pushstring( S, row[i], strlen( row[i] ) );
							break;
					}
				}
				else
				{
					sq_pushnull( S );
				}
				sq_rawset( S, -3 );
			}
		}
		else
			sq_pushbool( S, false );
	}
	else
		sq_pushbool( S, false );
	return 1;
}

/* mysql_insert_id( mysql_handler handler );
 *   Returns the last inserted row id
 */
int sq_mysql_insert_id(vm S)
{
	CHECK_PARAMS( 1 );
	CHECK_HANDLER( 1 );

	sq_pushinteger( S, handler->InsertID( ) );
	return 1;
}

/* mysql_affected_rows( mysql_handler handler );
 *   Returns the number of affected rows (SELECT - selected, DELETE - deleted etc.)
 */
int sq_mysql_affected_rows(vm S)
{
	CHECK_PARAMS( 1 );
	CHECK_HANDLER( 1 );

	sq_pushinteger( S, handler->AffectedRows( ) );
	return 1;
}
