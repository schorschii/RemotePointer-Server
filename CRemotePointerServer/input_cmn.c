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

int handle_message(const char* s)
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
		size_t count = 0;
        char** parts = NULL;

		printf("Received unknown message: %s", s);
        return 0;

		//parts = split_string(s, '|', &count);

        if (update && parts[0][0] == 'S') {
           // float x = (float)atof(parts[1]);
           // float y = (float)atof(parts[2]);
            // fp_update(x * rs.pointerSpeed, y * rs.pointerSpeed);
        }
        else if (parts[0][0] == 'M') {
            int x = 0, y = 0;
            // double old_x = MouseInfo.getPointerInfo().getLocation().getX();
            // double old_y = MouseInfo.getPointerInfo().getLocation().getY();
           // int x = (int)(atoi(parts[1])  * rs.mouseSpeed  + old_x );
           // int y = (int)(atoi(parts[2])  * rs.mouseSpeed  + old_y );
            if (x < 0) x = 0;
            if (y < 0) y = 0;
            mouse_move(x, y);

            free_split(parts, count);
        }
    }

    return 0;
}

char** split_string(const char* str, char delimiter, size_t* out_count)
{
    if (!str) return NULL;

    size_t count = 1;
    for (const char* p = str; *p; p++) {
        if (*p == delimiter) count++;
    }

    char** result = malloc((count + 1) * sizeof(char*));
    if (!result) return NULL;

    size_t idx = 0;
    const char* start = str;
    for (const char* p = str; ; p++) {
        if (*p == delimiter || *p == '\0') {
            size_t len = p - start;
            char* token = malloc(len + 1);
            if (token == NULL) {
                // Free previously allocated tokens
                for (size_t j = 0; j < idx; j++)
                    free(result[j]);
                free(result);
                return NULL;
			}
            memcpy(token, start, len);
            token[len] = '\0';
            result[idx++] = token;

            if (*p == '\0') break;
            start = p + 1;
        }
    }


    // Set the extra slot to NULL for safety (optional, but good practice)
    result[idx] = NULL;

    if (out_count) *out_count = count;
    return result;
}

void free_split(char** tokens, size_t count)
{
    for (size_t i = 0; i < count; i++)
        free(tokens[i]);
    free((void*)tokens);
}