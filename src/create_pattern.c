#include "create_pattern.h"

#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void remove_item(void *data)
{
	(void)data;
	return;
}

void print_item(void *data)
{
	struct Movement *mv = data;
	printf("x: %d, y: %d\n", mv->x, mv->y);
}

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
