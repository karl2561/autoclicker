/**
 * @file handle_file.h
 * @brief Defines functions for file I/O, device setup, and configuration
 * management.
 */
#ifndef HANDLE_FILE_H
#define HANDLE_FILE_H

#include "array_t.h"
#include "config.h"
#include "constants.h"
#include "raw_mode.h"

/**
 * @brief Creates and configures a virtual input device using uinput.
 * @param keycode The keycode that the virtual device will be able to emit
 * (e.g., for mouse clicks).
 * @return The file descriptor for the new uinput device, or a negative value on
 * error.
 */
[[nodiscard]] int create_autokey_setup(int keycode);

/**
 * @brief Destroys a virtual uinput device and closes its file descriptor.
 * @param fd The file descriptor of the uinput device to destroy.
 */
void remove_autokey_setup(int fd);

/**
 * @brief Opens the keyboard event device and sets it to non-blocking raw mode.
 * @param flags a nullptr, will return malloced.
 * @return array of results, nullptr on failure.
 */
array_t *create_keypress_setup(array_t *flag_array);

/**
 * @brief Restores the original file status flags and closes all fds.
 * @param array of file descriptors
 * @param array of their flags
 */
void remove_keypress_setup(array_t *fd_array, array_t *flag_array);

/**
 * @brief Opens the keyboard event device and sets it to non-blocking raw mode.
 * @param flags A pointer to an integer where the original file status flags
 * will be stored.
 * @return The file descriptor for the keyboard event device, or a negative
 * value on error.
 */
[[nodiscard]] int create_keypress_setup_old(int *flags);

/**
 * @brief Restores the original file status flags and closes the keyboard event
 * device.
 * @param fd The file descriptor of the keyboard event device.
 * @param flags The original file status flags to restore.
 */
void remove_keypress_setup_old(int fd, int flags);

/**
 * @brief Reads application settings from the configuration file.
 * @return 0 on success, -1 if the config file cannot be opened.
 */
extern int read_config();

/**
 * @brief Writes the current application settings to the configuration file.
 * @return 0 on success, -1 if the config file cannot be opened for writing.
 */
extern int write_config();

#endif
