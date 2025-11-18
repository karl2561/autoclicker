#include "menu_functions.h"
#include "autoclick_thrd.h"
#include "config.h"
#include "utils/conversion.h"
#include "utils/create_pattern.h"
#include "utils/printLine.h"
#include "utils/raw_mode.h"
#include "utils/slist.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <threads.h>
#include <unistd.h>

struct slist *start_line = nullptr;

#define ESC_KEY 27

int get_number()
{
	struct slist *head = nullptr;
start:
	int result = 0;

	struct printed_line *line = create_line("Enter number: ");
	head = slist_push(head, line);

	disable_raw_mode();
	tcflush(STDIN_FILENO, TCIFLUSH);

	char input[MAX_LINE_LENGTH];
	char *end;

	if (!fgets(input, MAX_LINE_LENGTH, stdin))
		return 0;

	enable_raw_mode();
	head = slist_push(head, strdup(input));

	errno = 0;
	result = strtol(input, &end, 10);

	if (errno != 0 || end == input) {
		line = create_line("Invalid number.\n");
		head = slist_push(head, line);
	}

	if (!result) {
		line = create_line("Interval must be bigger than 0.\n");
		head = slist_push(head, line);
		slist_delete(&head, remove_line);
		goto start;
	}

	switch (exit_function(&head)) {
	case 1:
		goto start;
	case -1:
		result = -1;
	case 0:
	}

	return result;
}

void handle_activation()
{
	atomic_store(&start, !atomic_load(&start));
	if (atomic_load(&start)) {
		thrd_t autoclick;
		thrd_create(&autoclick, autoclick_thread, NULL);
		thrd_detach(autoclick);
	}
	if (start_line != nullptr)
		slist_delete(&start_line, remove_line);
	struct printed_line *line =
		create_line("Toggled start: %d\n", atomic_load(&start));
	start_line = slist_push(start_line, line);
}

void handle_quit()
{
	atomic_store(&quit, true);
	struct printed_line *line = create_line("Quitting the program!\n");
	start_line = slist_push(start_line, line);
}

void handle_menu()
{
	atomic_store(&start, false);
	struct slist *head = nullptr;
	char *name;
	int j = 0;
	for (int i = 0; i < (int)cfg_map_length; i++) {
		if (cfg_map[i].type != CFG_KEY)
			continue;
		j += (1 << i);
		name = get_name(i);
		struct printed_line *line =
			create_line("Press %d to change %s\n", i, name);
		head = slist_push(head, line);
	}
	struct printed_line *line =
		create_line("Press ESC to exit this menu\n");
	head = slist_push(head, line);

	disable_raw_mode();
	tcflush(STDIN_FILENO, TCIFLUSH);
start:
	int c = getchar();
	if (c == ESC_KEY) {
		slist_delete(&head, remove_line);
		return;
	}
	int value = c - '0';
	if (!(j & (1 << value))) {
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
		return handle_interval();

	key_value = convert_field_to_int(value);
	line = create_line("Current activation key: %s\n", kctc(key_value));
	head = slist_push(head, line);
	line = create_line(
		"Press the key you wish to be the new activation key\n");
	head = slist_push(head, line);
	c = getchar();
	if (c == ESC_KEY) {
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
	enable_raw_mode();

	line = create_line("New activation key will be: %s\n", kctc(c));
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 1:
		goto start;
	case -1:
		return;
	case 0:
		break;
	}

	*(int *)cfg_map[value].field = c;
	change_line(value);
}

void handle_interval()
{
	atomic_store(&start, false);
	struct slist *head = nullptr;
start:
	struct printed_line *line =
		create_line("Current interval: %dms\n", cfg.interval_ms);
	head = slist_push(head, line);
	line = create_line("Press ESC to exit this menu\n");
	head = slist_push(head, line);
	int result = get_number();

	line = create_line("New interval will be: %dms\n", result);
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 1:
		goto start;
	case -1:
		enable_raw_mode();
		return;
	case 0:
	}
	cfg.interval_ms = result;
	change_line(interval_ms);
}

void change_line(int position)
{
	if (start_line) {
		slist_delete(&start_line, remove_line);
	}

	printf("\033[F\033[2K");
	printf("\033[F\033[2K");
	printf("\033[F\033[2K");
	int line;
	for (line = cfg_map_length; line > position; line--) {
		printf("\033[F");
	}

	printf("\033[2K");
	print_line_perm(line++);
	for (; line < (int)cfg_map_length; line++) {
		printf("\033[E");
	}
	fflush(stdout);
}

int pattern()
{
	struct slist *head = nullptr;
	struct printed_line *line = create_line("Create your Pattern\n");
	head = slist_push(head, line);
	line = create_line("Press right mousebutton when you are done.\n");
	head = slist_push(head, line);
	struct slist *head_pattern = create_pattern();
	line = create_line("Pattern created\n");
	head = slist_push(head, line);

start:
	struct slist *arg = head_pattern;
	thrd_t pattern;
	thrd_create(&pattern, pattern_thread, arg);
	thrd_detach(pattern);

	line = create_line("Pattern repeated\n");
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 1:
		goto start;
	case -1:
		slist_delete(&head_pattern, remove_item);
		return 0;
	case 0:
		break;
	}

	return 0;
}

void click_timer()
{
start:
	struct slist *head = nullptr;
	struct printed_line *line = create_line(
		"Enter how long you want the autoclicker to be active\n");
	head = slist_push(head, line);
	int duration = get_number();

	line = create_line("Clicker will be active for %ds\n", duration);
	head = slist_push(head, line);
	line = create_line("Press ESC to exit this menu\n");
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 1:
		goto start;
	case -1:
		return;
	case 0:
	}

	struct timer_arg *arg = malloc(sizeof(struct timer_arg));
	arg->ms = duration * 1000;

	thrd_t timer;
	thrd_create(&timer, timer_thread, arg);
	thrd_detach(timer);
}

void click_amount()
{
start:
	struct slist *head = nullptr;
	struct printed_line *line =
		create_line("Enter how often you want the clicker to click\n");
	head = slist_push(head, line);
	int amount = get_number();

	line = create_line("Clicker click %d times\n", amount);
	head = slist_push(head, line);
	line = create_line("Press ESC to exit this menu\n");
	head = slist_push(head, line);

	switch (exit_function(&head)) {
	case 1:
		goto start;
	case -1:
		return;
	case 0:
	}

	struct timer_arg *arg = malloc(sizeof(struct timer_arg));
	arg->ms = amount * atomic_load(&cfg.interval_ms);

	thrd_t timer;
	thrd_create(&timer, timer_thread, arg);
	thrd_detach(timer);
}

int exit_function(struct slist **head_ptr)
{
	struct slist *head = *head_ptr;
	struct printed_line *line;

	line = create_line("Press Enter to save.\n");
	head = slist_push(head, line);

	line = create_line("Press ESC to undo & quit the menu\n");
	head = slist_push(head, line);

	line = create_line("Press r to redo your entry\n");
	head = slist_push(head, line);

	int c;
	disable_raw_mode();
	tcflush(STDIN_FILENO, TCIFLUSH);

	while ((c = getchar()) != EOF) {
		if (c == 'r' || c == 'R') {
			c = 1;
			break;
		} else if (c == '\n') {
			c = 0;
			break;
		} else if (c == ESC_KEY) {
			c = -1;
			break;
		}
	}

	enable_raw_mode();
	slist_delete(&head, remove_line);

	return c;
}
