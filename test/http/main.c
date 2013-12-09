/* main.c  -  HTTP library http test  -  Internal use only  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a network abstraction built on foundation streams.
 *
 * All rights reserved. No part of this library, code or built products may be used without
 * the explicit consent from Rampant Pixels AB
 * 
 */

#include <network/network.h>
#include <foundation/foundation.h>
#include <test/test.h>


application_t test_http_application( void )
{
	application_t app = {0};
	app.name = "Network http tests";
	app.short_name = "test_http";
	app.config_dir = "test_http";
	app.flags = APPLICATION_UTILITY;
	return app;
}


memory_system_t test_http_memory_system( void )
{
	return memory_system_malloc();
}


int test_http_initialize( void )
{
	return network_initialize( 32 );
}


void test_http_shutdown( void )
{
	network_shutdown();
}


DECLARE_TEST( http, http )
{
	return 0;
}


void test_http_declare( void )
{
	ADD_TEST( http, http );
}


test_suite_t test_http_suite = {
	test_http_application,
	test_http_memory_system,
	test_http_declare,
	test_http_initialize,
	test_http_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID

int test_http_run( void )
{
	test_suite = test_http_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void )
{
	return test_http_suite;
}

#endif

