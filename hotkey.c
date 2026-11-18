#define UNICODE
#define _UNICODE

#include "hotkey.h"
#include <windows.h>
#include <commctrl.h>

static UINT hotkey_mod = 0;
static UINT hotkey_vk = VK_F6;

BOOL hotkey_register(HWND hwnd, UINT mod, UINT vk) {
    UnregisterHotKey(hwnd, HOTKEY_ID_TOGGLE);

    if (RegisterHotKey(hwnd, HOTKEY_ID_TOGGLE, mod | MOD_NOREPEAT, vk)) {
        hotkey_mod = mod;
        hotkey_vk = vk;
        return TRUE;
    }

    RegisterHotKey(hwnd, HOTKEY_ID_TOGGLE, hotkey_mod | MOD_NOREPEAT, hotkey_vk);
    return FALSE;
}

UINT hotkey_get_mod(void) {
    return hotkey_mod;
}

UINT hotkey_get_vk(void) {
    return hotkey_vk;
}

UINT hotkeyf_to_mod(BYTE hotkeyf) {
    UINT mod = 0;

    if (hotkeyf & HOTKEYF_SHIFT) {
        mod |= MOD_SHIFT;
    }
    if (hotkeyf & HOTKEYF_CONTROL) {
        mod |= MOD_CONTROL;
    }
    if (hotkeyf & HOTKEYF_ALT) {
        mod |= MOD_ALT;
    }

    return mod;
}

BYTE mod_to_hotkeyf(UINT mod) {
    BYTE hotkeyf = 0;

    if (mod & MOD_SHIFT) {
        hotkeyf |= HOTKEYF_SHIFT;
    }
    if (mod & MOD_CONTROL) {
        hotkeyf |= HOTKEYF_CONTROL;
    }
    if (mod & MOD_ALT) {
        hotkeyf |= HOTKEYF_ALT;
    }

    return hotkeyf;
}
