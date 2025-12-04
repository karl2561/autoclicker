#include "print_line.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int countChar(const char *s, const char c)
{
	int count = 0;
	for (; *s; s++)
		if (*s == c)
			count++;

	return count;
}

void remove_line(void *data)
{
	struct printed_line *line = (struct printed_line *)data;

	if (!line)
		return;

	if (line->lines > 0) {
		if (line->length > 0)
			line->lines++;
		for (int i = 0; i < line->lines; i++)
			printf("\033[F\033[2K");
	} else {
		for (int i = 0; i < line->length; i++)
			printf("\b \b");
	}
	fflush(stdout);
}

struct printed_line *create_line(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int len = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	char *buf = malloc(len + 1);
	if (!buf)
		return nullptr;

	va_start(args, fmt);
	vsnprintf(buf, len + 1, fmt, args);
	va_end(args);

	struct printed_line *node = malloc(sizeof(struct printed_line));
	if (!node) {
		free(buf);
		return nullptr;
	}

	const char *last = strrchr(buf, '\n');
	node->text = buf;
	node->lines = countChar(buf, '\n');
	node->length = last ? strlen(last + 1) : strlen(buf);

	printf("%s", buf);
	fflush(stdout);

	return node;
}

int exit_function(struct slist **head_ptr)
{
	if (!head_ptr)
		return -1;

	struct printed_line *line;

	line = create_line("Press Enter to save.\n");
	*head_ptr = slist_push(*head_ptr, line);

	line = create_line("Press ESC to undo & quit the menu\n");
	*head_ptr = slist_push(*head_ptr, line);

	line = create_line("Press r to redo your entry\n");
	*head_ptr = slist_push(*head_ptr, line);

	int c;
	while ((c = get_char_stdin()) != EOF) {
		if (c == 'r') {
			break;
		} else if (c == '\n') {
			break;
		} else if (c == ESC_KEY) {
			break;
		}
	}

	slist_delete(head_ptr, remove_line);
	return c;
}

bool is_stop_char(int key_code, int c)
{
	return c == EOF || c == '\n' || c == ESC_KEY || c == key_code;
}

void close_menu(struct slist **head_ptr, int key_code)
{
	if (!head_ptr)
		return;

	int c;
	while ((c = get_char_stdin()) != EOF && !is_stop_char(key_code, c))
		;

	slist_delete(head_ptr, remove_line);
}

int get_char_stdin()
{
	char buf[MAX_LINE_LENGTH];
	while (1) {
		ssize_t n = read(STDIN_FILENO, buf, MAX_LINE_LENGTH);
		if (n != 1)
			continue;
		return buf[0];
	}
	return -1;
}

int get_keycode_stdin()
{
	char buf[MAX_LINE_LENGTH];
	while (1) {
		ssize_t n = read(STDIN_FILENO, buf, MAX_LINE_LENGTH);
		if (n == 0)
			continue;
		return ascii_to_evcode(buf, n);
	}
	return -1;
}

int get_number(char *prompt)
{
	struct slist *head = nullptr;
	struct printed_line *line;
	int c = 1, pos = 0;
start:
	line = create_line(prompt);
	head = slist_push(head, line);
	int result = 0;
	while (1) {
		c = get_char_stdin();
		if (c == ESC_KEY) {
			slist_delete(&head, remove_line);
			return -1;
		} else if (c == EOF || c == '\n') {
			line = create_line("\n");
			head = slist_push(head, line);
			break;
		} else if (pos > 0 && (c == '\b' || c == 127)) {
			head = slist_pop(head, remove_line);
			result /= 10;
			pos--;
			continue;
		} else if (!isdigit(c))
			continue;
		++pos;
		int value = c - '0';
		line = create_line("%d", value);
		head = slist_push(head, line);
		result = 10 * result + value;
	}

	if (!result) {
		slist_delete(&head, remove_line);
		line = create_line("Interval must be bigger than 0.\n");
		head = slist_push(head, line);
		goto start;
	}

	switch (exit_function(&head)) {
	case 'r':
		goto start;
	case -1:
		result = -1;
	case '\n':
	}

	return result;
}
