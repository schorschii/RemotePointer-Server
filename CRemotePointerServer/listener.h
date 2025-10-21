#pragma once

#include <stdbool.h>

#define GREETING_MSG			("HELLO!\n")
#define CONNECTION_CODE_LEN		(4)
#define AUTHFAILED_MSG			("AUTHFAILED\n")

typedef void* pstate_ptr;

int listener_create(unsigned short port, const char* connectionCode, pstate_ptr *pstate);
int listener_run(const bool* running, const pstate_ptr pstate);
int listener_close(pstate_ptr pstate);

char* str_trim(char* str);
int index_of(const char* str, char ch);