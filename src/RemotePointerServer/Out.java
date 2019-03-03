/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package RemotePointerServer;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;

/**
 *
 * @author georg
 */
public class Out {
    private PrintWriter out;

    public Out(Socket socket) {
        try {
            out = new PrintWriter(socket.getOutputStream(), true);
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }

    public void close() {
        out.close();
    }

    public void println(Object x) {
        out.println(x);
        out.flush();
    }
}
