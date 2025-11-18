#ifndef HANDLE_FILE_H
#define HANDLE_FILE_H

#include "raw_mode.h"

/* Takes a path and flags to open a sys file */
[[nodiscard]] int file_open(char *path, int flags);

/* Takes the int of an open sys file to close it */
void file_close(int fd);

/* Creates an autokey setup, returns file connection */
[[nodiscard]] int create_autokey_setup(int keycode);

/* Closes the setup, removes the input device */
void remove_autokey_setup(int fd);

/* Creates the keypress setup, returns file connection, flags are preserved */
[[nodiscard]] int create_keypress_setup(int *flags);

/* Closes the setup, restores old flags */
void remove_keypress_setup(int fd, int flags);

#endif
