#define UNICODE
#define _UNICODE

#include "settings.h"
#include <stdio.h>
#include <wchar.h>

#define INI_SECTION      L"Settings"
#define INI_KEY_LANGUAGE L"Language"
#define INI_KEY_INTERVAL L"Interval"
#define INI_KEY_HOLD     L"Hold"
#define BUF_LEN          16

static void get_ini_path(wchar_t *out, size_t len) {
    GetModuleFileName(NULL, out, (DWORD)len);
    wchar_t *dot = wcsrchr(out, L'.');
    if (dot) {
        wcscpy(dot, L".ini");
    }
}

BOOL load_language(Lang *out) {
    wchar_t buf[BUF_LEN];
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    GetPrivateProfileString(INI_SECTION, INI_KEY_LANGUAGE, L"", buf, BUF_LEN, path);
    for (int i = 0; i < LANG_COUNT; i++) {
        if (wcscmp(buf, LANG_CODE[i]) == 0) {
            *out = (Lang)i;
            return TRUE;
        }
    }
    return FALSE;
}

void save_language(Lang lang) {
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    WritePrivateProfileString(INI_SECTION, INI_KEY_LANGUAGE, LANG_CODE[lang], path);
}

int load_interval(void) {
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    int ms = (int)GetPrivateProfileInt(INI_SECTION, INI_KEY_INTERVAL, DEFAULT_MS, path);
    return ms < MIN_MS ? DEFAULT_MS : ms;
}

void save_interval(int ms) {
    wchar_t buf[BUF_LEN];
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    _snwprintf(buf, BUF_LEN, L"%d", ms);
    WritePrivateProfileString(INI_SECTION, INI_KEY_INTERVAL, buf, path);
}

BOOL load_hold(void) {
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    int hold = (int)GetPrivateProfileInt(INI_SECTION, INI_KEY_HOLD, DEFAULT_HOLD, path);
    return hold != 0;
}

void save_hold(BOOL hold) {
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    WritePrivateProfileString(INI_SECTION, INI_KEY_HOLD, hold ? L"1" : L"0", path);
}
