#ifndef SETTINGS_H
#define SETTINGS_H

#include "lang.h"
#include <windows.h>

#define DEFAULT_MS   1000
#define DEFAULT_HOLD 0
#define MIN_MS       100

BOOL load_language(Lang *out);
void save_language(Lang lang);
int load_interval(void);
void save_interval(int ms);
BOOL load_hold(void);
void save_hold(BOOL hold);

#endif
