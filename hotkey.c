#define UNICODE
#define _UNICODE

#include "hotkey.h"
#include <windows.h>

static Hotkey current = {0, VK_F6};

BOOL hotkey_register(HWND hwnd, Hotkey hk) {
    UnregisterHotKey(hwnd, HOTKEY_ID_TOGGLE);

    if (RegisterHotKey(hwnd, HOTKEY_ID_TOGGLE, hk.mod | MOD_NOREPEAT, hk.vk)) {
        current = hk;
        return TRUE;
    }

    RegisterHotKey(hwnd, HOTKEY_ID_TOGGLE, current.mod | MOD_NOREPEAT,
                   current.vk);
    return FALSE;
}

Hotkey hotkey_get(void) {
    return current;
}
