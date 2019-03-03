/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package RemotePointerServer;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Scanner;

/**
 *
 * @author georg
 */
public final class In {
    private Scanner scanner;

    public In(java.net.Socket socket) {
        try {
            InputStream is = socket.getInputStream();
            scanner = new Scanner(new BufferedInputStream(is), "UTF-8");
        } catch (IOException ioe) {
            System.err.println("Could not open " + socket);
        }
    }

    public String readLine() {
        String line;
        try {
            line = scanner.nextLine();
        } catch (Exception e) {
            line = null;
        }
        return line;
    }

    public void close() {
        scanner.close();
    }
}
