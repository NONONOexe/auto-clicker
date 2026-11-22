#define UNICODE
#define _UNICODE

#include "ui.h"
#include "hotkey.h"
#include "lang.h"

#include <commctrl.h>
#include <stdlib.h>
#include <windows.h>

#define BUF_LEN 16

static HWND btn_toggle;
static HWND label_interval;
static HWND edit_interval;
static HWND chk_hold;

static HWND btn_lang;

static HWND label_target;
static HWND btn_capture;

static HWND label_hotkey;
static HWND hotkey_ctrl;
static HWND btn_hotkey_apply;

static ClickTarget current_target;

static HFONT font = NULL;
static UINT dpi = 96;

static BOOL created = FALSE;

static int scale(int px) {
    return MulDiv(px, (int)dpi, 96);
}

static HWND create_child(HWND parent, const wchar_t *cls, const wchar_t *text,
                         DWORD style, int x, int y, int w, int h, int id) {
    return CreateWindow(cls, text, WS_CHILD | WS_VISIBLE | style, scale(x),
                        scale(y), scale(w), scale(h), parent,
                        (HMENU)(INT_PTR)id, GetModuleHandle(NULL), NULL);
}

static HFONT create_ui_font(void) {
    NONCLIENTMETRICSW ncm = {0};
    ncm.cbSize = sizeof(ncm);
    if (!SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0)) {
        return NULL;
    }
    return CreateFontIndirectW(&ncm.lfMessageFont);
}

static BOOL CALLBACK set_font_proc(HWND hwnd, LPARAM lparam) {
    SendMessageW(hwnd, WM_SETFONT, (WPARAM)lparam, TRUE);
    return TRUE;
}

static UINT hotkeyf_to_mod(BYTE hotkeyf) {
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

static BYTE mod_to_hotkeyf(UINT mod) {
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

static void format_target(const ClickTarget *target, wchar_t *buf, int len) {
    if (target->kind == CLICK_TARGET_KEY) {
        UINT sc = MapVirtualKey(target->vk, MAPVK_VK_TO_VSC);
        if (GetKeyNameText((LONG)(sc << 16), buf, len) == 0) {
            _snwprintf(buf, len, L"VK 0x%02X", target->vk);
        }
        return;
    }
    static const int mouse_str[] = {
        S_MOUSE_LEFT, S_MOUSE_RIGHT, S_MOUSE_MIDDLE, S_MOUSE_X1, S_MOUSE_X2,
    };
    wcsncpy(buf, tr(mouse_str[target->button]), len - 1);
    buf[len - 1] = L'\0';
}

static void refresh_target_label(void) {
    wchar_t name[32], text[64];
    format_target(&current_target, name, 32);
    _snwprintf(text, 64, tr(S_LABEL_TARGET_FMT), name);
    SetWindowText(label_target, text);
}

BOOL ui_create(HWND parent, const UiInit *init) {
    dpi = GetDpiForWindow(parent);

    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_HOTKEY_CLASS};
    InitCommonControlsEx(&icc);

    btn_toggle = create_child(parent, L"BUTTON", tr(S_BTN_START), BS_PUSHBUTTON,
                              20, 20, 120, 30, ID_BTN_TOGGLE);
    label_interval = create_child(parent, L"STATIC", tr(S_LABEL_INTERVAL), 0,
                                  20, 65, 200, 20, 0);
    wchar_t buf[BUF_LEN];
    _snwprintf(buf, BUF_LEN, L"%d", init->interval);
    edit_interval = create_child(parent, L"EDIT", buf, WS_BORDER | ES_NUMBER,
                                 225, 62, 55, 24, 0);
    chk_hold = create_child(parent, L"BUTTON", tr(S_LABEL_HOLD),
                            BS_AUTOCHECKBOX, 20, 100, 200, 20, ID_CHK_HOLD);

    btn_lang = create_child(parent, L"BUTTON", L"English / 日本語",
                            BS_PUSHBUTTON, 150, 20, 130, 30, ID_BTN_LANG);

    label_target = create_child(parent, L"STATIC", L"", 0, 20, 136, 130, 20, 0);
    btn_capture =
        create_child(parent, L"BUTTON", tr(S_BTN_CAPTURE), BS_PUSHBUTTON, 155,
                     130, 125, 28, ID_BTN_CAPTURE);

    label_hotkey = create_child(parent, L"STATIC", tr(S_LABEL_HOTKEY), 0, 20,
                                175, 200, 20, 0);
    hotkey_ctrl = create_child(parent, HOTKEY_CLASS, L"", WS_TABSTOP, 20, 195,
                               200, 30, ID_HOTKEY_EDIT);

    btn_hotkey_apply =
        create_child(parent, L"BUTTON", tr(S_BTN_HOTKEY_APPLY), BS_PUSHBUTTON,
                     230, 195, 50, 30, ID_BTN_HOTKEY_APPLY);

    HWND ctrls[] = {btn_toggle,  label_interval,  edit_interval, chk_hold,
                    btn_lang,    label_target,    btn_capture,   label_hotkey,
                    hotkey_ctrl, btn_hotkey_apply};
    for (size_t i = 0; i < sizeof(ctrls) / sizeof(ctrls[0]); i++) {
        if (!ctrls[i]) {
            return FALSE;
        }
    }

    SendMessage(chk_hold, BM_SETCHECK, init->hold ? BST_CHECKED : BST_UNCHECKED,
                0);
    SendMessage(hotkey_ctrl, HKM_SETHOTKEY,
                MAKEWORD(init->hotkey.vk, mod_to_hotkeyf(init->hotkey.mod)), 0);

    font = create_ui_font();
    EnumChildWindows(parent, set_font_proc, (LPARAM)font);

    current_target = init->target;
    refresh_target_label();

    created = TRUE;
    return TRUE;
}

BOOL ui_is_ready(void) {
    return created;
}

int ui_read_interval(void) {
    wchar_t buf[BUF_LEN] = L"";
    GetWindowText(edit_interval, buf, BUF_LEN);
    return _wtoi(buf);
}

BOOL ui_read_hold(void) {
    return (SendMessage(chk_hold, BM_GETCHECK, 0, 0) == BST_CHECKED);
}

Hotkey ui_read_hotkey(void) {
    LRESULT r = SendMessage(hotkey_ctrl, HKM_GETHOTKEY, 0, 0);
    return (Hotkey){.mod = hotkeyf_to_mod(HIBYTE(r)), .vk = LOBYTE(r)};
}

ClickTarget ui_read_target(void) {
    return current_target;
}

void ui_set_running(BOOL running) {
    SetWindowText(btn_toggle, tr(running ? S_BTN_STOP : S_BTN_START));
    EnableWindow(edit_interval, !running);
    EnableWindow(chk_hold, !running);
}

void ui_set_target(const ClickTarget *t) {
    current_target = *t;
    refresh_target_label();
}

void ui_set_capturing(BOOL capturing) {
    if (capturing) {
        SetWindowText(label_target, tr(S_TARGET_WAITING));
    } else {
        refresh_target_label();
    }
}

void ui_apply_language(BOOL running) {
    SetWindowText(btn_toggle, running ? tr(S_BTN_STOP) : tr(S_BTN_START));
    SetWindowText(label_interval, tr(S_LABEL_INTERVAL));
    SetWindowText(chk_hold, tr(S_LABEL_HOLD));

    SetWindowText(btn_capture, tr(S_BTN_CAPTURE));
    refresh_target_label();

    SetWindowText(label_hotkey, tr(S_LABEL_HOTKEY));
    SetWindowText(btn_hotkey_apply, tr(S_BTN_HOTKEY_APPLY));
}

void ui_destroy(void) {
    created = FALSE;
    if (font != NULL) {
        DeleteObject(font);
        font = NULL;
    }
}
