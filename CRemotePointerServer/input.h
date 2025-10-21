/***input.h
* Common input handling definitions
* 
* Created: 2025-10-20
*/
#pragma once
#include "runtime_settings.h"

#define CMN_KEYCODE_NONE				(0)
#define CMN_KEYCODE_PAGE_UP				(1)
#define CMN_KEYCODE_PAGE_DOWN			(2)
#define CMN_KEYCODE_PAGE_ENTER			(3)
#define CMN_KEYCODE_PAGE_BACK_SPACE     (4)
#define CMN_KEYCODE_PAGE_ESCAPE			(5)
#define CMN_KEYCODE_UP					(6)
#define CMN_KEYCODE_DOWN				(7)
#define CMN_KEYCODE_LEFT				(8)
#define CMN_KEYCODE_RIGHT				(9)
#define CMN_KEYCODE_F1					(10)
#define CMN_KEYCODE_F2					(11)
#define CMN_KEYCODE_F3					(12)
#define CMN_KEYCODE_F4					(13)
#define CMN_KEYCODE_F5					(14)
#define CMN_KEYCODE_F6					(15)
#define CMN_KEYCODE_F7					(16)
#define CMN_KEYCODE_F8					(17)
#define CMN_KEYCODE_F9					(18)
#define CMN_KEYCODE_F10					(19)
#define CMN_KEYCODE_F11					(20)
#define	CMN_KEYCODE_F12					(21)

#define CMN_MOUSE_BUTTON_LEFT			(1)
#define CMN_MOUSE_BUTTON_RIGHT			(2)


int send_key(const int cmn_keycode);
int mouse_press(const int cmn_button);
int mouse_release(const int cmn_button);
int mouse_move(const int dx, const int dy);

/***
* Get current mouse cursor position
*
* Parameters:
*  int* x - pointer to store X coordinate
*  int* y - pointer to store Y coordinate
*
* Returns:
* 0 on success, non-zero on failure
*/
int mouse_get_position(int* x, int* y);

int set_clipboard_text(const char* text);
int get_clipboard_text(char** outText);

/* Message to input handler dispatcher */
int handle_message(const char* msg, const runtime_settings *settings);
