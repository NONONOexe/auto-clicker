#ifndef LANG_H
#define LANG_H

#include <wchar.h>

typedef enum { LANG_EN, LANG_JA, LANG_COUNT } Lang;

enum {
    S_NONE = -1,

    // Buttons
    S_BTN_START,
    S_BTN_STOP,
    S_BTN_CAPTURE,
    S_BTN_HOTKEY_APPLY,
    S_BTN_LANG,

    // Labels
    S_LABEL_INTERVAL,
    S_LABEL_HOLD,
    S_LABEL_HOTKEY,
    S_LABEL_TARGET_FMT,
    S_LABEL_TARGET_WAITING,

    // Mouse Buttons
    S_MOUSE_LEFT,
    S_MOUSE_RIGHT,
    S_MOUSE_MIDDLE,
    S_MOUSE_X1,
    S_MOUSE_X2,

    // Dialog Title
    S_TITLE_ERROR,

    // Error Messages
    S_ERR_ALREADY_RUNNING,
    S_ERR_INTERVAL_TOO_SHORT,
    S_ERR_HOTKEY_ALREADY_REGISTERED,
    S_ERR_UI_CREATE_FAILED,

    S_COUNT
};

extern Lang g_lang;
extern const wchar_t *LANG_CODE[LANG_COUNT];

const wchar_t *tr(int id);
Lang detect_language(void);

#endif
