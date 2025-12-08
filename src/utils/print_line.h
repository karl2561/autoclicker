/**
 * @file print_line.h
 * @brief Provides functions for printing and managing lines in the terminal.
 *
 * This includes creating and removing lines, handling interactive menus, and
 * getting user input from stdin.
 */
#ifndef PRINTLINE_H
#define PRINTLINE_H

#include "constants.h"
#include "conversion.h"
#include "slist.h"

/**
 * @brief Counts how many times a character `c` appears in a string `s`.
 * @param s The null-terminated string to search.
 * @param c The character to count.
 * @return The number of occurrences of `c`.
 */
int countChar(const char *s, const char c);

/**
 * @struct printed_line
 * @brief Holds information about a block of text printed to the console.
 *
 * This is used to track printed lines so they can be erased later.
 */
struct printed_line {
	char *text; /**< The full text content that was printed. */
	int length; /**< The number of characters in the last line (if it doesn't end with a newline). */
	int lines;  /**< The number of newline characters in the text. */
};

/**
 * @brief Frees a `printed_line` struct and erases its content from the console.
 * @param data A void pointer to a `printed_line` struct.
 */
void remove_line(void *data);

/**
 * @brief Prints a formatted string to the console and creates a `printed_line` struct to track it.
 * @param fmt The format string, as in `printf`.
 * @param ... Variable arguments for the format string.
 * @return A pointer to the newly created `printed_line` struct. The caller is responsible for freeing this.
 */
[[nodiscard]] extern struct printed_line *create_line(const char *fmt, ...);

/**
 * @brief Displays a standard exit/confirm menu and waits for user input.
 * @details Prints options to save, quit, or redo, then waits for the user to press Enter, ESC, or 'r'.
 * Cleans up all printed lines associated with the menu upon exit.
 * @param head_ptr A pointer to the head of an `slist` of `printed_line` structs to be cleaned up.
 * @return The character code of the user's choice ('\n', ESC_KEY, 'r', or -1 on error).
 */
[[nodiscard]] int exit_function(struct slist **head_ptr);

/**
 * @brief Waits for the user to press a specific key or the exit key to close a menu.
 * @param head_ptr A pointer to the head of the `slist` of lines to be removed upon closing.
 * @param key_code The special key code that also closes the menu.
 */
void close_menu(struct slist **head_ptr, int key_code);

/**
 * @brief Reads a single character from stdin in raw mode.
 * @return The character code, or -1 on failure.
 */
[[nodiscard]] int get_char_stdin();

/**
 * @brief Reads a keypress from stdin and converts it to a Linux input event code.
 * @return The input event code, or -1 on failure.
 */
[[nodiscard]] int get_keycode_stdin();

/**
 * @brief Prompts the user to enter a positive integer from stdin.
 * @param line The prompt message to display to the user.
 * @return The entered number, or -1 if the user cancels with ESC.
 */
int get_number(char *line);

#endif
