#include "clicker.h"
#include "click.h"
#include "settings.h"

#include <windows.h>

static ClickTarget current_target;
static BOOL running = FALSE;
static BOOL hold_active = FALSE;

StartResult clicker_start(int ms, BOOL hold, ClickTarget target) {
    if (!hold && ms < MIN_MS) {
        return CLICKER_START_ERR_INTERVAL;
    }

    current_target = target;
    running = TRUE;
    hold_active = hold;
    if (hold_active) {
        click_target_down(&current_target);
        return CLICKER_START_OK_HOLD;
    } else {
        return CLICKER_START_OK_TIMER;
    }
}

StopResult clicker_stop(void) {
    running = FALSE;
    if (hold_active) {
        click_target_up(&current_target);
        hold_active = FALSE;
        return CLICKER_STOP_OK_HOLD;
    } else {
        return CLICKER_STOP_OK_TIMER;
    }
}

void clicker_tick(void) {
    click_target_click(&current_target);
}

BOOL clicker_is_running(void) {
    return running;
}
