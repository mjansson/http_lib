/* http.c  -  HTTP library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a HTTP communication library built on our foundation and
 * network libraries. The latest source code is always available at
 * 
 * https://github.com/rampantpixels/http_lib
 * 
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 * 
 */

#include <http/http.h>

#include <network/network.h>
#include <foundation/foundation.h>


int http_initialize( void )
{
	return network_initialize( 0 );
}


void http_shutdown( void )
{
}


static object_t _http_open( const char* url, const char* headers, const char* cmd, unsigned int timeout )
{
	const char* cleanpath = url;	
	unsigned int port = 80;
	char* address_str = 0;
	char* host = 0;
	char* portstr = 0;
	char* name = 0;
	char* request = 0;
	network_address_t** addresses = 0;
	object_t sock = 0;
	bool connected = false;
	int iaddr, asize;
	stream_t* stream = 0;

	if( ( string_length( cleanpath ) > 7 ) && ( string_equal_substr( cleanpath, "http://", 7 ) ) )
		cleanpath = url + 7;
	
	string_split( cleanpath, "/", &address_str, &name, false );

	//TODO: Handle IPv6 numeric addresses with port, [xx:xx:..:xx]:80
	string_split( address_str ? address_str : "", ":", &host, &portstr, false );
	if( portstr && string_length( portstr ) )
		port = string_to_uint( portstr, false );
	if( !port )
		port = 80;

	addresses = network_address_resolve( address_str );
	if( !addresses )
	{
		log_warnf( HASH_HTTP, WARNING_BAD_DATA, "HTTP %s request failed, unable to resolve host: %s", cmd, host );
		goto exit;
	}

	if( !host )
		request = string_format( "%s /%s HTTP/1.1\n", cmd, name );
	else
		request = string_format( "%s /%s HTTP/1.1\nHost: %s\n", cmd, name, host );
	if( headers && string_length( headers ) )
	{
		request = string_append( request, headers );
		if( headers[ string_length( headers ) - 1 ] != '\n' )
			request = string_append( request, "\n" );
	}
	request = string_append( request, "\n" );

	//Send HTTP request
	sock = tcp_socket_create();
	socket_set_blocking( sock, true );
	
	for( iaddr = 0, asize = array_size( addresses ); iaddr < asize; ++iaddr )
	{
		network_address_t* address = addresses[iaddr];
		
		if( !network_address_ip_port( address ) )
			network_address_ip_set_port( address, port );
		
		if( socket_connect( sock, address, timeout ) )	
		{
			connected = true;
			break;
		}
	}
	
	if( !connected )
	{
		log_warnf( HASH_HTTP, WARNING_BAD_DATA, "HTTP %s request failed, unable to connect to host: %s", cmd, host );
		socket_free( sock );
		sock = 0;
		goto exit;
	}

	log_debugf( HASH_HTTP, "Sending HTTP %s request to host %s: %s /%s HTTP/1.1", cmd, host, cmd, name );

	//According to the HTTP RFC the line separator should be CRLF, not just LF
	request = string_replace( request, "\n", "\r\n", false );
	request = string_replace( request, "\r\r", "\r", true );

	stream = socket_stream( sock );
	if( stream )
	{
		stream_set_binary( stream, false );
		stream_write_string( stream, request );
		stream_flush( stream );
	}
	
	exit:

	for( iaddr = 0, asize = array_size( addresses ); iaddr < asize; ++iaddr )
		memory_deallocate( addresses[iaddr] );
	array_deallocate( addresses );

	string_deallocate( request );
	string_deallocate( host );
	string_deallocate( address_str );
	string_deallocate( name );
	string_deallocate( portstr );

	return sock;
}

	
object_t http_get( const char* url, const char* headers, unsigned int timeout )
{
	return _http_open( url, headers, "GET", timeout );
}


object_t http_post( const char* url, const void* data, unsigned int length, const char* headers, unsigned int timeout )
{
	stream_t* stream;
	char* full_headers = string_format( "Content-Length: %d\n%s", length, headers ? headers : "" );
	object_t sock = _http_open( url, full_headers, "POST", timeout );
	string_deallocate( full_headers );
	if( !sock )
		return 0;

	//Send POST-data
	stream = socket_stream( sock );
	if( stream )
	{
		stream_set_binary( stream, true );
		stream_write( stream, data, length );
		stream_flush( stream );
		stream_set_binary( stream, false );
	}

	return sock;
}


http_status_t http_parse_reply( object_t sock, char*** headers )
{
	http_status_t code = HTTP_UNKNOWN;
	stream_t* stream = socket_stream( sock );
	bool binary;
	bool blocking;
	char* line;
	char** tokens;
	char* key = 0;
	char* value = 0;
	
	if( !stream )
		return code;

	binary = stream_is_binary( stream );
	blocking = socket_blocking( sock );
	if( binary )
		stream_set_binary( stream, false );
	if( !blocking )
		socket_set_blocking( sock, true );

	line = string_strip( stream_read_line( stream, '\n' ), STRING_WHITESPACE );
	tokens = string_explode( line, " ", false );

	if( ( array_size( tokens ) < 2 ) || !string_equal_substr( tokens[0], "HTTP", 4 ) )
		goto exit;

	code = (http_status_t)string_to_uint( tokens[1], false );

	do
	{
		string_deallocate( line );

		line = string_strip( stream_read_line( stream, '\n' ), STRING_WHITESPACE );
		if( !line || !string_length( line ) )
			break;

		if( headers )
		{
			string_split( line, ":", &key, &value, false );

			if( key ) key = string_strip( key, STRING_WHITESPACE );
			if( value ) value = string_strip( value, STRING_WHITESPACE );

			if( !key || !value || !string_length( key ) || !string_length( value ) )
			{
				string_deallocate( key );
				string_deallocate( value );
			}

			array_push( *headers, key );
			array_push( *headers, value );
		}
	} while( true );

	if( binary )
		stream_set_binary( stream, true );
	if( !blocking )
		socket_set_blocking( sock, false );

exit:

	string_deallocate( line );
	string_array_deallocate( tokens );

	return code;
}

