#include "menu_functions.h"
#include "config.h"
#include "utils/conversion.h"

#include <ctype.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <termios.h>
#include <threads.h>
#include <unistd.h>

struct slist *start_line = nullptr;

#define ESC_KEY 27

ssize_t read_buf(int fd, char *buf)
{
	return read(fd, buf, MAX_LINE_LENGTH);
}

int get_char_stdin()
{
	int fd = STDIN_FILENO;
	char buf[MAX_LINE_LENGTH];
	ssize_t n;
	while (1) {
		n = read_buf(fd, buf);
		if (n != 1)
			continue;
		return buf[0];
	}
	return -1;
}

int get_keycode_stdin()
{
	int fd = STDIN_FILENO;
	char buf[MAX_LINE_LENGTH];
	ssize_t n;
	while (1) {
		n = read_buf(fd, buf);
		if (n == 0)
			continue;
		return ascii_to_evcode(buf, n);
	}
	return -1;
}

int get_number()
{
	struct slist *head = nullptr;
	int c = 1, pos = 0;
start:
	int result = 0;

	struct printed_line *line = create_line("Enter number: ");
	head = slist_push(head, line);

	while (1) {
		c = get_char_stdin();
		if (c == ESC_KEY) {
			slist_delete(&head, remove_line);
			return -1;
		} else if (c == EOF || c == '\n') {
			line = create_line("\n");
			head = slist_push(head, line);
			break;
		} else if (pos > 0 && c == '\b') {
			head = slist_pop(head, remove_line);
			result /= 10;
			pos--;
			continue;
		} else if (!isdigit(c))
			continue;
		++pos;
		int value = c - '0';
		line = create_line("%d", value);
		head = slist_push(head, line);
		result = 10 * result + value;
	}

	if (!result) {
		line = create_line("Interval must be bigger than 0.\n");
		head = slist_push(head, line);
		slist_delete(&head, remove_line);
		goto start;
	}

	switch (exit_function(&head)) {
	case 'r':
		goto start;
	case -1:
		result = -1;
	case '\n':
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
		return handle_interval();

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
	case 'r':
		goto start;
	case -1:
		disable_input();
		return;
	case '\n':
	}
	cfg.interval_ms = result;
	change_line(interval_ms);
}

void change_line(int position)
{
	if (start_line) {
		slist_delete(&start_line, remove_line);
	}

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
	case 'r':
		goto start;
	case -1:
		slist_delete(&head_pattern, remove_item);
		return 0;
	case '\n':
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
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
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
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
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
	while ((c = get_char_stdin()) != EOF) {
		if (c == 'r') {
			break;
		} else if (c == '\n') {
			break;
		} else if (c == ESC_KEY) {
			break;
		}
	}

	slist_delete(&head, remove_line);
	return c;
}
