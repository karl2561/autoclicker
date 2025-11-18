#ifndef KEYPRESS_H
#define KEYPRESS_H

#include "config.h"
#include "menu_functions.h"
#include "utils/handleFile.h"

/* Thread for picking up keypresses. */
extern int keypress_thread(void *arg);

/* Listens to inputs to determine if a menus needs to be opened */
int get_input(int fd);

#endif
