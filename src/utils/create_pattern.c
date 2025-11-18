#include "create_pattern.h"
#include "handleFile.h"
#include "slist.h"

#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PATH "/dev/input/by-id/usb-1bcf_USB_Optical_Mouse-event-mouse"

struct Movement *createInput(int x, int y)
{
	struct Movement *newPointer = malloc(sizeof(struct Movement));
	if (!newPointer) {
		perror("Creating pointer\n");
		return nullptr;
	}
	newPointer->x_change = x;
	newPointer->y_change = y;
	return newPointer;
}

struct slist *create_pattern()
{
	int fd = file_open(PATH, O_RDONLY);
	if (!fd) {
		perror("File open!\n");
		return nullptr;
	}

	struct input_event ev;
	struct slist *head = nullptr;
	struct slist *tail = nullptr;
	head = tail = slist_append(nullptr, createInput(0, 0));

	int x = 0, y = 0;
	int x_total = 0, y_total = 0;
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
			tail = slist_append(tail, createInput(x, y));
			if (!tail)
				goto error;

			x_total -= x, y_total -= y;
			x = 0, y = 0;
		}
	}
	tail = slist_append(tail, createInput(x_total, y_total));

	return head;
error:
	if (fd)
		file_close(fd);
	if (head)
		slist_delete(&head, remove_item);
	return nullptr;
}

void remove_item(void *data)
{
	(void)data;
	return;
}
