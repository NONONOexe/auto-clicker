#include "click.h"

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
