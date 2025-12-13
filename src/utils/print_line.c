/**
 * @file print_line.c
 * @brief Implements functions for printing and managing lines in the terminal.
 */
#include "print_line.h"
#include "slist.h"

#include <ctype.h>
#include <linux/input-event-codes.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Counts how many times a character `c` appears in a string `s`.
 * @param s The null-terminated string to search.
 * @param c The character to count.
 * @return The number of occurrences of `c`.
 */
int countChar(const char *s, const char c)
{
	int count = 0;
	for (; *s; s++)
		if (*s == c)
			count++;

	return count;
}

/**
 * @brief Frees a `printed_line` struct and erases its content from the console.
 * @details Uses ANSI escape codes to move the cursor up and clear lines.
 * @param data A void pointer to a `printed_line` struct.
 */
void remove_line(void *data)
{
	struct printed_line *line = (struct printed_line *)data;

	if (!line)
		return;

	if (line->line_count > 0) {
		if (line->trailing_char_count > 0)
			line->line_count++;
		for (size_t i = 0; i < line->line_count; i++)
			printf("\033[F\033[2K");
	} else {
		for (size_t i = 0; i < line->trailing_char_count; i++)
			printf("\b \b");
	}
	free(line->text);
	fflush(stdout);
}

/**
 * @brief Checks if a string ends in a newline character '\n'
 * @param const char *s string to be checked
 * @return boolean: true if ends in '\n', false otherwise
 */
bool ends_in_newline(const char *s)
{
	if (!s)
		return false;

	size_t len = strlen(s);
	return len > 0 && s[len - 1] == '\n';
}

/**
 * @brief Helper function for create_line and append_line
 * @param fmt The format string, as in `printf`.
 * @param args The arguments for fmt
 * @return A pointer to the newly created `printed_line` struct. The caller
 * needs to free printed_line.
 */
struct printed_line *create_line_va(const char *fmt, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args);
	size_t str_len = vsnprintf(NULL, 0, fmt, args_copy);
	va_end(args_copy);

	size_t cap = 1;

	if (ends_in_newline(fmt))
		cap = str_len + 1;
	else
		while (cap < str_len + 1)
			cap <<= 1;

	char *buf = malloc(cap);
	if (!buf)
		return nullptr;

	vsnprintf(buf, str_len + 1, fmt, args);

	struct printed_line *node = malloc(sizeof(struct printed_line));
	if (!node) {
		free(buf);
		return nullptr;
	}

	const char *last = strrchr(buf, '\n');
	node->text = buf;
	node->line_count = countChar(buf, '\n');
	node->trailing_char_count = last ? strlen(last + 1) : str_len;
	node->str_len = str_len + 1;
	node->cap = cap;

	printf("%s", buf);
	fflush(stdout);

	return node;
}

/**
 * @brief Prints a formatted string to the console and creates a `printed_line`
 * struct to track it.
 * @param fmt The format string, as in `printf`.
 * @param ... Variable arguments for the format string.
 * @return A pointer to the newly created `printed_line` struct. The caller
 * needs to free printed_line.
 */
struct printed_line *create_line(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	struct printed_line *node = create_line_va(fmt, args);
	va_end(args);
	return node;
}

/**
 * @brief Checks if line ends in '\n', if so calls create_line instead,
 * otherwise prints the formatted string to console and appends it to line.
 * @param line The printe_line to append to
 * @param fmt The format string, as in `printf`.
 * @param ... Variable arguments for the format string.
 * @return returns the pointer to line, or a newly created line. The caller
 * needs to free printed_line.
 */
struct printed_line *append_line(
	struct printed_line *line, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	if (ends_in_newline(line->text)) {
		struct printed_line *node = create_line_va(fmt, args);
		va_end(args);
		return node;
	}

	size_t str_len = vsnprintf(NULL, 0, fmt, args);
	va_end(args);

	char *buf = malloc(str_len + 1);
	if (!buf)
		return nullptr;

	if (line->cap < line->str_len + str_len)
		while (line->cap < line->str_len + str_len)
			line->cap <<= 1;

	va_start(args, fmt);
	vsnprintf(buf, str_len + 1, fmt, args);
	va_end(args);

	const char *last = strrchr(buf, '\n');
	size_t count_new_line = countChar(buf, '\n');
	line->line_count += count_new_line;
	size_t trailing_char_count = last ? strlen(last + 1) : str_len;
	if (count_new_line > 0)
		line->trailing_char_count = trailing_char_count;
	else
		line->trailing_char_count += trailing_char_count;

	printf("%s", buf);
	fflush(stdout);

	return line;
}

/**
 * @brief Displays a standard exit/confirm menu and waits for user input.
 * @details Prints options to save, quit, or redo, then waits for the user to
 * press Enter, ESC, or 'r'. Cleans up all printed lines associated with the
 * menu upon exit.
 * @param head_ptr A pointer to the head of an `slist` of `printed_line` structs
 * to be cleaned up.
 * @return The character code of the user's choice ('\n', ESC_KEY, 'r', or -1 on
 * error).
 */
int exit_function_stdin(struct slist **head_ptr)
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

/**
 * @brief Checks if a character is a stop character for a menu.
 * @param key_code A specific key code that also acts as a stop character.
 * @param c The character input by the user.
 * @return `true` if `c` is EOF, newline, ESC, or matches `key_code`, `false`
 * otherwise.
 */
bool is_stop_char_stdin(int key_code, int c)
{
	return c == EOF || c == KEY_ENTER || c == ESC_KEY || c == key_code;
}

/**
 * @brief Waits for the user to press a specific key or the exit key to close a
 * menu.
 * @param head_ptr A pointer to the head of the `slist` of lines to be removed
 * upon closing.
 * @param key_code The special key code that also closes the menu.
 */
void close_menu_stdin(struct slist **head_ptr, int key_code)
{
	if (!head_ptr)
		return;

	int c;
	while ((c = get_char_stdin()) != EOF &&
		!is_stop_char_stdin(key_code, c))
		;

	slist_delete(head_ptr, remove_line);
}

/**
 * @brief Reads a single character from stdin in raw mode.
 * @return The character code, or -1 on failure.
 */
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

/**
 * @brief Reads a keypress from stdin and converts it to a Linux input event
 * code.
 * @return The input event code, or -1 on failure.
 */
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

/**
 * @brief Prompts the user to enter a positive integer from stdin.
 * @details Handles backspace and ignores non-digit characters. After entry,
 * it shows a confirmation menu.
 * @param prompt The prompt message to display to the user.
 * @return The entered number, or -1 if the user cancels with ESC.
 */
int get_number_stdin(char *prompt)
{
	struct slist *head = nullptr;
	struct printed_line *line;
	int c = 1, pos = 0;
start:
	line = create_line(prompt); // prompt doesn't end in '\n'
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

	switch (exit_function_stdin(&head)) {
	case 'r':
		goto start;
	case -1:
		result = -1;
	case '\n':
	}

	return result;
}
