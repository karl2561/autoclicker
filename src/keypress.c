#include "keypress.h"

#include <ctype.h>
#include <errno.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>

struct slist *start_line = nullptr;

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

void print_config()
{
	struct slist *head = nullptr;
	struct printed_line *line;
	for (int i = 0; i < (int)cfg_map_length; i++) {
		char const *description = cfg_map[i].description;
		int value = *(int *)cfg_map[i].field;
		switch (cfg_map[i].type) {
		case CFG_KEY:
			line = create_line(
				"Press %-3s to %s\n", kctc(value), description);
			break;
		case CFG_BTN:
			line = create_line("%s %s\n", kctc(value), description);
			break;
		case CFG_INT_A:
			value = convert_field_to_int(i);
			line = create_line("%s: %d\n", description, value);
			break;
		}
		head = slist_push(head, line);
	}

	close_menu(&head, cfg.key_menu);
}

void change_keybindings()
{
	atomic_store(&start, false);
	struct slist *head = nullptr;
	struct printed_line *line;
	char *name;
	int j = 0;
	for (int i = 0; i < (int)cfg_map_length; i++) {
		if (cfg_map[i].type != CFG_KEY)
			continue;
		j += (1 << i);
		name = get_name(i);
		line = create_line("Press %d to change %s\n", i, name);
		head = slist_push(head, line);
	}
	line = create_line("Press ESC to exit this menu\n");
	head = slist_push(head, line);

start:
	int c = get_char_stdin();
	if (c == ESC_KEY) {
		slist_delete(&head, remove_line);
		return;
	}
	int value = c - '0';
	if (!isdigit(c) || !(j & (1 << value))) {
		line = create_line("Invalid selection, try again\n");
		head = slist_push(head, line);
		goto start;
	}
	int key_value, field_code;

select_key:
	name = get_name(value);
	line = create_line("Selected to change %s\n", name);
	head = slist_push(head, line);
	if (cfg_map[value].type != CFG_KEY)
		return change_autoclick_interval();

	key_value = convert_field_to_int(value);
	line = create_line("Current activation key: %s\n", kctc(key_value));
	head = slist_push(head, line);
	line = create_line(
		"Press the key you wish to be the new activation key\n");
	head = slist_push(head, line);

	c = get_keycode_stdin();
	if (c == -1) {
		slist_delete(&head, remove_line);
		return;
	}

	for (int i = 0; i < (int)cfg_map_length; i++) {
		if (cfg_map[i].type != CFG_KEY)
			continue;
		field_code = convert_field_to_int(i);
		if (c == field_code) {
			name = get_name(i);
			line = create_line(
				"This key is already being used by %s\n", name);
			head = slist_push(head, line);
			line = create_line("Please choose another key\n");
			head = slist_push(head, line);
			goto select_key;
		}
	}
	free(name);

	line = create_line("New activation key will be: %s\n", kctc(c));
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
		break;
	}

	*(int *)cfg_map[value].field = c;
}

void change_autoclick_interval()
{
	atomic_store(&start, false);
	struct slist *head = nullptr;
start:
	struct printed_line *line =
		create_line("Current interval: %dms\n", cfg.interval_ms);
	head = slist_push(head, line);
	int result = get_number("Enter your new interval: ");
	if (result < 0) {
		slist_delete(&head, remove_line);
		return;
	}

	line = create_line("New interval will be: %dms\n", result);
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
	}
	cfg.interval_ms = result;
}

void autoclick_toggle()
{
	atomic_store(&start, !atomic_load(&start));
	if (start_line != nullptr)
		slist_delete(&start_line, remove_line);

	if (!atomic_load(&start))
		return;

	thrd_t autoclick;
	if (ac.mode == AUTOCLICK_AMOUNT)
		thrd_create(&autoclick, amount_thread, NULL);
	else
		thrd_create(&autoclick, autoclick_thread, NULL);

	thrd_detach(autoclick);

	struct printed_line *line;
	line = create_line("Autoclicker running.\n");
	start_line = slist_push(start_line, line);
}

void autoclick_timer()
{
start:
	struct slist *head = nullptr;
	struct printed_line *line;
	int duration = get_number(
		"Enter how many seconds the autoclicker will be active: ");
	if (duration < 0) {
		slist_delete(&head, remove_line);
		return;
	}

	line = create_line("Clicker will be active for %ds\n", duration);
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
	}

	ac_set(AUTOCLICK_TIMER, duration * 1000);
}

void autoclick_amount()
{
start:
	struct slist *head = nullptr;
	struct printed_line *line;
	int amount = get_number("Enter how often you want to click: ");
	if (amount < 0) {
		slist_delete(&head, remove_line);
		return;
	}

	line = create_line("Will click  %d times\n", amount);
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
	}

	ac_set(AUTOCLICK_AMOUNT, amount);
}

void record_pattern()
{
	struct slist *head = nullptr;
	struct printed_line *line = create_line("Create your Pattern\n");
	head = slist_push(head, line);
	line = create_line("Press right mousebutton when you are done.\n");
	head = slist_push(head, line);
	struct slist *head_pattern = create_pattern();
	line = create_line("Pattern created\n");
	head = slist_push(head, line);

	if (write_pattern(head_pattern) < 0) {
		line = create_line("Failed to save pattern\n");
		head = slist_push(head, line);
	}

	slist_delete(&head_pattern, remove_item);
	slist_delete(&head, remove_line);
}

void autoclick_pattern(int amount)
{
	struct pattern_arg *arg = malloc(sizeof(struct pattern_arg));
	arg->amount = amount;
	arg->pattern = read_pattern();
	if (!arg->pattern) {
		printf("No pattern to read\n");
		return;
	}
	thrd_t pattern_amount;
	thrd_create(&pattern_amount, pattern_thread_amount, arg);
	thrd_detach(pattern_amount);
}
