#ifndef AUTOCLICK_H
#define AUTOCLICK_H

#include "config.h"
#include "utils/create_pattern.h"
#include "utils/handleFile.h"
#include "utils/printLine.h"
#include "utils/slist.h"
#include "utils/time.h"

extern int autoclick_thread(void *arg);
extern int pattern_thread(void *arg);

/* Creates a new thread, to click until the timer runs out */
int timer_thread(void *arg);

#endif
