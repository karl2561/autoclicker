/**
 * @file create_pattern.h
 * @brief Functions for creating, reading, and writing mouse movement patterns.
 */
#ifndef RECORD_PATTERN_H
#define RECORD_PATTERN_H

#include "utils/constants.h"
#include "utils/handle_file.h"
#include "utils/slist.h"

/**
 * @struct Movement
 * @brief Describes changes in the x and y axis for a mouse movement.
 */
struct Movement {
	int x; /**< Change in the x-coordinate. */
	int y; /**< Change in the y-coordinate. */
};

/**
 * @brief Creates a Movement data structure for the pattern.
 * @param x The change in the x-coordinate.
 * @param y The change in the y-coordinate.
 * @return A pointer to the newly created Movement struct. The caller is
 * responsible for freeing this memory.
 */
[[nodiscard]] struct Movement *create_input(int x, int y);

/**
 * @brief Records a mouse movement pattern.
 * @details Records mouse movements and clicks until the right mouse button is
 * pressed. Each left-click stores the relative x and y movement since the last
 * click.
 * @return A singly-linked list (slist) where each node's data is a `struct
 * Movement`. The head is the first recorded element. Returns nullptr on
 * failure.
 */
[[nodiscard]] extern struct slist *create_pattern();

/**
 * @brief Callback function to handle data when removing an item from an slist
 * with Movement data.
 * @details This function is intended to be a callback for slist_delete or
 * slist_pop. It currently does nothing, as the Movement struct itself does not
 * allocate memory. The memory for the node data is freed by the slist
 * functions.
 * @param data A void pointer to the Movement data.
 */
void remove_item(void *data);

/**
 * @brief Prints the contents of a Movement item to stdout.
 * @param data A void pointer to the Movement data, expected to be a `struct
 * Movement*`.
 */
void print_item(void *data);

/**
 * @brief Reads a previously recorded pattern from the config file.
 * @return A singly-linked list (slist) containing the pattern, with the head as
 * the first element. Returns nullptr if the pattern file cannot be read.
 */
[[nodiscard]] struct slist *read_pattern();

/**
 * @brief Writes a pattern from an slist to a file.
 * @param head The head of the slist containing the pattern.
 * @return 0 on success, -1 on failure.
 */
[[nodiscard]] int write_pattern(struct slist *head);

#endif
