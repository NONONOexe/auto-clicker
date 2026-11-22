#ifndef UI_H
#define UI_H

#include "click.h"
#include "hotkey.h"
#include <windows.h>

#define ID_BTN_TOGGLE       1
#define ID_CHK_HOLD         2
#define ID_BTN_LANG         3
#define ID_HOTKEY_EDIT      4
#define ID_BTN_HOTKEY_APPLY 5
#define ID_BTN_CAPTURE      6

typedef struct {
    int interval;
    BOOL hold;
    Hotkey hotkey;
    ClickTarget target;
} UiInit;

BOOL ui_create(HWND parent, const UiInit *init);
BOOL ui_is_ready(void);
int ui_read_interval(void);
BOOL ui_read_hold(void);
Hotkey ui_read_hotkey(void);
ClickTarget ui_read_target(void);
void ui_set_running(BOOL running);
void ui_set_target(const ClickTarget *t);
void ui_set_capturing(BOOL capturing);
void ui_apply_language(BOOL running);
void ui_destroy(void);

#endif
