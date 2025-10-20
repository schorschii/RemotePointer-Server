#include <stdio.h>

#include "listener.h"

int main() {
	printf("CRemotePointer-Server started.\n");

	create_listener(5000, "1234");
	// Placeholder for server implementation
	return 0;
}