
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "listener.h"

typedef struct {
	char conn_code[CONNECTION_CODE_LEN + 1];
    SOCKET socket;
} listener_state;

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

static int send_greeting(SOCKET c_socket)
{
    int bytes_sent = send(c_socket, GREETING_MSG, (int)strlen(GREETING_MSG), 0);
    if (bytes_sent == SOCKET_ERROR) {
        fprintf_s(stderr, "send failed");
        return 1;
    }
    return 0;
}

static int verify_auth_code(const bool* running, SOCKET c_socket, const pstate_ptr p)
{
    char recvbuf[64] = { 0 };
    int rv;

    while (*running) {
        rv = recv(c_socket, recvbuf, sizeof(recvbuf) - 1, MSG_PEEK);
        
        if (rv < 0 && WSAGetLastError() != WSAEWOULDBLOCK) {
            fprintf_s(stderr, "recv() failed, err code: %d\n", WSAGetLastError());
            return 1;
        }
		if (rv == 0) {
			return 1; // graceful disconnect
        }

		if(index_of(recvbuf, '\n') == -1) {
			// not full line yet (or client is sending a big bad response)
            Sleep(1);
            continue;
        }

		// full line available, drain it... technically should only read up to \n...
        recv(c_socket, recvbuf, sizeof(recvbuf) - 1, 0);
        break;
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

static int process_messages(const bool* running, SOCKET c_socket)
{
    char recvbuf[128] = { 0 };
    int bytes_received = 0;
    while (*running)
    {
        int retval = recv(c_socket, recvbuf, sizeof(recvbuf) - 1, 0);
        if (retval > 0) {
            bytes_received += retval;


            if (bytes_received > 0) {
                recvbuf[retval] = '\0'; // Null-terminate the received data
                printf("recieved: % s\n", recvbuf);
                bytes_received = 0; // Reset for next message
			}

        }
        else if (retval == 0) {
            // Connection closed
            printf("connection closed\n");
            // TODO:
            return 1;
        }
        else {
            // TODO:
           // fprintf_s(stderr, "recv failed");
          //  return 1;
        }
    }

    return 0;
}

/* Accept clients, but only need to handle one at a time... */
int listener_run(const bool* running, const pstate_ptr p)
{
    SOCKET c_socket;
    struct sockaddr_in client = {0};
    int clientLen = sizeof(client);
 
    while (*running)
    {
		/* Accept a client */
        c_socket = accept(PSTATE(p)->socket, (SOCKADDR*)&client, & clientLen);
        if (c_socket == INVALID_SOCKET) {
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                fprintf_s(stderr, "accept failed, err code: %d\n", WSAGetLastError());
			}
            Sleep(10);
            continue;
        }
        printf("Connection accepted.\n");

        /* Send greeting message as part of original Java code */
        if (send_greeting(c_socket)) {
            goto close_socket;
        }

        /* Receive connection auth code and verify */
        if (verify_auth_code(running, c_socket, p)) {
            goto close_socket;
        }
		printf("Client authenticated successfully.\n");

        /* Handle client messages */
        process_messages(running, c_socket);

    close_socket:
        /* Close the client socket */
        printf("Closing connection with client: '%s'\n", inet_ntoa(client.sin_addr));
        closesocket(c_socket);
    }
    return 0;
}


static void handleMessage(const char* s) { }


/*

    // interprets a message from the client app
    private static void handleMessage(String s) throws IOException {
        if(s.equals("START"))          { update = true; fp.setVisible(true); }
        else if(s.equals("STOP"))      { update = false; fp.setVisible(false); }
        else if(s.equals("PREV"))      { sendKey(KeyEvent.VK_PAGE_UP); }
        else if(s.equals("NEXT"))      { sendKey(KeyEvent.VK_PAGE_DOWN); }
        else if(s.equals("RETURN"))    { sendKey(KeyEvent.VK_ENTER); }
        else if(s.equals("BACKSPACE")) { sendKey(KeyEvent.VK_BACK_SPACE); }
        else if(s.equals("ESCAPE"))    { sendKey(KeyEvent.VK_ESCAPE); }
        else if(s.equals("MDOWN"))     { mRobot.mousePress(InputEvent.BUTTON1_MASK); }
        else if(s.equals("MUP"))       { mRobot.mouseRelease(InputEvent.BUTTON1_MASK); }
        else if(s.equals("MLEFT"))     { mRobot.mousePress(InputEvent.BUTTON1_MASK); mRobot.mouseRelease(InputEvent.BUTTON1_MASK); }
        else if(s.equals("MRIGHT"))    { mRobot.mousePress(InputEvent.BUTTON3_MASK); mRobot.mouseRelease(InputEvent.BUTTON3_MASK); }
        else if(s.equals("UP"))        { sendKey(KeyEvent.VK_UP); }
        else if(s.equals("DOWN"))      { sendKey(KeyEvent.VK_DOWN); }
        else if(s.equals("LEFT"))      { sendKey(KeyEvent.VK_LEFT); }
        else if(s.equals("RIGHT"))     { sendKey(KeyEvent.VK_RIGHT); }
        else if(s.equals("F1"))        { sendKey(KeyEvent.VK_F1); }
        else if(s.equals("F2"))        { sendKey(KeyEvent.VK_F2); }
        else if(s.equals("F3"))        { sendKey(KeyEvent.VK_F3); }
        else if(s.equals("F4"))        { sendKey(KeyEvent.VK_F4); }
        else if(s.equals("F5"))        { sendKey(KeyEvent.VK_F5); }
        else if(s.equals("F6"))        { sendKey(KeyEvent.VK_F6); }
        else if(s.equals("F7"))        { sendKey(KeyEvent.VK_F7); }
        else if(s.equals("F8"))        { sendKey(KeyEvent.VK_F8); }
        else if(s.equals("F9"))        { sendKey(KeyEvent.VK_F9); }
        else if(s.equals("F10"))       { sendKey(KeyEvent.VK_F10); }
        else if(s.equals("F11"))       { sendKey(KeyEvent.VK_F11); }
        else if(s.equals("F12"))       { sendKey(KeyEvent.VK_F12); }
        else if(s.startsWith("TEXT|")) {
            // get text from app
            String remotePointerText = s.substring(5, s.length());

            // get previous clipboard content
            Clipboard clipboard = Toolkit.getDefaultToolkit().getSystemClipboard();
            String prevClipboardText = null;
            Transferable t = clipboard.getContents( null );
            if( t.isDataFlavorSupported(DataFlavor.stringFlavor) ) {
                try {
                    prevClipboardText = (String)t.getTransferData( DataFlavor.stringFlavor );
                    System.out.println( "Saving prev clipboard contents: " + prevClipboardText );
                } catch (UnsupportedFlavorException ex) {
                    System.out.println( "Unable to get clipboard content for temp saving: " + ex.getMessage() );
                }
            }

            // set app text to clipboard
            clipboard.setContents(new StringSelection(remotePointerText), new StringSelection(remotePointerText));

            // paste from clipboard
            if(System.getProperty("os.name").toLowerCase().contains("mac")) {
                robot.keyPress(KeyEvent.VK_META); robot.keyPress(KeyEvent.VK_V);
                robot.keyRelease(KeyEvent.VK_V); robot.keyRelease(KeyEvent.VK_META);
            } else {
                robot.keyPress(KeyEvent.VK_CONTROL); robot.keyPress(KeyEvent.VK_V);
                robot.keyRelease(KeyEvent.VK_V); robot.keyRelease(KeyEvent.VK_CONTROL);
            }

            // restore clipboard content
            if(prevClipboardText != null) {
                clipboard.setContents(new StringSelection(prevClipboardText), new StringSelection(prevClipboardText));
            }
        }

        else {
            String[] parts = s.split("\\|");
            if(update && parts[0].equals("S")) {
                float x = Float.parseFloat(parts[1]);
                float y = Float.parseFloat(parts[2]);
                fp.update(x*rs.pointerSpeed, y*rs.pointerSpeed);
            }
            else if(parts[0].equals("M")) {
                double old_x = MouseInfo.getPointerInfo().getLocation().getX();
                double old_y = MouseInfo.getPointerInfo().getLocation().getY();
                int x = (int)((Integer.parseInt(parts[1])*rs.mouseSpeed)+old_x);
                int y = (int)((Integer.parseInt(parts[2])*rs.mouseSpeed)+old_y);
                if(x<0) x = 0;
                if(y<0) y = 0;
                mRobot.mouseMove(x, y);
            }
        }
    }
}

*/