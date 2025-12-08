/**
 * @file sleep.c
 * @brief Implements time-related utility functions.
 */
#include "sleep.h"

#include <stdlib.h>
#include <threads.h>
#include <time.h>

/**
 * @brief Creates a `timespec` struct from a duration in milliseconds.
 * @param ms The duration in milliseconds.
 * @return A pointer to a dynamically allocated `timespec` struct. The caller must free this memory.
 */
struct timespec *create_timespec(int ms)
{
	struct timespec *ts = malloc(sizeof(struct timespec));
	ts->tv_sec = ms / 1000;
	ts->tv_nsec = (ms % 1000) * 1'000'000L;
	return ts;
}
