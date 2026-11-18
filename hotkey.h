#ifndef HOTKEY_H
#define HOTKEY_H

#include <windows.h>

#define HOTKEY_ID_TOGGLE 1

BOOL hotkey_register(HWND hwnd, UINT mod, UINT vk);
UINT hotkey_get_mod(void);
UINT hotkey_get_vk(void);
UINT hotkeyf_to_mod(BYTE hotkeyf);
BYTE mod_to_hotkeyf(UINT mod);

#endif
