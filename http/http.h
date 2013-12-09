/* http.h  -  Network library  -  Internal use only  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a network abstraction built on foundation streams.
 *
 * All rights reserved. No part of this library, code or built products may be used without
 * the explicit consent from Rampant Pixels AB
 * 
 */

#pragma once

/*! \file build.h
    HTTP methods */

#include <foundation/platform.h>

#include <network/types.h>


//! GET request
/*! \param url                                          URL
	\param headers                                      Extra headers (use LF for line separator, will be replaced to CRLF when written to socket)
	\param timeout                                      Timeout in milliseconds
	\return                                             Socket, null if error */
NETWORK_API object_t                                    http_get( const char* url, const char* headers, unsigned int timeout );

//! POST request
/*! \param url                                          URL
	\param data                                         Data
	\param length                                       Data length
	\param headers                                      Extra headers (use LF for line separator, will be replaced to CRLF when written to socket)
	\param timeout                                      Timeout in milliseconds
	\return                                             Socket, null if error */
NETWORK_API object_t                                    http_post( const char* url, const void* data, unsigned int length, const char* headers, unsigned int timeout );

//! Parse reply headers
/*! Parse headers in HTTP reply, as status code and a key-value map
	\param sock                                         Socket
	\param headers                                      Pointer to array receiving headers as string pairs (key,value,key,value,...) if not null
	\return                                             Status code */
NETWORK_API http_status_t                               http_parse_reply( object_t sock, char*** headers );

