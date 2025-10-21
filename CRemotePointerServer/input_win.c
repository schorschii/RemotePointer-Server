/***input_win.c
* Windows-specific input handling implementation
* 
* Created: 2025-10-20
*/
#include <Windows.h>
#include <stdio.h>
#include <stdbool.h>

#include "input.h"

static int _send_key(const int vk_code, bool down)
{
    INPUT in = { 0 };
    in.type = INPUT_KEYBOARD;

    // Map VK -> scan code
    UINT sc = MapVirtualKey(vk_code, MAPVK_VK_TO_VSC);
    in.ki.wVk = 0;                 // When using SCANCODE, wVk should be 0
    in.ki.wScan = sc;
    in.ki.dwFlags = KEYEVENTF_SCANCODE | (down ? 0 : KEYEVENTF_KEYUP);

    // Extended-key fixups (not needed for TAB, but good to have)
    // e.g., arrow keys, Insert/Delete/Home/End/PageUp/PageDown, Numpad slash, RAlt, etc.
    switch (vk_code) {
    case VK_LEFT: case VK_RIGHT:
    case VK_UP: case VK_DOWN:
    case VK_PRIOR: case VK_NEXT:     // PageUp/PageDown
    case VK_END: case VK_HOME:
    case VK_INSERT: case VK_DELETE:
    case VK_DIVIDE:                  // Numpad /
    case VK_RMENU:                   // Right Alt
    case VK_RCONTROL:                // Right Ctrl
        in.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        break;
    default: break;
    }

    return SendInput(1, &in, sizeof(INPUT));
}

static int _map_cmn_keycode_to_vk(const int cmn_keycode)
{
    switch (cmn_keycode) {
    case CMN_KEYCODE_PAGE_UP:
        return VK_PRIOR;
    case CMN_KEYCODE_PAGE_DOWN:
        return VK_NEXT;
    case CMN_KEYCODE_PAGE_ENTER:
        return VK_RETURN;
    case CMN_KEYCODE_PAGE_BACK_SPACE:
        return VK_BACK;
    case CMN_KEYCODE_PAGE_ESCAPE:
        return VK_ESCAPE;
    case CMN_KEYCODE_UP:
        return VK_UP;
    case CMN_KEYCODE_DOWN:
        return VK_DOWN;
    case CMN_KEYCODE_LEFT:
        return VK_LEFT;
    case CMN_KEYCODE_RIGHT:
        return VK_RIGHT;
	case CMN_KEYCODE_F1:
		return VK_F1;
    case CMN_KEYCODE_F2:
        return VK_F2;
    case CMN_KEYCODE_F3:
        return VK_F3;
    case CMN_KEYCODE_F4:
        return VK_F4;
    case CMN_KEYCODE_F5:
        return VK_F5;
    case CMN_KEYCODE_F6:
        return VK_F6;
    case CMN_KEYCODE_F7:
        return VK_F7;
    case CMN_KEYCODE_F8:
        return VK_F8;
    case CMN_KEYCODE_F9:
        return VK_F9;
    case CMN_KEYCODE_F10:
        return VK_F10;
    case CMN_KEYCODE_F11:
        return VK_F11;
    case CMN_KEYCODE_F12:
        return VK_F12;
    default:
        return 0;
    }
}

int send_key(const int cmn_keycode)
{
	_send_key(_map_cmn_keycode_to_vk(cmn_keycode), true);
    _send_key(_map_cmn_keycode_to_vk(cmn_keycode), false);

    return 0;
}

int mouse_press(const int cmn_button)
{
    INPUT in= {0};
    in.type = INPUT_MOUSE;
	in.mi.dwFlags = cmn_button == CMN_MOUSE_BUTTON_LEFT
		? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;

    return SendInput(1, &in, sizeof(INPUT));
}
int mouse_release(const int cmn_button)
{
    INPUT in = { 0 };
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = cmn_button == CMN_MOUSE_BUTTON_LEFT
        ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;

    return SendInput(1, &in, sizeof(INPUT));
}

int mouse_move(const int dx, const int dy)
{
    INPUT in = { 0 };
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = MOUSEEVENTF_MOVE;
    in.mi.dx = dx;
    in.mi.dy = dy;

    return SendInput(1, &in, sizeof(INPUT));
}

int mouse_get_position(int* x, int* y)
{
    POINT p;
    if (!GetCursorPos(&p)) {
        return 1;
    }

    *x = p.x;
    *y = p.y;
    return 0;
}

int set_clipboard_text(const char* text) {
    //if (!OpenClipboard(NULL)) {
    //    fprintf(stderr, "OpenClipboard failed\n");
    //    return 0;
    //}

    //if (!EmptyClipboard()) {
    //    fprintf(stderr, "EmptyClipboard failed\n");
    //    CloseClipboard();
    //    return 0;
    //}

    //size_t len = strlen(text) + 1; // include null terminator
    //HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
    //if (!hMem) {
    //    fprintf(stderr, "GlobalAlloc failed\n");
    //    CloseClipboard();
    //    return 0;
    //}

    //// Copy string into the allocated memory
    //memcpy(GlobalLock(hMem), text, len);
    //GlobalUnlock(hMem);

    //// Place it on the clipboard as CF_TEXT (ANSI) or CF_UNICODETEXT if using wide chars
    //if (SetClipboardData(CF_TEXT, hMem) == NULL) {
    //    fprintf(stderr, "SetClipboardData failed\n");
    //    GlobalFree(hMem); // clipboard did not take ownership
    //    CloseClipboard();
    //    return 0;
    //}

    //CloseClipboard();
    return 1;
}

int get_clipboard_text(char** outText)
{
    return 0;
}

