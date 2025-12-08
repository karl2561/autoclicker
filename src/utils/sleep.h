/**
 * @file sleep.h
 * @brief Provides time-related utility functions.
 */
#ifndef SLEEP_H
#define SLEEP_H

/**
 * @brief Creates a `timespec` struct from a duration in milliseconds.
 * @param ms The duration in milliseconds.
 * @return A pointer to a dynamically allocated `timespec` struct. The caller must free this memory.
 */
[[nodiscard]] struct timespec *create_timespec(int ms);

#endif // SLEEP_H
