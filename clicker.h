#ifndef CLICKER_H
#define CLICKER_H

#include <windows.h>

typedef enum {
    CLICKER_START_OK_TIMER,
    CLICKER_START_OK_HOLD,
    CLICKER_START_ERR_INTERVAL
} StartResult;

typedef enum {
    CLICKER_STOP_OK_TIMER,
    CLICKER_STOP_OK_HOLD
} StopResult;

StartResult clicker_start(int ms, BOOL hold);
StopResult clicker_stop(void);
BOOL clicker_is_running(void);

#endif
