#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>

#include "listener.h"


static unsigned int __stdcall ThreadFunction(void* pData)
{
    printf("Thread started\n");
	listener_run((listener_state*)pData);
    return 0;
}

static void generate_connection_code(char* conn_code)
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < CONNECTION_CODE_LEN; i++) {
		char key = (char)(rand() % 10);
		conn_code[i] = '0' + key;
	}
}

int main()
{
	HANDLE h_thread;
	unsigned int threadID;

	listener_state pstate = { 0 };
	pstate.port = 4444;
	pstate.running = TRUE;
	pstate.settings.mouse_speed = 1.0f;
	generate_connection_code(pstate.settings.connection_code);

	printf("CRemotePointer-Server started. Connection Code: %s \n", pstate.settings.connection_code);
	listener_create(&pstate);
	printf("Listener created.\n");

	/* Run server loop */
	h_thread = (HANDLE)_beginthreadex(
		NULL,               // default security attributes
		0,                  // default stack size
		ThreadFunction,    // thread function
		&pstate,               // argument to thread function
		0,                  // default creation flags
		&threadID);        // receive thread identifier
	if (h_thread == NULL) {
		fprintf(stderr, "Error creating thread.\n");
		return 1;
	}

	// Wait for user input to stop the server
	getc(stdin);
	pstate.running = FALSE;
	printf("Stopping server...\n");
	WaitForSingleObject(h_thread, INFINITE);
	CloseHandle(h_thread);

	listener_close(&pstate);
	printf("Listener closed.\n");

	return 0;
}