

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <unistd.h>
//#include <arpa/inet.h>
//#include <sys/socket.h>

#include "listener.h"



void create_listener(int controlPort, const char* connectionCode) {

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