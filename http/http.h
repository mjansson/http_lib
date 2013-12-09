/* http.h  -  HTTP library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a HTTP communication library built on our foundation and
 * network libraries. The latest source code is always available at
 * 
 * https://github.com/rampantpixels/http_lib
 * 
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 * 
 */

#pragma once

/*! \file http.h
    HTTP methods */

#include <foundation/platform.h>

#include <http/types.h>


//! Initialize library
/*! \return                                             0 if success, -1 if error */
HTTP_API int                                            http_initialize( void );

//! Shutdown library
HTTP_API void                                           http_shutdown( void );


//! GET request
/*! \param url                                          URL
	\param headers                                      Extra headers (use LF for line separator, will be replaced to CRLF when written to socket)
	\param timeout                                      Timeout in milliseconds
	\return                                             Socket, null if error */
HTTP_API object_t                                       http_get( const char* url, const char* headers, unsigned int timeout );

//! POST request
/*! \param url                                          URL
	\param data                                         Data
	\param length                                       Data length
	\param headers                                      Extra headers (use LF for line separator, will be replaced to CRLF when written to socket)
	\param timeout                                      Timeout in milliseconds
	\return                                             Socket, null if error */
HTTP_API object_t                                       http_post( const char* url, const void* data, unsigned int length, const char* headers, unsigned int timeout );

//! Parse reply headers
/*! Parse headers in HTTP reply, as status code and a key-value map
	\param sock                                         Socket
	\param headers                                      Pointer to array receiving headers as string pairs (key,value,key,value,...) if not null
	\return                                             Status code */
HTTP_API http_status_t                                  http_parse_reply( object_t sock, char*** headers );

