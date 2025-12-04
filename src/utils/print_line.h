#ifndef PRINTLINE_H
#define PRINTLINE_H

#include "constants.h"
#include "conversion.h"
#include "slist.h"

/* Counts how often c appears in s */
int countChar(const char *s, const char c);

struct printed_line {
	char *text;
	int length; // #chars in the last line if partial (otherwise 0)
	int lines;  // #'\n'
};

/* Removes a line, frees it's memory */
void remove_line(void *data);

/* Creates a new line, returns the pointer */
[[nodiscard]] extern struct printed_line *create_line(const char *fmt, ...);

/* Exiting a handler function, called to confirm, redo, trash the changes,
 * return value is used to determine action, cleans up printed lines  */
[[nodiscard]] int exit_function(struct slist **head_ptr);

/* Waits for the user to close the menu */
void close_menu(struct slist **head_ptr, int key_code);

/* Retrieves a char from stdin, ignores all escaped characters */
[[nodiscard]] int get_char_stdin();

/* Retrieves the next input from stdin, converts it to keycode */
[[nodiscard]] int get_keycode_stdin();

/* Gets a positive integar from stdin, all non digits are ignored. returns -1 on
 * error*/
int get_number(char *line);

#endif
