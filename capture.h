#ifndef CAPTURE_H
#define CAPTURE_H

#include "click.h"
#include <windows.h>

void capture_begin(HWND hwnd);
void capture_end(void);
BOOL capture_is_active(void);
BOOL capture_translate(UINT msg, WPARAM wp, LPARAM lp, ClickTarget *out);

#endif
