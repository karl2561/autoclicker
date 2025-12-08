/**
 * @file create_pattern.c
 * @brief Implementation of functions for creating, reading, and writing mouse movement patterns.
 */
#include "create_pattern.h"

#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Allocates and initializes a new Movement struct.
 * @param x The x-coordinate relative movement.
 * @param y The y-coordinate relative movement.
 * @return A pointer to the newly allocated Movement struct, or `nullptr` on memory allocation failure.
 */
struct Movement *create_input(int x, int y)
{
	struct Movement *newPointer = malloc(sizeof(struct Movement));
	if (!newPointer) {
		perror("Creating pointer\n");
		return nullptr;
	}
	newPointer->x = x;
	newPointer->y = y;
	return newPointer;
}

/**
 * @brief Records a mouse movement pattern by reading from the mouse device file.
 *
 * This function opens the USB mouse device file and reads input events. It tracks
 * relative mouse movement (REL_X, REL_Y) and records the accumulated x and y
 * changes upon a left-click (BTN_LEFT). The recording session ends when the right
 * mouse button (BTN_RIGHT) is pressed. The movements are stored in a singly-linked
 * list.
 *
 * @return A pointer to the head of an `slist` containing the recorded pattern.
 *         Each node's data is a `struct Movement`. Returns `nullptr` on error.
 */
struct slist *create_pattern()
{
	int fd = open(PATH_USB_MOUSE, O_RDONLY);
	if (!fd) {
		perror("File open!\n");
		return nullptr;
	}

	struct input_event ev;
	struct slist *head = nullptr;
	struct slist *tail = nullptr;

	int x = 0, y = 0;
	while (read(fd, &ev, sizeof(ev)) > 0) {
		if (ev.type == EV_KEY && ev.code == BTN_RIGHT)
			break;
		if (ev.type == EV_REL) {
			if (ev.code == REL_X)
				x += ev.value;
			if (ev.code == REL_Y)
				y += ev.value;
		}

		if (ev.type == EV_KEY && ev.code == BTN_LEFT && ev.value == 1) {
			if (!head)
				head = tail = slist_append(
					nullptr, create_input(x, y));
			else
				tail = slist_append(tail, create_input(x, y));

			if (!tail)
				goto error;

			x = 0, y = 0;
		}
	}

	return head;
error:
	if (fd)
		close(fd);
	if (head)
		slist_delete(&head, remove_item);
	return nullptr;
}

/**
 * @brief Callback function to handle freeing a `Movement` struct's data.
 *
 * This function is designed to be used with `slist_delete` or `slist_pop`.
 * Since `struct Movement` itself doesn't contain pointers to allocated memory,
 * this function currently does nothing. The memory for the struct passed to `data`
 * is freed by the slist functions.
 *
 * @param data A void pointer to the data of a list node (expected to be `struct Movement*`).
 */
void remove_item(void *data)
{
	(void)data;
	return;
}

/**
 * @brief Prints the coordinates of a `Movement` struct to standard output.
 * @param data A void pointer to the data of a list node (expected to be `struct Movement*`).
 */
void print_item(void *data)
{
	struct Movement *mv = data;
	printf("x: %d, y: %d\n", mv->x, mv->y);
}

/**
 * @brief Reads a saved mouse pattern from a file.
 *
 * Opens the pattern file defined by `PATH_PATTERN_SAVE` and parses each line
 * to reconstruct the `Movement` data. Each line is expected to be in the format "x=  num,y=  num".
 *
 * @return A pointer to the head of an `slist` containing the pattern, or `nullptr` if the file
 *         cannot be opened or an error occurs during parsing.
 */
struct slist *read_pattern()
{
	FILE *fconf = fopen(PATH_PATTERN_SAVE, "r");
	if (!fconf)
		return nullptr;
	char line[MAX_LINE_LENGTH];
	struct slist *head = nullptr;
	struct slist *tail = nullptr;
	while (fgets(line, sizeof(line), fconf)) {
		int x_val, y_val;
		if (sscanf(line, "x=%5d,y=%5d", &x_val, &y_val) != 2) {
			printf("Failed to scan line\n");
			goto error;
		}
		struct Movement *pattern = create_input(x_val, y_val);
		if (!pattern)
			goto error;
		if (!head)
			head = tail = slist_append(nullptr, pattern);
		else
			tail = slist_append(tail, pattern);
		if (!tail)
			goto error;
	}

	fclose(fconf);
	return head;
error:
	slist_delete(&head, remove_item);
	fclose(fconf);
	return nullptr;
}

/**
 * @brief Writes a mouse pattern to a file.
 *
 * Opens the pattern file defined by `PATH_PATTERN_SAVE` in write mode and writes
 * the coordinates from each `Movement` struct in the provided linked list.
 * Each movement is written on a new line in the format "x=%5d,y=%5d".
 *
 * @param head A pointer to the head of the `slist` containing the pattern.
 * @return 0 on success, -1 on failure (e.g., if the file cannot be opened).
 */
int write_pattern(struct slist *head)
{
	FILE *fconf = fopen(PATH_PATTERN_SAVE, "w");
	if (!fconf)
		return -1;

	for (struct slist *node = head; node; node = node->next) {
		struct Movement *pattern = node->data;

		fprintf(fconf, "x=%5d,y=%5d\n", pattern->x, pattern->y);
	}

	fclose(fconf);
	return 0;
}
