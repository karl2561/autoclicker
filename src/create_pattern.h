#ifndef RECORD_PATTERN_H
#define RECORD_PATTERN_H

#include "utils/constants.h"
#include "utils/handle_file.h"
#include "utils/slist.h"

/* Describes changes in the x and y axis */
struct Movement {
	int x;
	int y;
};

struct pattern_arg {
	struct slist *pattern;
	int amount;
	int fd;
};

/* Creates the Movement Data for the pattern */
[[nodiscard]] struct Movement *create_input(int x, int y);

/* Creates an slist with the Movement as data. Head is the first recorded
 * element. */
[[nodiscard]] extern struct slist *create_pattern();

/* Function to handle data when removing item from slist with data Movement  */
void remove_item(void *data);

/* Prints the contents of a Movement item to stdout */
void print_item(void *data);

/* Reads a previously recorded pattern from the config, returns head */
[[nodiscard]] struct slist *read_pattern();

/* Takes in a slist with a pattern and writes it to a file.  */
[[nodiscard]] int write_pattern(struct slist *head);

#endif
