#include "keypress.h"

#include <asm-generic/errno.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <stdatomic.h>
#include <stdio.h>
#include <unistd.h>

int keypress_thread(void *arg)
{
	(void)arg;
	int flags = 0;
	int fd = create_keypress_setup(&flags);
	if (!fd) {
		perror("File open!\n");
		return 1;
	}

	int key_code = 0;
	while (!atomic_load(&quit) && key_code >= 0) {
		key_code = get_input(fd);

		if (key_code == cfg.key_start)
			handle_activation();
		else if (key_code == cfg.key_quit)
			handle_quit();
		else if (key_code == cfg.key_change_interval)
			handle_interval();
		else if (key_code == cfg.key_menu)
			handle_menu();
		else if (key_code == cfg.key_timer)
			click_timer();
		else if (key_code == cfg.key_amount)
			click_amount();
		else if (key_code == cfg.key_pattern)
			pattern();
	}

	if (!atomic_load(&quit))
		handle_quit();

	remove_keypress_setup(fd, flags);
	return 0;
}

int get_input(int fd)
{
	struct input_event ev, ev2;
	while (read(fd, &ev2, sizeof(ev2)) > 0)
		;

	while (1) {
		ssize_t n = read(fd, &ev, sizeof(ev));
		if (n == sizeof(ev)) {
			if (ev.code == cfg.key_pressed)
				continue;
			if (ev.type == EV_KEY && ev.value == 1)
				return ev.code;
		} else if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
			perror("read failed");
			return -1;
		}
	}
	return -1;
}
