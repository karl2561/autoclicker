#ifndef AUTOCLICK_H
#define AUTOCLICK_H

#include "utils/config.h"
#include "utils/create_pattern.h"
#include "utils/handleFile.h"
#include "utils/printLine.h"
#include "utils/slist.h"
#include "utils/time.h"

enum autoclick_modes {
	AUTOCLICK_NORMAL,
	AUTOCLICK_TIMER,
	AUTOCLICK_AMOUNT,
};

struct autoclick_config {
	enum autoclick_modes mode;
	unsigned i;
};

extern struct autoclick_config ac;

/* Switches the mode of the autoclicker and set the arg */
void ac_set(enum autoclick_modes new_mode, unsigned i);

/* Thread that autoclicks when start is toggled */
extern int autoclick_thread(void *arg);

/* Thread that determines how often pattern thread will be run */
int pattern_thread_amount(void *arg);

/* Repeats a pattern once, then finishes */
extern int pattern_thread(void *arg);

/* Creates a new thread, to click until the timer runs out */
extern int timer_thread(void *arg);

/* Creates a new thread, to click the received amount of times */
extern int amount_thread(void *arg);

#endif
