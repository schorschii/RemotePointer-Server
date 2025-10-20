#include <Windows.h>
#include <stdio.h>
#include <process.h>  // For _beginthreadex and _endthreadex

#include "listener.h"


BOOL running = TRUE;

unsigned int __stdcall ThreadFunction(void* pData) {
    pstate_ptr data = (pstate_ptr)pData;

    printf("Thread started\n");

	listener_run(&running, &data);


	_endthreadex(0);
    return 0;
}


int main() {
	HANDLE h_thread;
	unsigned int threadID;

	pstate_ptr pstate = NULL;

	printf("CRemotePointer-Server started.\n");

	listener_create(4444, "1234", &pstate);
	printf("Listener created.\n");


	/* Run server loop */
	h_thread = (HANDLE)_beginthreadex(
		NULL,               // default security attributes
		0,                  // default stack size
		ThreadFunction,    // thread function
		pstate,               // argument to thread function
		0,                  // default creation flags
		&threadID);        // receive thread identifier
	if (h_thread == NULL) {
		fprintf(stderr, "Error creating thread.\n");
		return 1;
	}

	// Wait for user input to stop the server
	getc(stdin);
	running = FALSE;
	printf("Stopping server...\n");
	WaitForSingleObject(h_thread, INFINITE);
	CloseHandle(h_thread);



	listener_close(&pstate);
	printf("Listener closed.\n");

	// Placeholder for server implementation
	return 0;
}