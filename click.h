#ifndef CLICK_H
#define CLICK_H

#include <windows.h>

typedef enum {
    CLICK_TARGET_MOUSE,
    CLICK_TARGET_KEY,
} ClickTargetKind;

typedef enum {
    MOUSE_BTN_LEFT,
    MOUSE_BTN_RIGHT,
    MOUSE_BTN_MIDDLE,
    MOUSE_BTN_X1,
    MOUSE_BTN_X2,
} MouseButton;

typedef struct {
    ClickTargetKind kind;
    MouseButton button;
    WORD vk;
} ClickTarget;

void send_mouse(DWORD flags);
void do_click(void);
void click_target_down(const ClickTarget *target);
void click_target_up(const ClickTarget *target);
void click_target_click(const ClickTarget *target);

#endif
