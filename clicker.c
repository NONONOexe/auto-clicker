#include "clicker.h"
#include "click.h"
#include "settings.h"

#include <windows.h>

static BOOL running = FALSE;
static BOOL hold_active = FALSE;

StartResult clicker_start(int ms, BOOL hold) {
    if (!hold && ms < MIN_MS) {
        return CLICKER_START_ERR_INTERVAL;
    }

    running = TRUE;
    hold_active = hold;
    if (hold_active) {
        send_mouse(MOUSEEVENTF_LEFTDOWN);
        return CLICKER_START_OK_HOLD;
    } else {
        return CLICKER_START_OK_TIMER;
    }
}

StopResult clicker_stop(void) {
    running = FALSE;
    if (hold_active) {
        send_mouse(MOUSEEVENTF_LEFTUP);
        hold_active = FALSE;
        return CLICKER_STOP_OK_HOLD;
    } else {
        return CLICKER_STOP_OK_TIMER;
    }
}

BOOL clicker_is_running(void) { return running; }
