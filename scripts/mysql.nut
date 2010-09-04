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

class mysql
{
	handler = false;
	hostname = "";
	username = "";
	password = "";
	database = "";
	
	constructor( hostname, username, password, database )
	{
		this.hostname = hostname;
		this.username = username;
		this.password = password;
		this.database = database;
		
		// Connect to the database
		connect( );
	}
	
	// Connects to a MySQL Server
	function connect( )
	{
		handler = mysql_connect( hostname, username, password, database );
		return handler != false;
	}
	
	// Checks if the Server is connected
	function connected( )
	{
		if( !handler || !mysql_ping( handler ) )
			return connect( );
		return true;
	}
	
	// disconnects from the MySQL-Server
	function disconnect( )
	{
		if( connected( ) )
		{
			mysql_close( handler );
			handler = false;
		}
	}
	
	// prints the error to the console
	function error( )
	{
		if( mysql_errno( handler ) > 0 )
			log( "MySQL-Error: " + mysql_errno( handler ).tostring( ) + " " + mysql_error( handler ) );
	}
	
	// Escapes a String for safe use with MySQL
	function escape( string )
	{
		if( connected( ) )
			return mysql_escape_string( handler, string );
	}
	
	// Executes a Query
	function query( string )
	{
		if( connected( ) )
		{
			local result = mysql_query( handler, string );
			if( result )
			{
				mysql_free_result( result );
				return true;
			}
			else
				error( );
		}
		return false;
	}
	
	// Executes a query and returns the inserted ID
	function query_insertid( string )
	{
		if( connected )
		{
			local result = mysql_query( handler, string );
			if( result )
			{
				mysql_free_result( result );
				return mysql_insert_id( handler );
			}
			else
				error( );
		}
		return false;
	}
	
	// Returns an array containing a key = value table for all results returned by the query
	function query_assoc( string )
	{
		if( connected( ) )
		{
			local result = mysql_query( handler, string );
			if( result )
			{
				local rows = []
				local row = null
				while( row = mysql_fetch_assoc( result ) )
				{
					rows.push( row )
				}
				mysql_free_result( result )
				
				return rows;
			}
			else
				error( );
		}
		return false;
	}
	
	// Returns a single row for a MySQL Query Result, false on error or if no such result exists
	function query_assoc_single( string )
	{
		if( connected( ) )
		{
			local result = mysql_query( handler, string );
			if( result )
			{
				local row = mysql_fetch_assoc( result ) )
				if( row )
				{
					mysql_free_result( result );
					return row;
				}
				return false;
			}
			else
				error( );
		}
		return false;
	}
	
	// Executes a query and returns the number of affected rows - selected, deleted, modified etc.
	function query_affected_rows( string )
	{
		if( connected( ) )
		{
			local result = mysql_query( handler, string );
			if( result )
			{
				local rows = mysql_affected_rows( result ) )
				mysql_free_result( result );
				return rows;
			}
			else
				error( );
		}
		return false;
	}
}
