/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package RemotePointerServer;

import java.util.concurrent.ThreadLocalRandom;

/**
 *
 * @author georg
 */
public class RuntimeSettings {
    /* pointerStyle
    0: red laser pointer
    1: green laser pointer
    2: blue laser pointer
    3: spotlight
    */
    protected int pointerStyle = 0;
    protected float pointerSpeed = (float)25;
    protected float mouseSpeed = (float)1.0;
    protected String connectionCode = "0000";
    protected String generateNewConnectionCode() {
        return Integer.toString(ThreadLocalRandom.current().nextInt(1000, 9999));
    }
}
