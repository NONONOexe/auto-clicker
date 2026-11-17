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
            [S_BTN_HOTKEY_APPLY] = L"Apply",
            [S_LABEL_INTERVAL] = L"Interval (ms):",
            [S_LABEL_HOLD] = L"Hold the click",
            [S_LABEL_HOTKEY] = L"Hotkey",
            [S_TITLE_ERROR] = L"Error",
            [S_ERR_ALREADY_RUNNING] = L"Auto Clicker is already running.",
            [S_ERR_INTERVAL_TOO_SHORT] = L"Please set the click interval to 100 ms or more.",
            [S_ERR_HOTKEY_ALREADY_REGISTERED] = L"Hotkey could not be registered.",
        },
    [LANG_JA] =
        {
            [S_BTN_START] = L"開始",
            [S_BTN_STOP] = L"停止",
            [S_BTN_HOTKEY_APPLY] = L"適用",
            [S_LABEL_INTERVAL] = L"クリックの間隔（ミリ秒）",
            [S_LABEL_HOLD] = L"クリックの長押し",
            [S_LABEL_HOTKEY] = L"ホットキー",
            [S_TITLE_ERROR] = L"エラー",
            [S_ERR_ALREADY_RUNNING] = L"Auto Clickerはすでに起動しています。",
            [S_ERR_INTERVAL_TOO_SHORT] = L"クリックの間隔は100ミリ秒以上にしてください。",
            [S_ERR_HOTKEY_ALREADY_REGISTERED] = L"ホットキーを設定できませんでした。",
        },
};

const wchar_t *LANG_CODE[LANG_COUNT] = {
    [LANG_EN] = L"en",
    [LANG_JA] = L"ja",
};

const wchar_t *tr(int id) { return STR[g_lang][id]; }

Lang detect_language(void) {
    return PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_JAPANESE ? LANG_JA : LANG_EN;
}
