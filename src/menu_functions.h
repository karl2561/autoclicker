#ifndef MENUFUNCTIONS_H
#define MENUFUNCTIONS_H

#include "utils/slist.h"

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

struct timer_arg {
	unsigned int ms;
	struct printed_line *head;
};

/* Creates a timer for timer_thread */
void click_timer();

/* Uses the amount to determine how long timer_thread needs to run */
void click_amount();

/* Exits a function, cleans up the printed lines, raw mode is disabled on call
 */
int exit_function(struct slist **head_ptr);

#endif
