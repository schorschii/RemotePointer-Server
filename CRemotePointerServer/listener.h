/***listener.h
* 
* Created: 2025-10-20
*/
#pragma once

#include <stdbool.h>

#include "runtime_settings.h"

#define GREETING_MSG			("HELLO!\n")
#define AUTHFAILED_MSG			("AUTHFAILED\n")

typedef struct {
	runtime_settings settings;
	union {
		int dummy;
		long big_dummy;
	} socket_fd;
	unsigned short port;
	bool running;
} listener_state;

int listener_create(listener_state *pstate);
int listener_run(const listener_state *pstate);
int listener_close(const listener_state *pstate);

/* Helper functions */
char* str_trim(char* str);
int index_of(const char* str, char ch);