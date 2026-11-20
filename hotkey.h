#ifndef HOTKEY_H
#define HOTKEY_H

#include <windows.h>

#define HOTKEY_ID_TOGGLE 1

typedef struct {
    UINT mod;
    UINT vk;
} Hotkey;

BOOL hotkey_register(HWND hwnd, Hotkey hk);
Hotkey hotkey_get(void);

#endif
