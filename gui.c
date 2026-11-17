#define UNICODE
#define _UNICODE

#include "click.h"
#include "lang.h"
#include "settings.h"
#include <commctrl.h>
#include <stdlib.h>
#include <windows.h>

#define CLASS_NAME          L"AutoClickerWnd"
#define APP_TITLE           L"Auto Clicker"
#define MUTEX_NAME          L"Local\\AutoClickerMutex"
#define ID_BTN_TOGGLE       1
#define ID_TIMER            1
#define ID_HOTKEY_TOGGLE    1
#define ID_CHK_HOLD         2
#define ID_BTN_LANG         3
#define ID_HOTKEY_EDIT      4
#define ID_BTN_HOTKEY_APPLY 5
#define BUF_LEN             16

static HWND g_btn;
static HWND g_label;
static HWND g_edit;
static HWND g_chk;
static HWND g_btn_lang;
static HWND g_label_hotkey;
static HWND g_hotkey_edit;
static HWND g_btn_hotkey_apply;

static BOOL g_running = FALSE;
static BOOL g_hold_active = FALSE;

static UINT g_hotkey_mod = 0; // MOD_ALT / MOD_CONTROL / MOD_SHIFT
static UINT g_hotkey_vk = VK_F6;

static void apply_language() {
    SetWindowText(g_btn, g_running ? tr(S_BTN_STOP) : tr(S_BTN_START));
    SetWindowText(g_label, tr(S_LABEL_INTERVAL));
    SetWindowText(g_chk, tr(S_LABEL_HOLD));
}

static int read_interval(void) {
    wchar_t buf[BUF_LEN] = L"";
    GetWindowText(g_edit, buf, BUF_LEN);
    return _wtoi(buf);
}

static BOOL read_hold(void) { return (SendMessage(g_chk, BM_GETCHECK, 0, 0) == BST_CHECKED); }

static void save_settings(void) {
    save_language(g_lang);
    int ms = read_interval();
    save_interval(ms < MIN_MS ? DEFAULT_MS : ms);
    save_hold(read_hold());
    save_hotkey_vk(g_hotkey_vk);
    save_hotkey_mod(g_hotkey_mod);
}

static void toggle(HWND hwnd) {
    if (g_running) {
        if (g_hold_active) {
            send_mouse(MOUSEEVENTF_LEFTUP);
            g_hold_active = FALSE;
        } else {
            KillTimer(hwnd, ID_TIMER);
        }
        SetWindowText(g_btn, tr(S_BTN_START));
        EnableWindow(g_edit, TRUE);
        EnableWindow(g_chk, TRUE);
    } else {
        g_hold_active = read_hold();
        if (g_hold_active) {
            send_mouse(MOUSEEVENTF_LEFTDOWN);
        } else {
            int ms = read_interval();
            if (ms < MIN_MS) {
                MessageBox(hwnd, tr(S_ERR_INTERVAL_TOO_SHORT), tr(S_TITLE_ERROR), MB_OK);
                return;
            }
            SetTimer(hwnd, ID_TIMER, ms, NULL);
        }
        SetWindowText(g_btn, tr(S_BTN_STOP));
        EnableWindow(g_edit, FALSE);
        EnableWindow(g_chk, FALSE);
    }
    g_running = !g_running;
}

static BOOL register_hotkey(HWND hwnd, UINT mod, UINT vk) {
    UnregisterHotKey(hwnd, ID_HOTKEY_TOGGLE);

    if (RegisterHotKey(hwnd, ID_HOTKEY_TOGGLE, mod | MOD_NOREPEAT, vk)) {
        g_hotkey_mod = mod;
        g_hotkey_vk = vk;
        return TRUE;
    }

    RegisterHotKey(hwnd, ID_HOTKEY_TOGGLE, g_hotkey_mod | MOD_NOREPEAT, g_hotkey_vk);
    return FALSE;
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

static HWND create_child(HWND parent, const wchar_t *cls, const wchar_t *text, DWORD style, int x,
                         int y, int w, int h, int id) {
    return CreateWindow(cls, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, parent,
                        (HMENU)(INT_PTR)id, GetModuleHandle(NULL), NULL);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_HOTKEY_CLASS};
        InitCommonControlsEx(&icc);

        g_btn = create_child(hwnd, L"BUTTON", tr(S_BTN_START), BS_PUSHBUTTON, 20, 20, 120, 30,
                             ID_BTN_TOGGLE);
        g_label = create_child(hwnd, L"STATIC", tr(S_LABEL_INTERVAL), 0, 20, 65, 200, 20, 0);

        wchar_t buf[BUF_LEN];
        _snwprintf(buf, BUF_LEN, L"%d", load_interval());
        g_edit = create_child(hwnd, L"EDIT", buf, WS_BORDER | ES_NUMBER, 225, 62, 55, 24, 0);
        if (!g_edit) {
            MessageBox(hwnd, L"Failed to create edit box.", tr(S_TITLE_ERROR), MB_OK);
        }

        g_chk = create_child(hwnd, L"BUTTON", tr(S_LABEL_HOLD), BS_AUTOCHECKBOX, 20, 100, 200, 20,
                             ID_CHK_HOLD);
        if (!g_chk) {
            MessageBox(hwnd, L"Failed to create checkbox.", tr(S_TITLE_ERROR), MB_OK);
        }
        SendMessage(g_chk, BM_SETCHECK, load_hold() ? BST_CHECKED : BST_UNCHECKED, 0);

        g_btn_lang = create_child(hwnd, L"BUTTON", L"English / 日本語", BS_PUSHBUTTON, 150, 20, 130,
                                  30, ID_BTN_LANG);

        UINT mod = load_hotkey_mod();
        UINT vk = load_hotkey_vk();
        if (!register_hotkey(hwnd, mod, vk)) {
            MessageBox(hwnd, tr(S_ERR_HOTKEY_ALREADY_REGISTERED), tr(S_TITLE_ERROR), MB_OK);
        }

        g_label_hotkey = create_child(hwnd, L"STATIC", tr(S_LABEL_HOTKEY), 0, 20, 135, 200, 20, 0);
        g_hotkey_edit =
            create_child(hwnd, HOTKEY_CLASS, L"", WS_TABSTOP, 20, 155, 200, 30, ID_HOTKEY_EDIT);
        SendMessage(g_hotkey_edit, HKM_SETHOTKEY, MAKEWORD(vk, mod_to_hotkeyf(mod)), 0);
        g_btn_hotkey_apply = create_child(hwnd, L"BUTTON", tr(S_BTN_HOTKEY_APPLY), BS_PUSHBUTTON,
                                          230, 155, 50, 30, ID_BTN_HOTKEY_APPLY);

        return 0;
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BTN_TOGGLE) {
            toggle(hwnd);
        }
        if (LOWORD(wp) == ID_BTN_LANG) {
            g_lang = (Lang)((g_lang + 1) % LANG_COUNT);
            apply_language();
        }
        if (LOWORD(wp) == ID_BTN_HOTKEY_APPLY) {
            LRESULT r = SendMessage(g_hotkey_edit, HKM_GETHOTKEY, 0, 0);
            UINT mod = hotkeyf_to_mod(HIBYTE(r));
            UINT vk = LOBYTE(r);
            if (!register_hotkey(hwnd, mod, vk)) {
                MessageBox(hwnd, tr(S_ERR_HOTKEY_ALREADY_REGISTERED), tr(S_TITLE_ERROR), MB_OK);
            }
        }

        return 0;
    case WM_TIMER:
        if (wp == ID_TIMER) {
            do_click();
        }
        return 0;
    case WM_HOTKEY:
        if (wp == ID_HOTKEY_TOGGLE) {
            toggle(hwnd);
        }
        return 0;
    case WM_DESTROY:
        if (g_hold_active) {
            send_mouse(MOUSEEVENTF_LEFTUP);
        }
        KillTimer(hwnd, ID_TIMER);
        UnregisterHotKey(hwnd, ID_HOTKEY_TOGGLE);
        save_settings();
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show) {
    g_lang = detect_language();
    Lang saved;
    if (load_language(&saved)) {
        g_lang = saved;
    }

    HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);
    if (hMutex != NULL && GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBox(NULL, tr(S_ERR_ALREADY_RUNNING), tr(S_TITLE_ERROR), MB_OK);
        return 0;
    }

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(CLASS_NAME, APP_TITLE,                                  //
                             WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, //
                             CW_USEDEFAULT, CW_USEDEFAULT,                           //
                             320, 240, NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, show);

    MSG m;
    while (GetMessage(&m, NULL, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessage(&m);
    }
    return (int)m.wParam;
}
