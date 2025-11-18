#ifndef PRINTLINE_H
#define PRINTLINE_H

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

#endif
