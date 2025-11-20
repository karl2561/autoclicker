#ifndef HANDLE_FILE_H
#define HANDLE_FILE_H

#include "config.h"
#include "constants.h"
#include "raw_mode.h"

/* Creates an autokey setup, returns file connection */
[[nodiscard]] int create_autokey_setup(int keycode);

/* Closes the setup, removes the input device */
void remove_autokey_setup(int fd);

/* Creates the keypress setup, returns file connection, flags are preserved */
[[nodiscard]] int create_keypress_setup(int *flags);

/* Closes the setup, restores old flags */
void remove_keypress_setup(int fd, int flags);

/* Opens the config file, reads it's content to load existing settings */
extern int read_config();

/* Opens the config file,  overwrites it's content with the current settings */
extern int write_config();

#endif
