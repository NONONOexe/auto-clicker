#ifndef SETTINGS_H
#define SETTINGS_H

#include "lang.h"
#include <windows.h>

#define DEFAULT_MS 1000
#define MIN_MS     100

BOOL load_language(Lang *out);
void save_language(Lang lang);
int load_interval(void);
void save_interval(int ms);

#endif
