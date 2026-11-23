#define UNICODE
#define _UNICODE

#include "capture.h"
#include "click.h"
#include "clicker.h"
#include "hotkey.h"
#include "lang.h"
#include "settings.h"
#include "ui.h"
#include <dwmapi.h>
#include <windows.h>

#define CLASS_NAME L"AutoClickerWnd"
#define APP_TITLE  L"Auto Clicker"
#define MUTEX_NAME L"Local\\AutoClickerMutex"
#define ID_TIMER   1

#define WINDOW_BG_COLOR RGB(0xFF, 0xFF, 0xFF)
#define CAPTION_COLOR   RGB(0xE3, 0xE3, 0xE3)

static HBRUSH bg_brush;

static void save_settings(void) {
    save_language(g_lang);
    int ms = ui_read_interval();
    save_interval(ms < MIN_MS ? DEFAULT_MS : ms);
    save_hold(ui_read_hold());
    save_target(ui_read_target());
    save_hotkey(hotkey_get());
}

static void toggle(HWND hwnd) {
    BOOL running = clicker_is_running();
    if (running) {
        switch (clicker_stop()) {
        case CLICKER_STOP_OK_TIMER:
            KillTimer(hwnd, ID_TIMER);
            break;
        case CLICKER_STOP_OK_HOLD:
            break;
        }
    } else {
        int ms = ui_read_interval();
        BOOL hold = ui_read_hold();
        ClickTarget target = ui_read_target();
        switch (clicker_start(ms, hold, target)) {
        case CLICKER_START_OK_TIMER:
            SetTimer(hwnd, ID_TIMER, ms, NULL);
            break;
        case CLICKER_START_OK_HOLD:
            break;
        case CLICKER_START_ERR_INTERVAL:
            MessageBox(hwnd, tr(S_ERR_INTERVAL_TOO_SHORT), tr(S_TITLE_ERROR),
                       MB_OK);
            return;
        }
    }
    ui_set_running(clicker_is_running());
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (capture_is_active()) {
        ClickTarget t;
        if (capture_translate(msg, wp, lp, &t)) {
            capture_end();
            ui_set_target(&t);
            return (msg == WM_XBUTTONDOWN) ? TRUE : 0;
        }
    }
    switch (msg) {
    case WM_CREATE: {
        COLORREF caption = CAPTION_COLOR;
        DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &caption,
                              sizeof(caption));

        UiInit init = {
            .interval = load_interval(),
            .hold = load_hold(),
            .hotkey = load_hotkey(),
            .target = load_target(),
        };
        if (!ui_create(hwnd, &init)) {
            MessageBox(hwnd, tr(S_ERR_UI_CREATE_FAILED), tr(S_TITLE_ERROR),
                       MB_OK);
            return -1;
        }
        if (!hotkey_register(hwnd, init.hotkey)) {
            MessageBox(hwnd, tr(S_ERR_HOTKEY_ALREADY_REGISTERED),
                       tr(S_TITLE_ERROR), MB_OK);
        }
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BTN_TOGGLE) {
            toggle(hwnd);
        }
        if (LOWORD(wp) == ID_BTN_LANG) {
            g_lang = (Lang)((g_lang + 1) % LANG_COUNT);
            ui_apply_language(clicker_is_running());
        }
        if (LOWORD(wp) == ID_BTN_HOTKEY_APPLY) {
            if (!hotkey_register(hwnd, ui_read_hotkey())) {
                MessageBox(hwnd, tr(S_ERR_HOTKEY_ALREADY_REGISTERED),
                           tr(S_TITLE_ERROR), MB_OK);
            }
        }
        if (LOWORD(wp) == ID_BTN_CAPTURE) {
            capture_begin(hwnd);
            ui_set_capturing(TRUE);
        }
        return 0;
    case WM_TIMER:
        if (wp == ID_TIMER) {
            clicker_tick();
        }
        return 0;
    case WM_HOTKEY:
        if (wp == HOTKEY_ID_TOGGLE) {
            toggle(hwnd);
        }
        return 0;
    case WM_ACTIVATE:
        if (LOWORD(wp) == WA_INACTIVE && capture_is_active()) {
            capture_end();
            ui_set_capturing(FALSE);
        }
        break;
    case WM_DESTROY:
        clicker_stop();
        KillTimer(hwnd, ID_TIMER);
        UnregisterHotKey(hwnd, HOTKEY_ID_TOGGLE);
        if (ui_is_ready()) {
            save_settings();
        }
        ui_destroy();
        PostQuitMessage(0);
        return 0;
    case WM_CTLCOLORSTATIC:
        HDC hdc = (HDC)wp;
        SetBkColor(hdc, WINDOW_BG_COLOR);
        return (LRESULT)bg_brush;
    case WM_CTLCOLORBTN:
        return (LRESULT)bg_brush;
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

    bg_brush = CreateSolidBrush(WINDOW_BG_COLOR);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = bg_brush;
    RegisterClass(&wc);

    UINT dpi = GetDpiForSystem();

    HWND hwnd =
        CreateWindow(CLASS_NAME, APP_TITLE,                                  //
                     WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, //
                     CW_USEDEFAULT, CW_USEDEFAULT,                           //
                     MulDiv(320, dpi, 96), MulDiv(300, dpi, 96),             //
                     NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, show);

    MSG m;
    while (GetMessage(&m, NULL, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessage(&m);
    }
    return (int)m.wParam;
}
