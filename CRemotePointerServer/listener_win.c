
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "listener.h"

typedef struct {
    SOCKET socket;
} listener_state;

#define BACKLOG 10

int listener_create(unsigned short port, const char* connectionCode, pstate_ptr *pstate) {
	WSADATA wsa_data = { 0 };
    SOCKET listener_socket = INVALID_SOCKET;
	struct sockaddr_in server_addr = { 0 };
    u_long mode = 1;

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
    *pstate = malloc(sizeof(listener_state));
    if (*pstate == NULL) {
        fprintf_s(stderr, "malloc failed");
        closesocket(listener_socket);
        WSACleanup();
        return 1;
	}
    ((listener_state*)*pstate)->socket = listener_socket;


    printf("Server listening on port %d...\n", port);
    return 0;
 }

int listener_close(pstate_ptr *pstate) {
	listener_state* state = (listener_state*)*pstate;
    closesocket(state->socket);
    WSACleanup();

    free(*pstate);
    return 0;
}

int listener_run(BOOL *running, pstate_ptr *pstate) {
    listener_state* state = (listener_state*)*pstate;

    // Main loop to accept clients
    while (*running) {
        // 5. Accept a connection
        struct sockaddr_in client = { 0 };
        int clientLen = sizeof(client);
        SOCKET clientSock = accept(state->socket, (SOCKADDR*)&client, &clientLen);
        if (clientSock == INVALID_SOCKET) {
           /* fprintf_s(stderr, "accept failed");
            closesocket(clientSock);
            WSACleanup();
            return 1;*/
			Sleep(100);
        }
        else {
            printf("Connection accepted.\n");

            // Close the client socket
            closesocket(clientSock);
        }
    }
}


void handleMessage(const char* s) {
}


/*
 // start server
        ServerSocket serverSocket = new ServerSocket(controlPort, 50, InetAddress.getByAddress(new byte[] {0,0,0,0}));
        System.out.println("Started server on Port " + controlPort + ", Host " + hostname);

        // wait for incoming message
        while (true) {
            // handle incoming messages
            Socket clientSocket = serverSocket.accept();
            clientSocket.setSoTimeout(5000);
            System.out.println("Accepted connection from client: "  + clientSocket.getRemoteSocketAddress() );

            In  in  = new In (clientSocket);
            Out out = new Out(clientSocket);

            // auth check
            out.println("HELLO!");
            String clientCode = in.readLine();
            if(!clientCode.equals(connectionCode)) {
                out.println("AUTHFAILED");
                System.err.println("Wrong AuthCode: '"+clientCode+"'");
            } else {

                // wait for client messages
                String s;
                while ((s = in.readLine()) != null) {
                    //System.out.println( "Received Client message: "  + s );
                    handleMessage(s);
                }

            }

            System.out.println("Closing connection with client: " + clientSocket.getInetAddress());
            out.close();
            in.close();
            clientSocket.close();
        }
    }

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
    static void sendKey(int keyCode) {
        robot.keyPress(keyCode);
        robot.keyRelease(keyCode);
    }

}

*/