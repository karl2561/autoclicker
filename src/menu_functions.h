#ifndef MENUFUNCTIONS_H
#define MENUFUNCTIONS_H

#include "autoclick_thrd.h"
#include "config.h"
#include "utils/conversion.h"
#include "utils/create_pattern.h"
#include "utils/printLine.h"
#include "utils/raw_mode.h"
#include "utils/slist.h"
#include "utils/time.h"

/**/
int get_number();
void handle_activation();
void handle_quit();
// Function is limited to 10 options!
void handle_menu();
void handle_interval();
void change_line(int position);

/* Create a new pattern */
int pattern();

/* Creates a timer for timer_thread */
void click_timer();

/* Uses the amount to determine how long timer_thread needs to run */
void click_amount();

/* Exits a function, cleans up the printed lines, raw mode is disabled on call
 */
int exit_function(struct slist **head_ptr);

#endif
