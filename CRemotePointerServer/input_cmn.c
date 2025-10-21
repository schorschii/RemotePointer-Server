/***input_cmn.c
 * Common input handling implementation
 * 
 * Created: 2025-10-20
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#include "input.h"

static bool update = false;

int handle_message(const char* s, const runtime_settings *settings)
{
    if (s == NULL) {
        return -1;
    }
    else if (strcmp(s, "PING\n") == 0) {
        printf("Received PING message.\n");
    }
    else if (strcmp(s, "START\n") == 0) {
        update = true;
		// fp.setVisible(true);
    }
    else if (strcmp(s, "STOP\n") == 0) {
        update = false;
		// fp.setVisible(false);
    }
    else if (strcmp(s, "PREV\n") == 0) {
        send_key(CMN_KEYCODE_PAGE_UP);
    }
    else if (strcmp(s, "NEXT\n") == 0) {
        send_key(CMN_KEYCODE_PAGE_DOWN);
    }
    else if (strcmp(s, "RETURN\n") == 0) {
        send_key(CMN_KEYCODE_PAGE_ENTER);
    }
    else if (strcmp(s, "BACKSPACE\n") == 0) {
        send_key(CMN_KEYCODE_PAGE_BACK_SPACE);
    }
    else if (strcmp(s, "ESCAPE\n") == 0) {
        send_key(CMN_KEYCODE_PAGE_ESCAPE);
    }
    else if (strcmp(s, "MDOWN\n") == 0) {
        mouse_press(CMN_MOUSE_BUTTON_LEFT);
    }
    else if (strcmp(s, "MUP\n") == 0) {
        mouse_release(CMN_MOUSE_BUTTON_LEFT);
    }
    else if (strcmp(s, "MLEFT\n") == 0) {
        mouse_press(CMN_MOUSE_BUTTON_LEFT);
        mouse_release(CMN_MOUSE_BUTTON_LEFT);
    }
    else if (strcmp(s, "MRIGHT\n") == 0) {
        mouse_press(CMN_MOUSE_BUTTON_RIGHT);
        mouse_release(CMN_MOUSE_BUTTON_RIGHT);
    }
    else if (strcmp(s, "UP\n") == 0) {
		send_key(CMN_KEYCODE_UP);
    }
    else if (strcmp(s, "DOWN\n") == 0) {
		send_key(CMN_KEYCODE_DOWN);
    }
    else if (strcmp(s, "LEFT\n") == 0) {
		send_key(CMN_KEYCODE_LEFT);
    }
    else if (strcmp(s, "RIGHT\n") == 0) {
		send_key(CMN_KEYCODE_RIGHT);
    }
    else if (strcmp(s, "F1\n") == 0) {
		send_key(CMN_KEYCODE_F1);
    }
    else if (strcmp(s, "F2\n") == 0) {
		send_key(CMN_KEYCODE_F2);
    }
    else if (strcmp(s, "F3\n") == 0) {
		send_key(CMN_KEYCODE_F3);
    }
    else if (strcmp(s, "F4\n") == 0) {
		send_key(CMN_KEYCODE_F4);
    }
    else if (strcmp(s, "F5\n") == 0) {
		send_key(CMN_KEYCODE_F5);
    }
    else if (strcmp(s, "F6\n") == 0) {
		send_key(CMN_KEYCODE_F6);
    }
    else if (strcmp(s, "F7\n") == 0) {
		send_key(CMN_KEYCODE_F7);
    }
    else if (strcmp(s, "F8\n") == 0) {
		send_key(CMN_KEYCODE_F8);
    }
    else if (strcmp(s, "F9\n") == 0) {
		send_key(CMN_KEYCODE_F9);
    }
    else if (strcmp(s, "F10\n") == 0) {
		send_key(CMN_KEYCODE_F10);
    }
    else if (strcmp(s, "F11\n") == 0) {
		send_key(CMN_KEYCODE_F11);
    }
    else if (strcmp(s, "F12\n") == 0) {
		send_key(CMN_KEYCODE_F12);
    }
    else if (strncmp(s, "TEXT|", 5) == 0) {

		//// NEED PAID VERSION TO TEST THIS FEATURE

  //      /* Get start of text to copy */
  //      const char* remote_pointer_text = s + 5;

  //      /* Get copy of current clipboard and replace with message text */
		//char prev_clipboard_text[1024] = { 0 };
		//get_clipboard_text(&prev_clipboard_text);

  //      set_clipboard_text(remote_pointer_text);

		///* Paste clipboard */

		//// Paste from clipboard implementation goes here
  //      
  //      //    robot.keyPress(KeyEvent.VK_CONTROL); robot.keyPress(KeyEvent.VK_V);
  //      //    robot.keyRelease(KeyEvent.VK_V); robot.keyRelease(KeyEvent.VK_CONTROL);
  //      

  //      /* Restore original clipboard */
		//set_clipboard_text(prev_clipboard_text);
    }
    else {
        if (update && s[0] == 'S') {
            float x, y;
            int rv;
            rv = sscanf(s, "S|%f|%f", &x, &y);

            printf("HERE %s", s); // IDK what triggers this or what it does lol
            // fp_update(x * rs.pointerSpeed, y * rs.pointerSpeed);
        }
        else if (s[0] == 'M') {
            int x, y, mx, my, rv;
			rv = sscanf(s, "M|%d|%d", &x, &y);

            mouse_get_position(&mx, &my);
            x = (int)(x * settings->mouse_speed + mx);
            y = (int)(y * settings->mouse_speed + my);
            if (x < 0) x = 0;
            if (y < 0) y = 0;
			mouse_move(x, y);
        }
        else {
			printf("Ignoring message: '%s'\n", s);
        }
    }

    return 0;
}
