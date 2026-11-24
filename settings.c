#define UNICODE
#define _UNICODE

#include "settings.h"
#include "hotkey.h"
#include <wchar.h>

#define INI_SECTION_SETTINGS L"Settings"
#define INI_KEY_LANGUAGE     L"Language"
#define INI_KEY_INTERVAL     L"Interval"
#define INI_KEY_HOLD         L"Hold"

#define INI_SECTION_HOTKEY L"Hotkey"
#define INI_KEY_HOTKEY_VK  L"VK"
#define INI_KEY_HOTKEY_MOD L"Mod"
#define BUF_LEN            16

#define INI_SECTION_TARGET    L"Target"
#define INI_KEY_TARGET_KIND   L"Kind"
#define INI_KEY_TARGET_BUTTON L"Button"
#define INI_KEY_TARGET_VK     L"VK"

static void get_ini_path(wchar_t *out, size_t len) {
    GetModuleFileName(NULL, out, (DWORD)len);
    wchar_t *dot = wcsrchr(out, L'.');
    if (dot) {
        wcscpy(dot, L".ini");
    }
}

static void read_str(const wchar_t *section, const wchar_t *key,
                     const wchar_t *def, wchar_t *out, DWORD len) {
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    GetPrivateProfileString(section, key, def, out, len, path);
}

static int read_int(const wchar_t *section, const wchar_t *key, int def) {
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    return (int)GetPrivateProfileInt(section, key, def, path);
}

static UINT read_uint(const wchar_t *section, const wchar_t *key, UINT def) {
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    return GetPrivateProfileInt(section, key, def, path);
}

static void write_str(const wchar_t *section, const wchar_t *key,
                      const wchar_t *value) {
    wchar_t path[MAX_PATH];
    get_ini_path(path, MAX_PATH);
    WritePrivateProfileString(section, key, value, path);
}

static void write_int(const wchar_t *section, const wchar_t *key, int value) {
    wchar_t buf[BUF_LEN];
    _snwprintf(buf, BUF_LEN, L"%d", value);
    write_str(section, key, buf);
}

static void write_uint(const wchar_t *section, const wchar_t *key, UINT value) {
    wchar_t buf[BUF_LEN];
    _snwprintf(buf, BUF_LEN, L"%u", value);
    write_str(section, key, buf);
}

BOOL load_language(Lang *out) {
    wchar_t buf[BUF_LEN];
    read_str(INI_SECTION_SETTINGS, INI_KEY_LANGUAGE, L"", buf, BUF_LEN);
    for (int i = 0; i < LANG_COUNT; i++) {
        if (wcscmp(buf, LANG_CODE[i]) == 0) {
            *out = (Lang)i;
            return TRUE;
        }
    }
    return FALSE;
}

void save_language(Lang lang) {
    write_str(INI_SECTION_SETTINGS, INI_KEY_LANGUAGE, LANG_CODE[lang]);
}

int load_interval(void) {
    int ms = read_int(INI_SECTION_SETTINGS, INI_KEY_INTERVAL, DEFAULT_MS);
    return ms < MIN_MS ? DEFAULT_MS : ms;
}

void save_interval(int ms) {
    write_int(INI_SECTION_SETTINGS, INI_KEY_INTERVAL, ms);
}

BOOL load_hold(void) {
    int hold = read_int(INI_SECTION_SETTINGS, INI_KEY_HOLD, DEFAULT_HOLD);
    return hold != 0;
}

void save_hold(BOOL hold) {
    write_int(INI_SECTION_SETTINGS, INI_KEY_HOLD, hold ? 1 : 0);
}

Hotkey load_hotkey(void) {
    return (Hotkey){
        .vk =
            read_uint(INI_SECTION_HOTKEY, INI_KEY_HOTKEY_VK, DEFAULT_HOTKEY_VK),
        .mod = read_uint(INI_SECTION_HOTKEY, INI_KEY_HOTKEY_MOD,
                         DEFAULT_HOTKEY_MOD),
    };
}

void save_hotkey(Hotkey hk) {
    write_uint(INI_SECTION_HOTKEY, INI_KEY_HOTKEY_VK, hk.vk);
    write_uint(INI_SECTION_HOTKEY, INI_KEY_HOTKEY_MOD, hk.mod);
}

ClickTarget load_target(void) {
    ClickTarget target = {
        .kind = read_int(INI_SECTION_TARGET, INI_KEY_TARGET_KIND,
                         DEFAULT_TARGET_KIND),
        .button = read_int(INI_SECTION_TARGET, INI_KEY_TARGET_BUTTON,
                           DEFAULT_TARGET_BUTTON),
        .vk = (WORD)read_uint(INI_SECTION_TARGET, INI_KEY_TARGET_VK,
                              DEFAULT_TARGET_VK),
    };

    if (target.kind != CLICK_TARGET_MOUSE && target.kind != CLICK_TARGET_KEY) {
        target.kind = DEFAULT_TARGET_KIND;
    }
    if (target.button < MOUSE_BTN_LEFT || target.button > MOUSE_BTN_X2) {
        target.button = DEFAULT_TARGET_BUTTON;
    }
    if (target.kind == CLICK_TARGET_KEY && (target.vk == 0 || target.vk > 255)) {
        target.kind = DEFAULT_TARGET_KIND;
        target.button = DEFAULT_TARGET_BUTTON;
    }
    return target;
}

void save_target(ClickTarget target) {
    write_uint(INI_SECTION_TARGET, INI_KEY_TARGET_KIND, target.kind);
    write_uint(INI_SECTION_TARGET, INI_KEY_TARGET_BUTTON, target.button);
    write_uint(INI_SECTION_TARGET, INI_KEY_TARGET_VK, target.vk);
}

Settings settings_load(void) {
    Settings s;
    if (!load_language(&s.lang)) {
        s.lang = detect_language();
    }
    s.interval = load_interval();
    s.hold = load_hold();
    s.hotkey = load_hotkey();
    s.target = load_target();
    return s;
}

void settings_save(const Settings *s) {
    save_language(s->lang);
    save_interval(s->interval);
    save_hold(s->hold);
    save_hotkey(s->hotkey);
    save_target(s->target);
}
