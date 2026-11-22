#ifndef SETTINGS_H
#define SETTINGS_H

#include "click.h"
#include "hotkey.h"
#include "lang.h"
#include <windows.h>

#define DEFAULT_MS            1000
#define MIN_MS                100
#define DEFAULT_HOLD          0
#define DEFAULT_HOTKEY_VK     VK_F6
#define DEFAULT_HOTKEY_MOD    0
#define DEFAULT_TARGET_KIND   CLICK_TARGET_MOUSE
#define DEFAULT_TARGET_BUTTON MOUSE_BTN_LEFT
#define DEFAULT_TARGET_VK     0

BOOL load_language(Lang *out);
void save_language(Lang lang);
int load_interval(void);
void save_interval(int ms);
BOOL load_hold(void);
void save_hold(BOOL hold);
Hotkey load_hotkey(void);
void save_hotkey(Hotkey hk);
ClickTarget load_target(void);
void save_target(ClickTarget target);

#endif
