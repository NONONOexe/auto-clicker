#define UNICODE
#define _UNICODE

#include "lang.h"
#include <windows.h>

Lang g_lang = LANG_EN;

const wchar_t *STR[LANG_COUNT][S_COUNT] = {
    [LANG_EN] =
        {
            [S_BTN_START] = L"Start",
            [S_BTN_STOP] = L"Stop",
            [S_BTN_CAPTURE] = L"Capture",
            [S_BTN_HOTKEY_APPLY] = L"Apply",
            [S_BTN_LANG] = L"English / 日本語",

            [S_LABEL_INTERVAL] = L"Interval (ms):",
            [S_LABEL_HOLD] = L"Hold the click",
            [S_LABEL_HOTKEY] = L"Hotkey",
            [S_LABEL_TARGET_FMT] = L"Target: %ls",
            [S_LABEL_TARGET_WAITING] = L"Press a mouse button or key...",

            [S_MOUSE_LEFT] = L"Left button",
            [S_MOUSE_RIGHT] = L"Right button",
            [S_MOUSE_MIDDLE] = L"Middle button",
            [S_MOUSE_X1] = L"Side button 1",
            [S_MOUSE_X2] = L"Side button 2",

            [S_TITLE_ERROR] = L"Error",

            [S_ERR_ALREADY_RUNNING] = L"Auto Clicker is already running.",
            [S_ERR_INTERVAL_TOO_SHORT] =
                L"Please set the click interval to 100 ms or more.",
            [S_ERR_HOTKEY_ALREADY_REGISTERED] =
                L"Hotkey could not be registered.",
            [S_ERR_UI_CREATE_FAILED] = L"Failed to start the application.",
        },
    [LANG_JA] =
        {
            [S_BTN_START] = L"開始",
            [S_BTN_STOP] = L"停止",
            [S_BTN_CAPTURE] = L"キャプチャ",
            [S_BTN_HOTKEY_APPLY] = L"適用",
            [S_BTN_LANG] = L"English / 日本語",

            [S_LABEL_INTERVAL] = L"クリックの間隔（ミリ秒）",
            [S_LABEL_HOLD] = L"クリックの長押し",
            [S_LABEL_HOTKEY] = L"ホットキー",
            [S_LABEL_TARGET_FMT] = L"対象: %ls",
            [S_LABEL_TARGET_WAITING] =
                L"マウスのボタンまたはキーを押してください...",

            [S_MOUSE_LEFT] = L"左ボタン",
            [S_MOUSE_RIGHT] = L"右ボタン",
            [S_MOUSE_MIDDLE] = L"ミドルボタン",
            [S_MOUSE_X1] = L"サイドボタン1",
            [S_MOUSE_X2] = L"サイドボタン2",

            [S_TITLE_ERROR] = L"エラー",

            [S_ERR_ALREADY_RUNNING] = L"Auto Clickerはすでに起動しています。",
            [S_ERR_INTERVAL_TOO_SHORT] =
                L"クリックの間隔は100ミリ秒以上にしてください。",
            [S_ERR_HOTKEY_ALREADY_REGISTERED] =
                L"ホットキーを設定できませんでした。",
            [S_ERR_UI_CREATE_FAILED] =
                L"アプリケーションを起動できませんでした。",
        },
};

const wchar_t *LANG_CODE[LANG_COUNT] = {
    [LANG_EN] = L"en",
    [LANG_JA] = L"ja",
};

const wchar_t *tr(int id) {
    return STR[g_lang][id];
}

Lang detect_language(void) {
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_JAPANESE ? LANG_JA
                                                                      : LANG_EN;
}
