/**
 * @file keypress.h
 * @brief Defines functions for handling user input and menu interactions.
 */
#ifndef MENUFUNCTIONS_H
#define MENUFUNCTIONS_H

#include "create_pattern.h"
#include "utils/config.h"
#include "utils/conversion.h"
#include "utils/print_line.h"
#include "utils/raw_mode.h"
#include "utils/sleep.h"
#include "utils/slist.h"
#include "worker.h"

/**
 * @brief Listens for keyboard input and processes recognized key commands.
 * @param list of file descriptors to listen to
 * @return The key code of the pressed key, or -1 on read failure.
 */
int get_input(array_t *fd_array);

/**
 * @brief Prints the current keybindings and configuration to the terminal.
 * @param fd The file descriptor for the input event device.
 * @details The menu stays open until the user presses the menu key or quit key.
 */
void print_config(array_t *fd_array);

/**
 * @brief Allows the user to change the keybindings for various actions.
 * @param fd The file descriptor for the input event device.
 * @details Enters a menu where the user can select an action and press a new
 * key to assign to it.
 */
void change_keybindings(array_t *fd_array);

/**
 * @brief Allows the user to change the keybindings which key is being clicked.
 * @param fd The file descriptor for the input event device.
 * @details Enters a menu where the user can select a new key to assign to it.
 */
void change_autoclick_btn(array_t *fd_array);
/**
 * @brief Allows the user to set the autoclicker click interval.
 * @details Enters a menu prompting the user to enter a new click interval in
 * milliseconds.
 */
void change_autoclick_interval();

/**
 * @brief Toggles the autoclicker on or off.
 * @details If not running, it submits a task to the worker thread to start
 * clicking. If running, it stops the worker.
 */
void autoclick_toggle();

/**
 * @brief Sets a timer for the autoclicker.
 * @details Prompts the user for a duration in seconds, then sets the
 * autoclicker to run for that amount of time.
 */
void autoclick_timer();

/**
 * @brief Sets the autoclicker to run for a specific number of clicks.
 * @details Prompts the user for a number of clicks, then sets the autoclicker
 * to perform that many clicks.
 */
void autoclick_amount();

/**
 * @brief Initiates the recording of a new mouse pattern.
 * @details Guides the user to create a pattern and saves it.
 */
void record_pattern();

/**
 * @brief Plays a saved mouse pattern a specified number of times.
 * @details Prompts the user for the number of repetitions.
 */
void autoclick_pattern();

#endif
