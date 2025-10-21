
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "listener.h"
#include "input.h"

typedef struct {
	char conn_code[CONNECTION_CODE_LEN + 1];
    SOCKET socket;
} listener_state;

#define SLEEPTMS		10
#define BACKLOG         10
#define PSTATE(ptr)     ((listener_state*)ptr)

int listener_create(unsigned short port, const char* conn_code, pstate_ptr *p)
{
	WSADATA wsa_data = { 0 };
    SOCKET listener_socket = INVALID_SOCKET;
	struct sockaddr_in server_addr = { 0 };
    u_long mode = 0;

    /* Initialize Winsock */
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf_s(stderr, "WSAStartup failed\n");
        return 1;
    }

    /* Create a socket */
    listener_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener_socket == INVALID_SOCKET) {
        fprintf_s(stderr, "socket failed");
        WSACleanup();
        return 1;
    }
    
	/* Set socket to non-blocking */
    mode = 1; // 1 to enable non-blocking, 0 to disable
    if (ioctlsocket(listener_socket, FIONBIO, &mode) != 0) {
        fprintf(stderr, "ioctlsocket failed with error: %ld\n", WSAGetLastError());
    }

	/* Bind the socket */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(listener_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        fprintf_s(stderr, "bind failed");
        closesocket(listener_socket);
        WSACleanup();
        return 1;
    }

	/* Start listening */
    if (listen(listener_socket, BACKLOG) == SOCKET_ERROR) {
        fprintf_s(stderr, "listen failed");
        closesocket(listener_socket);
        WSACleanup();
        return 1;
    }

    /* Alloc & set state */
    *p = malloc(sizeof(listener_state));
    if (*p == NULL) {
        fprintf_s(stderr, "malloc failed");
        closesocket(listener_socket);
        WSACleanup();
        return 1;
	}
    PSTATE(*p)->socket = listener_socket;
	strncpy_s(PSTATE(*p)->conn_code, sizeof(PSTATE(*p)->conn_code), conn_code, CONNECTION_CODE_LEN);

    printf("Server listening on port %d...\n", port);
    return 0;
 }

int listener_close(pstate_ptr p)
{
    closesocket(PSTATE(p)->socket);
    WSACleanup();

    free(p);
    return 0;
}

static int _send_greeting(SOCKET c_socket)
{
    int bytes_sent = send(c_socket, GREETING_MSG, (int)strlen(GREETING_MSG), 0);
    if (bytes_sent == SOCKET_ERROR) {
        fprintf_s(stderr, "send failed");
        return 1;
    }
    return 0;
}

/**
 * Check recv() return value for errors or disconnect.
 * Returns:
 *   0 - no error
 *   1 - error or disconnect
 */
static int _recv_rv_check(int rv)
{
    if (rv < 0 && WSAGetLastError() != WSAEWOULDBLOCK) {
        fprintf_s(stderr, "recv() failed, err code: %d\n", WSAGetLastError());
        return 1;
    }
    if (rv == 0) {
		printf("Client disconnected.\n");
        return 1;
    }
    return 0;
}

/**
 * Peek and read a full line from socket.
 * Returns:
 *   0 - full line read into recvbuf
 *   1 - error or disconnect
 *  -1 - no full line yet
 */
static int _get_line(SOCKET s, char* recvbuf, const int bufsize)
{
    int rv, nl_idx;

    memset(recvbuf, 0, sizeof(recvbuf));
    rv = recv(s, recvbuf, bufsize - 1, MSG_PEEK);
    if (_recv_rv_check(rv)) {
        return 1;
    }

    nl_idx = index_of(recvbuf, '\n');
    if (nl_idx == -1) {
        /* no full line yet */
        /* ...or client is sending a resp bigger than expected */
        return -1;
    }

    /* full line available, drain it */
    rv = recv(s, recvbuf, nl_idx + 1, 0);
    if (_recv_rv_check(rv)) {
        return 1;
    }

    /* Make a c_string by null terminating it */
    recvbuf[rv] = '\0'; 

    return 0;
}

static int _verify_auth_code(const bool* running, SOCKET c_socket, const pstate_ptr p)
{
    char recvbuf[64] = { 0 };
    int rv;

    while (*running) {
        rv = _get_line(c_socket, recvbuf, sizeof(recvbuf));
        if (rv == 1) return 1;
        else if (rv == 0) break;

        Sleep(SLEEPTMS);
    }
    
    /* Trim off newline */
    str_trim(recvbuf);

    /* Compare connection codes (or whatever client was able to send) */
    if (strlen(recvbuf) != CONNECTION_CODE_LEN) {
        send(c_socket, AUTHFAILED_MSG, (int)strlen(AUTHFAILED_MSG), 0);
        fprintf_s(stderr, "Wrong AuthCode length: '%d', expecting: '%d'\n", 
            (int)strlen(str_trim(recvbuf)), CONNECTION_CODE_LEN);
        return 1;
	}

    if (strncmp(PSTATE(p)->conn_code, recvbuf, CONNECTION_CODE_LEN)) {
        send(c_socket, AUTHFAILED_MSG, (int)strlen(AUTHFAILED_MSG), 0);
        fprintf_s(stderr, "Wrong AuthCode: '%s', expecting: '%s'\n",
            recvbuf, PSTATE(p)->conn_code);
        return 1;
    }

    return 0;
}

static int _process_messages(const bool* running, SOCKET c_socket)
{
    char recvbuf[1024] = { 0 };
    int rv;

    while (*running) {
        Sleep(SLEEPTMS);
        rv = _get_line(c_socket, recvbuf, sizeof(recvbuf));
        if (rv == 0) {
			printf("Received message: %s", recvbuf);
			handle_message(recvbuf);
        }
        else if (rv == 1) return 1;
    }

    return 0;
}

/* Accept clients, but only need to handle one at a time... */
int listener_run(const bool* running, const pstate_ptr p)
{
    SOCKET c_socket;
    struct sockaddr_in client = { 0 };
    int c_len = sizeof(client);

    while (*running)
    {
        /* Accept a client */
        c_socket = accept(PSTATE(p)->socket, (SOCKADDR*)&client, &c_len);
        if (c_socket == INVALID_SOCKET) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                fprintf_s(stderr, "accept failed, err code: %d\n", WSAGetLastError());
            }
            Sleep(SLEEPTMS);
            continue;
        }
        printf("Connection accepted.\n");

        /* Send greeting message as part of original Java code */
        if (_send_greeting(c_socket)) {
            goto close_socket;
        }

        /* Receive connection auth code and verify */
        if (_verify_auth_code(running, c_socket, p)) {
            goto close_socket;
        }
        printf("Client authenticated successfully.\n");

        /* Handle client messages */
        _process_messages(running, c_socket);

    close_socket:
        /* Close the client socket */
        printf("Closing connection with client: '%s'\n", inet_ntoa(client.sin_addr));
        closesocket(c_socket);
    }
    return 0;
}

