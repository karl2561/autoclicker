/**
 * @file raw_mode.c
 * @brief Implements terminal raw mode management.
 */
#include "raw_mode.h"

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/** @brief Stores the original terminal attributes to restore them on exit. */
struct termios orig_term;

/**
 * @brief Enables raw mode for the terminal.
 * @details Disables canonical mode and character echoing. It also registers `enable_input`
 * to be called on program exit to restore the terminal settings.
 */
void raw_mode_setup()
{
	struct termios tmp;
	tcgetattr(STDIN_FILENO, &orig_term);
	atexit(enable_input);

	tmp = orig_term;
	tmp.c_lflag &= ~(ECHO | ICANON);
	tmp.c_cc[VMIN] = 1;
	tmp.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp);
}

/**
 * @brief Restores the original terminal settings.
 * @details This function is typically registered with `atexit` to ensure the
 * terminal is returned to a usable state when the program terminates.
 */
void enable_input()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
	tcflush(STDIN_FILENO, TCIFLUSH);
}
