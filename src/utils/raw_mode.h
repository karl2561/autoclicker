/**
 * @file raw_mode.h
 * @brief Manages terminal raw mode settings.
 */
#ifndef RAWMODE_H
#define RAWMODE_H

/**
 * @brief Enables raw mode for the terminal.
 * @details Disables canonical mode and character echoing. It also registers `enable_input`
 * to be called on program exit to restore the terminal settings.
 */
extern void raw_mode_setup();

/**
 * @brief Restores the original terminal settings.
 * @details This function is typically registered with `atexit` to ensure the
 * terminal is returned to a usable state when the program terminates.
 */
void enable_input();

#endif
