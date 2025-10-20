#pragma once

#define pstate_ptr  void*

int listener_create(unsigned short port, const char* connectionCode, pstate_ptr pstate);
int listener_run(BOOL* running, pstate_ptr pstate);
int listener_close(pstate_ptr pstate);

