#ifndef AUTOCLICK_H
#define AUTOCLICK_H

extern int autoclick_thread(void *arg);
extern int pattern_thread(void *arg);

/* Creates a new thread, to click until the timer runs out */
int timer_thread(void *arg);

#endif
