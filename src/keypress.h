#ifndef MENUFUNCTIONS_H
#define MENUFUNCTIONS_H

#include "create_pattern.h"
#include "utils/config.h"
#include "utils/conversion.h"
#include "utils/print_line.h"
#include "utils/raw_mode.h"
#include "utils/sleep.h"
#include "utils/slist.h"
#include "worker.h"

/* Listens to inputs to determine if a menus needs to be opened */
int get_input(int fd);

/* Prints the keybindings to terminal, deletes them on closure */
void print_config();

/* Change keybindings for the autoclicker */
void change_keybindings();

/* Sets an interval, how long the autoclicker should run */
void change_autoclick_interval();

/* Toogles the autoclicker. On start creates the autokey threat and detaches it
 */
void autoclick_toggle();

/* Creates a timer for timer_thread */
void autoclick_timer();

/* Uses the amount to determine how long timer_thread needs to run */
void autoclick_amount();

/* Create a new pattern to replay later */
void record_pattern();

/* Playes a saved pattern, as often as specified */
void autoclick_pattern();

#endif
