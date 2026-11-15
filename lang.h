#ifndef LANG_H
#define LANG_H

#include <wchar.h>

typedef enum { LANG_EN, LANG_JA, LANG_COUNT } Lang;

enum {
    S_BTN_START,
    S_BTN_STOP,
    S_LABEL_INTERVAL,
    S_LABEL_HOLD,
    S_TITLE_ERROR,
    S_ERR_ALREADY_RUNNING,
    S_ERR_INTERVAL_TOO_SHORT,
    S_ERR_HOTKEY_ALREADY_REGISTERED,
    S_COUNT
};

extern Lang g_lang;
extern const wchar_t *LANG_CODE[LANG_COUNT];

const wchar_t *tr(int id);
Lang detect_language(void);

#endif
