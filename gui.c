#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define ID_BTN_TOGGLE 1
#define ID_TIMER      1
#define ID_HOTKEY     1
#define ID_CHK_HOLD   2
#define DEFAULT_MS    1000
#define MIN_MS        100

static HWND g_btn;
static HWND g_edit;
static HWND g_chk;

static BOOL g_running = FALSE;
static BOOL g_hold_active = FALSE;

static void do_click(void) {
    INPUT in[2] = {0};
    in[0].type = INPUT_MOUSE;
    in[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    in[1].type = INPUT_MOUSE;
    in[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(2, in, sizeof(INPUT));
}

static void send_mouse(DWORD flags) {
    INPUT in = {0};
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = flags;
    SendInput(1, &in, sizeof(INPUT));
}

static void toggle(HWND hwnd) {
    if (g_running) {
        if (g_hold_active) {
            send_mouse(MOUSEEVENTF_LEFTUP);
            g_hold_active = FALSE;
        } else {
            KillTimer(hwnd, ID_TIMER);
        }
        SetWindowText(g_btn, "Start (F6)");
        EnableWindow(g_edit, TRUE);
        EnableWindow(g_chk, TRUE);
    } else {
        g_hold_active = (SendMessage(g_chk, BM_GETCHECK, 0, 0) == BST_CHECKED);
        if (g_hold_active) {
            send_mouse(MOUSEEVENTF_LEFTDOWN);
        } else {            
            char buf[16];
            GetWindowText(g_edit, buf, sizeof(buf));
            int ms = atoi(buf);
            if (ms < MIN_MS) {
                MessageBox(hwnd, "Interval is too short.", "Error", MB_OK);
                return;
            }
            SetTimer(hwnd, ID_TIMER, ms, NULL);
        }
        SetWindowText(g_btn, "Stop (F6)");
        EnableWindow(g_edit, FALSE);
        EnableWindow(g_chk, FALSE);
    }
    g_running = !g_running;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        g_btn = CreateWindow("Button", "Start (F6)",                //
                             WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, //
                             20, 20, 160, 30,                       //
                             hwnd, (HMENU)ID_BTN_TOGGLE,            //
                             ((LPCREATESTRUCT)lp)->hInstance, NULL);
        CreateWindow("STATIC", "Interval (ms):", //
                     WS_CHILD | WS_VISIBLE,      //
                     20, 65, 100, 20,            //
                     hwnd, NULL,                 //
                     ((LPCREATESTRUCT)lp)->hInstance, NULL);
        if (!RegisterHotKey(hwnd, ID_HOTKEY, 0, VK_F6)) {
            MessageBox(hwnd, "F6 could not be registered.", "Error", MB_OK);
        }

        char buf[16];
        snprintf(buf, sizeof(buf), "%d", DEFAULT_MS);
        g_edit = CreateWindow("EDIT", buf,                                   //
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER, //
                              125, 62, 55, 24,                               //
                              hwnd, NULL,                                    //
                              ((LPCREATESTRUCT)lp)->hInstance, NULL);
        if (!g_edit) {
            MessageBox(hwnd, "Failed to create edit box.", "Error", MB_OK);
        }

        g_chk = CreateWindow("BUTTON", "Hold (press and keep)",       //
                             WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, //
                             20, 100, 200, 20,                        //
                             hwnd, (HMENU)ID_CHK_HOLD,                //
                             ((LPCREATESTRUCT)lp)->hInstance, NULL);
        if (!g_chk) {
            MessageBox(hwnd, "Failed to create checkbox.", "Error", MB_OK);
        }

        return 0;
    case WM_COMMAND:
        if (LOWORD(wp) == ID_BTN_TOGGLE) {
            toggle(hwnd);
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
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmd, int show) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "AutoClickerWnd";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("AutoClickerWnd", "Auto Clicker",                       //
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
