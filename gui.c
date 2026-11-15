#define UNICODE
#define _UNICODE

#include "click.h"
#include "lang.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define CLASS_NAME    L"AutoClickerWnd"
#define APP_TITLE     L"Auto Clicker"
#define MUTEX_NAME    L"Local\\AutoClickerMutex"
#define ID_BTN_TOGGLE 1
#define ID_TIMER      1
#define ID_HOTKEY     1
#define ID_CHK_HOLD   2
#define ID_BTN_LANG   3
#define BUF_LEN       16

static HWND g_btn;
static HWND g_label;
static HWND g_edit;
static HWND g_chk;
static HWND g_btn_lang;

static BOOL g_running = FALSE;
static BOOL g_hold_active = FALSE;

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

static void save_settings(void) {
    save_language(g_lang);
    int ms = read_interval();
    if (ms >= MIN_MS) {
        save_interval(ms);
    }
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
        g_hold_active = (SendMessage(g_chk, BM_GETCHECK, 0, 0) == BST_CHECKED);
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        g_btn = CreateWindow(L"BUTTON", tr(S_BTN_START),            //
                             WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, //
                             20, 20, 120, 30,                       //
                             hwnd, (HMENU)ID_BTN_TOGGLE,            //
                             ((LPCREATESTRUCT)lp)->hInstance, NULL);
        g_label = CreateWindow(L"STATIC", tr(S_LABEL_INTERVAL), //
                               WS_CHILD | WS_VISIBLE,           //
                               20, 65, 200, 20,                 //
                               hwnd, NULL,                      //
                               ((LPCREATESTRUCT)lp)->hInstance, NULL);
        if (!RegisterHotKey(hwnd, ID_HOTKEY, 0, VK_F6)) {
            MessageBox(hwnd, tr(S_ERR_HOTKEY_ALREADY_REGISTERED), tr(S_TITLE_ERROR), MB_OK);
        }

        wchar_t buf[BUF_LEN];
        _snwprintf(buf, BUF_LEN, L"%d", load_interval());
        g_edit = CreateWindow(L"EDIT", buf,                                  //
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, //
                              225, 62, 55, 24,                               //
                              hwnd, NULL,                                    //
                              ((LPCREATESTRUCT)lp)->hInstance, NULL);
        if (!g_edit) {
            MessageBox(hwnd, L"Failed to create edit box.", tr(S_TITLE_ERROR), MB_OK);
        }

        g_chk = CreateWindow(L"BUTTON", tr(S_LABEL_HOLD),             //
                             WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, //
                             20, 100, 200, 20,                        //
                             hwnd, (HMENU)ID_CHK_HOLD,                //
                             ((LPCREATESTRUCT)lp)->hInstance, NULL);
        if (!g_chk) {
            MessageBox(hwnd, L"Failed to create checkbox.", tr(S_TITLE_ERROR), MB_OK);
        }

        g_btn_lang = CreateWindow(L"BUTTON", L"English / 日本語",        //
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, //
                                  150, 20, 130, 30,                      //
                                  hwnd, (HMENU)ID_BTN_LANG,              //
                                  ((LPCREATESTRUCT)lp)->hInstance, NULL);

        return 0;
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BTN_TOGGLE) {
            toggle(hwnd);
        }
        if (LOWORD(wp) == ID_BTN_LANG) {
            g_lang = (Lang)((g_lang + 1) % LANG_COUNT);
            apply_language();
        }
        return 0;
    case WM_TIMER:
        if (wp == ID_TIMER) {
            do_click();
        }
        return 0;
    case WM_HOTKEY:
        if (wp == ID_HOTKEY) {
            toggle(hwnd);
        }
        return 0;
    case WM_DESTROY:
        if (g_hold_active) {
            send_mouse(MOUSEEVENTF_LEFTUP);
        }
        KillTimer(hwnd, ID_TIMER);
        UnregisterHotKey(hwnd, ID_HOTKEY);
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
                             320, 200, NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, show);

    MSG m;
    while (GetMessage(&m, NULL, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessage(&m);
    }
    return (int)m.wParam;
}
