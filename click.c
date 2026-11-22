#include "click.h"

static void fill_input(INPUT *in, const ClickTarget *target, BOOL down) {
    ZeroMemory(in, sizeof(INPUT));

    if (target->kind == CLICK_TARGET_KEY) {
        in->type = INPUT_KEYBOARD;
        in->ki.wVk = target->vk;
        in->ki.dwFlags = down ? 0 : KEYEVENTF_KEYUP;
        return;
    }

    in->type = INPUT_MOUSE;
    switch (target->button) {
    case MOUSE_BTN_LEFT:
        in->mi.dwFlags = down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
        break;
    case MOUSE_BTN_RIGHT:
        in->mi.dwFlags = down ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
        break;
    case MOUSE_BTN_MIDDLE:
        in->mi.dwFlags = down ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
        break;
    case MOUSE_BTN_X1:
    case MOUSE_BTN_X2:
        in->mi.dwFlags = down ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
        in->mi.mouseData = (target->button == MOUSE_BTN_X1) ? XBUTTON1 : XBUTTON2;
        break;
    }
}

void send_mouse(DWORD flags) {
    INPUT in = {0};
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = flags;
    SendInput(1, &in, sizeof(INPUT));
}

void do_click(void) {
    INPUT in[2] = {0};
    in[0].type = INPUT_MOUSE;
    in[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    in[1].type = INPUT_MOUSE;
    in[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(2, in, sizeof(INPUT));
}

void click_target_down(const ClickTarget *target) {
    INPUT in;
    fill_input(&in, target, TRUE);
    SendInput(1, &in, sizeof(INPUT));
}

void click_target_up(const ClickTarget *target) {
    INPUT in;
    fill_input(&in, target, FALSE);
    SendInput(1, &in, sizeof(INPUT));
}

void click_target_click(const ClickTarget *target) {
    INPUT in[2];
    fill_input(&in[0], target, TRUE);
    fill_input(&in[1], target, FALSE);
    SendInput(2, in, sizeof(INPUT));
}
