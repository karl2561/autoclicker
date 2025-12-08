#ifndef WORKER_H
#define WORKER_H

#include "create_pattern.h"
#include "utils/config.h"
#include "utils/handle_file.h"
#include "utils/print_line.h"
#include "utils/sleep.h"
#include "utils/slist.h"

#include <threads.h>

extern atomic_bool worker_run;

typedef enum {
	AUTOCLICK_NORMAL,
	AUTOCLICK_TIMER,
	AUTOCLICK_AMOUNT,
	AUTOCLICK_PATTERN,
} autoclick_modes;

/*
 * Could add a struct slist *pattern;
 * Then I would not need to declare it but could always set it with ac_set
 * ac_clear would then be responsible to free the memory and set it to a nullptr
 * This is fine, however also cumbersome. Best would be if it wasn't freed.
 * Inside ac_set if it isn't a nullptr it would free the previous linked list
 * then read the new pattern. Thus minimizing the calls.
 */
typedef struct {
	struct slist *pattern;
	unsigned i;
	autoclick_modes mode;
} autoclicker_task_t;

typedef struct {
	autoclicker_task_t task;
	mtx_t mutex;
	cnd_t cond;
	atomic_bool available;
	atomic_bool running;
} worker_job_t;

/* Switches the mode of the autoclicker and set the arg */
void ac_set(autoclick_modes new_mode, unsigned i);

/* Clears the mode of the autoclicker */
void ac_clear();

/* Frees old pattern if exits, takes ownership from new_pattern */
void ac_set_pattern(struct slist *new_pattern);

/* Worker thread */
int autoclicker_worker(void *arg);

/* Starts the worker thread */
void start_worker();

/* Ends the worker thread */
void stop_worker();

/* Creates a new task for the autoclicker, returns true if running */
bool submit_task();

/* Thread that autoclicks when start is toggled */
extern int autoclick_thread(void *arg);

/* Creates a new thread, to click until the timer runs out */
extern int timer_thread(void *arg);

/* Creates a new thread, to click the received amount of times */
extern int amount_thread(void *arg);

typedef struct {
	struct slist *pattern;
	int fd;
} pattern_arg_t;

/* Thread that determines how often pattern thread will be run */
int pattern_thread_amount(void *arg);

/* Repeats a pattern once, then finishes */
extern int pattern_thread(void *arg);

#endif
