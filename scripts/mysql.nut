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
				local row = mysql_fetch_assoc( result );
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
				local rows = mysql_affected_rows( handler );
				mysql_free_result( result );
				return rows;
			}
			else
				error( );
		}
		return false;
	}
	
	// SQL stuff
	function column_string( column )
	{
		local str = "`" + escape( column.name ) + "` " + escape( column.type );
		if( !column.rawin( "allownull" ) || !column.allownull )
			str += " NOT NULL";
		if( column.rawin( "defaultv" ) && column.defaultv != null )
			str += " DEFAULT " + ( column.defaultv == "CURRENT_TIMESTAMP" ? "CURRENT_TIMESTAMP" : ( "'" + escape( column.defaultv.tostring( ) ) + "'" ) );
		if( column.rawin( "auto_increment" ) && column.auto_increment )
			str += " AUTO_INCREMENT";
		return str;
	}
	
	function create_table( name, columns )
	{
		if( connected( ) )
		{
			local function concat( table, sep )
			{
				if( table.len( ) == 0 )
					return "";
				
				local str = table[0];
				for( local i = 1; i < table.len( ); i ++ )
					str += sep + table[i];
				return str;
			}
			
			if( !sql.query_assoc_single( "SHOW TABLES LIKE '" + escape( name ) + "'" ) )
			{
				// table doesn't exist, create it
				local cols = [ ];
				local keys = [ ];
				local autoIncrementValue = "";
				
				foreach( column in columns )
				{
					if( column.rawin( "primary_key" ) )
						keys.push( "`" + escape( column.name ) + "`" );
					
					if( column.rawin( "auto_increment" ) && typeof( column.auto_increment ) == "integer" )
						autoIncrementValue = " AUTO_INCREMENT = " + value.auto_increment;
					
					cols.push( column_string( column ) );
				}
				
				if( keys.len( ) >= 1 )
					cols.push( "PRIMARY KEY (" + concat( keys, ", " ) + ")" );
				
				return query( "CREATE TABLE `" + name + "`\n(\n  " + concat( cols, ",\n  " ) + "\n) ENGINE = MyISAM DEFAULT CHARSET = utf8" + autoIncrementValue );
			}
			else
			{
				// make sure all columns exist
				local fields = { };
				local keys = [ ];
				local has_primary_key = false;
				local change_primary_keys = false;
				
				foreach( field in query_assoc( "DESCRIBE `" + escape( name ) + "`" ) )
				{
					fields[ field.Field ] <- { name = field.Field, type = field.Type };
					
					if( field.Null == "YES" )
						fields[ field.Field ].allownull <- true;
					
					if( field.Extra == "auto_increment" )
						fields[ field.Field ].auto_increment <- true;
					
					if( field.rawin( "Default" ) )
						fields[ field.Field ].defaultv <- field.rawget( "Default" );
					
					if( field.Key == "PRI" )
					{
						fields[ field.Field ].primary_key <- true;
						has_primary_key = true;
					}
				}
				
				local insertWhere = "FIRST";
				foreach( column in columns )
				{
					if( !fields.rawin( column.name ) )
					{
						if( !query( "ALTER TABLE `" + escape( name ) + "` ADD " + column_string( column ) + " " + insertWhere ) && false )
						{
							return false;
						}
					}
					else
					{
						// check if it matches our definition
						if( column_string( fields[ column.name ] ) != column_string( column ) )
						{
							if( !query( "ALTER TABLE `" + escape( name ) + "` MODIFY COLUMN " + column_string( column ) ) && false )
							{
								return false;
							}
						}
					}
					
					// verify primary keys
					if( column.rawin( "primary_key" ) )
						keys.push( "`" + escape( column.name ) + "`" );
					if( !fields.rawin( column.name ) )
					{
						if( column.rawin( "primary_key" ) )
							change_primary_keys = true;
					}
					else if( fields[ column.name ].rawin( "primary_key" ) != column.rawin( "primary_key" ) )
						change_primary_keys = true;
					
					insertWhere = "AFTER `" + escape( column.name ) + "`"
				}
				
				// change_primary_keys
				if( change_primary_keys )
				{
					if( has_primary_key )
					{
						if( keys.len( ) == 0 )
							query( "ALTER TABLE `" + escape( name ) + "` DROP PRIMARY KEY" );
						else
							query( "ALTER TABLE `" + escape( name ) + "` DROP PRIMARY KEY, ADD PRIMARY KEY(" + concat( keys, ", " ) + ")" )
					}
					else if( keys.len( ) > 0 ) // New primary key(s), had none
					{
						query( "ALTER TABLE `" + escape( name ) + "` ADD PRIMARY KEY(" + concat( keys, ", " ) + ")" )
					}
				}
				return true;
			}
			return true;
		}
		return false;
	}
}
