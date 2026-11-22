#include "capture.h"

static BOOL active = FALSE;

void capture_begin(HWND hwnd) {
    active = TRUE;
    SetFocus(hwnd);
    SetCapture(hwnd);
}

void capture_end(void) {
    if (active) {
        active = FALSE;
        ReleaseCapture();
    }
}

BOOL capture_is_active(void) {
    return active;
}

BOOL capture_translate(UINT msg, WPARAM wp, LPARAM lp, ClickTarget *out) {
    (void)lp;
    *out = (ClickTarget){0};

    switch (msg) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        out->kind = CLICK_TARGET_KEY;
        out->vk = (WORD)wp;
        return TRUE;
    case WM_LBUTTONDOWN:
        out->kind = CLICK_TARGET_MOUSE;
        out->button = MOUSE_BTN_LEFT;
        return TRUE;
    case WM_RBUTTONDOWN:
        out->kind = CLICK_TARGET_MOUSE;
        out->button = MOUSE_BTN_RIGHT;
        return TRUE;
    case WM_MBUTTONDOWN:
        out->kind = CLICK_TARGET_MOUSE;
        out->button = MOUSE_BTN_MIDDLE;
        return TRUE;
    case WM_XBUTTONDOWN:
        out->kind = CLICK_TARGET_MOUSE;
        out->button =
            (GET_XBUTTON_WPARAM(wp) == XBUTTON1) ? MOUSE_BTN_X1 : MOUSE_BTN_X2;
        return TRUE;
    }
    return FALSE;
}
