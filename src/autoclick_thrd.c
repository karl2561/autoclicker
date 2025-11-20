#include "autoclick_thrd.h"
#include "utils/time.h"

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

struct autoclick_config ac = {.mode = AUTOCLICK_NORMAL};

void ac_set(enum autoclick_modes new_mode, unsigned i)
{
	switch (new_mode) {
	case AUTOCLICK_TIMER:
		ac.mode = AUTOCLICK_TIMER;
		ac.i = i;
		return;
	case AUTOCLICK_AMOUNT:
		ac.mode = AUTOCLICK_AMOUNT;
		ac.i = i;
		return;
	case AUTOCLICK_NORMAL:
		ac.mode = AUTOCLICK_NORMAL;
		return;
	}
}

void ac_clear()
{
	switch (ac.mode) {
	case AUTOCLICK_TIMER:
	case AUTOCLICK_AMOUNT:
	case AUTOCLICK_NORMAL:
		ac.mode = AUTOCLICK_NORMAL;
	}
}

void write_click_event(int fd, int keycode)
{
	struct input_event click[4] = {
		{.type = EV_KEY, .code = keycode, .value = 1},
		{.type = EV_SYN, .code = SYN_REPORT, .value = 0},
		{.type = EV_KEY, .code = keycode, .value = 0},
		{.type = EV_SYN, .code = SYN_REPORT, .value = 0},
	};

	write(fd, &click, sizeof(click));
}

void write_move_event(int fd, int x, int y)
{
	struct input_event move[3] = {
		{.type = EV_ABS, .code = ABS_X, .value = x},
		{.type = EV_ABS, .code = ABS_Y, .value = y},
		{.type = EV_SYN, .code = SYN_REPORT, .value = 0}};

	write(fd, &move, sizeof(move));
}

int autoclick_thread(void *arg)
{
	(void)arg;
	int fd = create_autokey_setup(cfg.key_pressed);
	if (fd < 0)
		return 1;

	if (ac.mode == AUTOCLICK_TIMER) {
		thrd_t timer;
		thrd_create(&timer, timer_thread, NULL);
		thrd_detach(timer);
	}

	struct timespec *wait = create_timespec(cfg.interval_ms);

	while (atomic_load(&start)) {
		write_click_event(fd, cfg.key_pressed);
		thrd_sleep(wait, NULL);
	}

	free(wait);
	remove_autokey_setup(fd);
	return 0;
}

int pattern_thread_amount(void *arg)
{
	struct pattern_arg *pattern = arg;
	pattern->fd = create_autokey_setup(cfg.key_pressed);
	if (pattern->fd < 0) {
		free(arg);
		return 1;
	}

	for (int i = 0; i < pattern->amount && atomic_load(&start); i++)
		pattern_thread(pattern);

	remove_autokey_setup(pattern->fd);
	slist_delete(&pattern->pattern, remove_item);
	free(arg);

	return 0;
}

int pattern_thread(void *arg)
{
	struct pattern_arg *pattern = arg;
	struct slist *head = pattern->pattern;
	int fd = pattern->fd;

	struct Movement abs_pos = {.x = 100, .y = 100};
	struct timespec *wait = create_timespec(cfg.interval_ms);

	for (struct slist *node = head; node && atomic_load(&start);
		node = node->next) {
		struct Movement *change = node->data;
		abs_pos.x += change->x;
		abs_pos.y += change->y;
		write_move_event(fd, abs_pos.x, abs_pos.y);
		write_click_event(fd, cfg.key_pressed);
		thrd_sleep(wait, NULL);
	}

	free(wait);
	return 0;
}

int timer_thread(void *arg)
{
	(void)arg;
	struct slist *head = nullptr;
	struct timespec end, now;
	struct timespec *duration = create_timespec(ac.i);
	clock_gettime(CLOCK_REALTIME, &end);
	end.tv_sec += duration->tv_sec;
	end.tv_nsec += duration->tv_nsec;
	double remaining = 1;
	struct timespec *wait = create_timespec(1000 / 60);

	while (atomic_load(&start) && remaining > 0) {
		clock_gettime(CLOCK_REALTIME, &now);
		remaining = (end.tv_sec - now.tv_sec) +
			(end.tv_nsec - now.tv_nsec) * 1e-9;

		if (head) {
			slist_delete(&head, remove_line);
			head = nullptr;
		}
		struct printed_line *line =
			create_line("Time remaining: %.3f\n", remaining);
		head = slist_push(head, line);
		thrd_sleep(wait, NULL);
	}

	free(wait);
	ac_clear();
	atomic_store(&start, false);
	if (head)
		slist_delete(&head, remove_line);
	return 0;
}

int amount_thread(void *arg)
{
	(void)arg;
	unsigned amount = ac.i;
	int fd = create_autokey_setup(cfg.key_pressed);
	if (fd < 0)
		return 1;

	struct timespec *wait = create_timespec(cfg.interval_ms);
	for (unsigned i = 0; i < amount && atomic_load(&start); i++) {
		write_click_event(fd, cfg.key_pressed);
		thrd_sleep(wait, NULL);
	}

	free(wait);
	ac_clear();
	atomic_store(&start, false);
	remove_autokey_setup(fd);
	return 0;
}
