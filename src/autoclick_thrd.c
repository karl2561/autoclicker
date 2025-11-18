#include "autoclick_thrd.h"

#include <fcntl.h>
#include <linux/uinput.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <threads.h>
#include <unistd.h>

int autoclick_thread(void *arg)
{
	(void)arg;
	int fd = create_autokey_setup(cfg.key_pressed);
	if (!fd)
		return 1;

	struct input_event ev[4] = {0};
	ev[0].type = EV_KEY, ev[0].code = cfg.key_pressed, ev[0].value = 1;
	ev[1].type = EV_SYN, ev[1].code = SYN_REPORT, ev[1].value = 0;
	ev[2].type = EV_KEY, ev[2].code = cfg.key_pressed, ev[2].value = 0;
	ev[3].type = EV_SYN, ev[3].code = SYN_REPORT, ev[3].value = 0;

	while (!atomic_load(&quit) && atomic_load(&start)) {
		sleep_ms(cfg.interval_ms);
		write(fd, &ev, sizeof(ev));
	}

	remove_autokey_setup(fd);
	return 0;
}

int pattern_thread(void *arg)
{

	if (!arg)
		return 1;
	struct slist *head = arg;
	int fd = create_autokey_setup(cfg.key_pressed);
	if (!fd)
		return 1;

	struct input_event ev[6] = {0};
	ev[0].type = EV_REL, ev[0].code = REL_X, ev[0].value = 0;
	ev[1].type = EV_REL, ev[1].code = REL_Y, ev[1].value = 0;
	ev[2].type = EV_KEY, ev[2].code = cfg.key_pressed, ev[2].value = 1;
	ev[3].type = EV_SYN, ev[3].code = SYN_REPORT, ev[3].value = 0;
	ev[4].type = EV_KEY, ev[4].code = cfg.key_pressed, ev[4].value = 0;
	ev[5].type = EV_SYN, ev[5].code = SYN_REPORT, ev[5].value = 0;

	while (head) {
		struct Movement *change = head->data;
		ev[0].value = change->x_change;
		ev[1].value = change->y_change;
		write(fd, &ev, sizeof(ev));
		head = head->next;
	}

	remove_autokey_setup(fd);

	return 0;
}

int timer_thread(void *arg)
{
	atomic_store(&start, true);
	struct timer_arg *t = arg;
	struct slist *head = nullptr;

	struct timespec end, now;
	clock_gettime(CLOCK_REALTIME, &end);
	end.tv_sec = (int)(t->ms / 1000);
	end.tv_nsec = (int)((t->ms - (int)(t->ms / 1000)) * 1e6);
	double remaining;

	while (1) {
		if (!atomic_load(&start))
			break;
		clock_gettime(CLOCK_REALTIME, &now);
		if (now.tv_sec > end.tv_sec ||
			(now.tv_sec == end.tv_sec &&
				now.tv_nsec >= end.tv_nsec))
			break;
		remaining = (end.tv_sec - now.tv_sec) +
			(end.tv_nsec - now.tv_nsec) * 1e-9;

		if (head) {
			slist_delete(&head, remove_line);
			head = nullptr;
		}
		struct printed_line *line =
			create_line("Time remaining: %.3f\n", remaining);
		head = slist_push(head, line);
		sleep_ms(1000 / 60);
	}

	atomic_store(&start, false);
	if (head)
		slist_delete(&head, remove_line);
	free(t);
	return 0;
}
