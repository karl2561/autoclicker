#ifndef AUTOCLICK_H
#define AUTOCLICK_H

#include "create_pattern.h"
#include "utils/config.h"
#include "utils/handle_file.h"
#include "utils/print_line.h"
#include "utils/slist.h"
#include "utils/time.h"

typedef enum {
	AUTOCLICK_NORMAL,
	AUTOCLICK_TIMER,
	AUTOCLICK_AMOUNT,
} autoclick_modes;

typedef struct {
	autoclick_modes mode;
	unsigned i;
} autoclicker_task;

extern autoclicker_task ac;

/* Switches the mode of the autoclicker and set the arg */
void ac_set(autoclick_modes new_mode, unsigned i);

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
