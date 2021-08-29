/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package RemotePointerServer;

import java.awt.AWTException;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.MenuItem;
import java.awt.MouseInfo;
import java.awt.PopupMenu;
import java.awt.Robot;
import java.awt.SystemTray;
import java.awt.Toolkit;
import java.awt.TrayIcon;
import java.awt.datatransfer.Clipboard;
import java.awt.datatransfer.DataFlavor;
import java.awt.datatransfer.StringSelection;
import java.awt.datatransfer.Transferable;
import java.awt.datatransfer.UnsupportedFlavorException;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Objects;
import java.util.ResourceBundle;
import java.util.TimerTask;
import java.util.Timer;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.prefs.Preferences;
import javax.imageio.ImageIO;
import javax.swing.UIManager;
import javax.swing.UnsupportedLookAndFeelException;

/**
 *
 * @author georg
 */
public class RemoteSpotlight {

    private static void sendAllBroadcasts(String hostname, int port) {
        try {
            for(InetAddress ia : listAllBroadcastAddresses()) {
                try {
                    broadcast("HELLOREMOTEPOINTER|"+hostname, ia, port);
                } catch (IOException ex) {
                    System.err.println(ex.toString());
                }
            }
        } catch (SocketException ex) {
            System.err.println(ex.toString());
        }
    }
    private static void broadcast(
      String broadcastMessage, InetAddress address, int port) throws IOException {
        DatagramSocket socket = new DatagramSocket();
        socket.setBroadcast(true);
 
        byte[] buffer = broadcastMessage.getBytes();
 
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length, address, port);
        socket.send(packet);
        socket.close();
    }
    private static ArrayList<InetAddress> listAllBroadcastAddresses() throws SocketException {
        ArrayList<InetAddress> broadcastList = new ArrayList<>();
        Enumeration<NetworkInterface> interfaces = NetworkInterface.getNetworkInterfaces();
        while (interfaces.hasMoreElements()) {
            NetworkInterface networkInterface = interfaces.nextElement();
            if (networkInterface.isLoopback() || !networkInterface.isUp()) {
                continue;
            }
            networkInterface.getInterfaceAddresses().stream()
                    .map(a -> a.getBroadcast())
                    .filter(Objects::nonNull)
                    .forEach(broadcastList::add);
        }
        return broadcastList;
    }
    private static void setupTrayIcon() {
        ResourceBundle rb = ResourceBundle.getBundle("RemotePointerServer.Bundle");
        if (SystemTray.isSupported()) {
            SystemTray tray = SystemTray.getSystemTray();
            try {
                Image image = ImageIO.read(RemoteSpotlight.class.getResource("res/ic_launcher_round.png"));
                ActionListener listenerExit = (ActionEvent e) -> {
                    System.exit(0);
                };
                ActionListener listenerShow = (ActionEvent e) -> {
                    sw.setVisible(true);
                };
                PopupMenu popup = new PopupMenu();
                MenuItem defaultItem = new MenuItem(rb.getString("TrayMenu.Exit"));
                defaultItem.addActionListener(listenerExit);
                popup.add(defaultItem);
                int trayIconWidth = new TrayIcon(image).getSize().width;
                TrayIcon trayIcon = new TrayIcon(image.getScaledInstance(trayIconWidth, -1, Image.SCALE_SMOOTH), rb.getString("SetupWindow.title"), popup);
                trayIcon.addActionListener(listenerShow);
                try {
                    tray.add(trayIcon);
                } catch (AWTException e) {
                    System.err.println(e);
                }
            } catch (IOException ex) {
                Logger.getLogger(RemoteSpotlight.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    private static FramePointer fp;
    private static RuntimeSettings rs;
    private static SetupWindow sw;
    private static boolean update = false;
    private static Robot robot;
    private static Robot mRobot;
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws IOException, InterruptedException, ClassNotFoundException, InstantiationException, IllegalAccessException, UnsupportedLookAndFeelException, AWTException {
        Preferences prefs = Preferences.userNodeForPackage(RemotePointerServer.RemoteSpotlight.class);
        rs = new RuntimeSettings();
        
        // default values
        int broadcastPort     = 4445;
        int controlPort       = 4444;
        String hostname       = InetAddress.getLocalHost().getHostName();
        String connectionCode = prefs.get("connection_code", rs.generateNewConnectionCode());
        boolean showWindow    = true;

        // parse command line arguments
        for(String arg : args) {
            String[] argSplit = arg.split("=");
            if(arg.startsWith("--broadcastPort=") && argSplit.length > 1) {
                broadcastPort = Integer.parseInt(argSplit[1]);
            }
            if(arg.startsWith("--listenPort=") && argSplit.length > 1) {
                controlPort = Integer.parseInt(argSplit[1]);
            }
            if(arg.startsWith("--hostname=") && argSplit.length > 1) {
                hostname = argSplit[1];
            }
            if(arg.startsWith("--code=")) {
                if(argSplit.length > 1)
                    connectionCode = arg.split("=")[1];
                else
                    connectionCode = "";
            }
            if(arg.startsWith("--hidden")) {
                showWindow = false;
                System.out.println("Start without showing main window: --hidden is set!");
            }
        }
        
        // save new connection code
        prefs.put("connection_code", connectionCode);

        // load settings
        rs.connectionCode = connectionCode;
        rs.pointerStyle = Integer.parseInt(prefs.get("pointer_style", "0"));
        rs.pointerSpeed = Float.parseFloat(prefs.get("pointer_speed", "25"));
        rs.mouseSpeed = Float.parseFloat(prefs.get("mouse_speed", "1"));
        
        // init GUI
        UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        setupTrayIcon();
        fp = new FramePointer(rs);
        sw = new SetupWindow(rs);
        sw.pack();
        sw.setLocationRelativeTo(null);
        sw.setText(hostname, connectionCode);
        if(showWindow) {
            sw.setVisible(true);
        }
        
        // init robot & clipboard
        robot = new Robot();
        mRobot = new Robot();
        robot.setAutoDelay(10);

        // start broadcasting
        final int finalBroadcastPort = broadcastPort;
        final String finalHostname = hostname;
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                sendAllBroadcasts(finalHostname, finalBroadcastPort);
            }
        }, 1000, 1000);
        
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
                Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
                double old_x = MouseInfo.getPointerInfo().getLocation().getX();
                double old_y = MouseInfo.getPointerInfo().getLocation().getY();
                int x = (int)((Integer.parseInt(parts[1])*rs.mouseSpeed)+old_x);
                int y = (int)((Integer.parseInt(parts[2])*rs.mouseSpeed)+old_y);
                if(x<0) x = 0;
                if(y<0) y = 0;
                if(x>screenSize.getWidth()) x = (int)screenSize.getWidth();
                if(y>screenSize.getHeight()) y = (int)screenSize.getHeight();
                mRobot.mouseMove(x, y);
            }
        }
    }
    static void sendKey(int keyCode) {
        robot.keyPress(keyCode);
        robot.keyRelease(keyCode);
    }
    
}
