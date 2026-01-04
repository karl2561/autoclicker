/**
 * @file keypress.c
 * @brief Implements functions for handling user input and menu interactions.
 */
#include "keypress.h"
#include "utils/config.h"
#include "utils/handle_file.h"

#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>

/**
 * @brief Displays a standard exit/confirm menu and waits for user input.
 * @details Prints options to save, quit, or redo, then waits for the user to
 * press Enter, ESC, or 'r'. Cleans up all printed lines associated with the
 * menu upon exit.
 * @param head_ptr A pointer to the head of an `slist` of `printed_line` structs
 * to be cleaned up.
 * @return The character code of the user's choice ('\n', ESC_KEY, 'r', or -1 on
 * error).
 */
static int exit_function(struct slist **head_ptr, array_t *fd_array);

/**
 * @brief Waits for the user to press a specific key or the exit key to close a
 * menu.
 * @param head_ptr A pointer to the head of the `slist` of lines to be removed
 * upon closing.
 * @param key_code The special key code that also closes the menu.
 */
static void close_menu(struct slist **head_ptr, array_t *fd_array);

/** @brief List head for lines printed by `autoclick_toggle` to be
 * cleared later. */
static struct slist *start_line = nullptr;

/**
 * @brief Reads and returns a single key press event from an input device.
 *
 * This function reads from the given file descriptor, which is expected to be
 * an input event device. It blocks until a key-press event (EV_KEY with value
 * 1) is received, ignoring repeated key-down events for the currently pressed
 * key.
 *
 * @param list of file descriptors to listen to
 * @return The `ev.code` of the pressed key, or -1 if a read error occurs.
 */
int get_input(array_t *fd_array)
{
	struct input_event ev;
	size_t n = fd_array->len;
	struct pollfd *pfds = calloc(n, sizeof(struct pollfd));
	if (!pfds) return -1;

	/* Setup pollfds to drain old events */
	for (size_t i = 0; i < n; i++) {
		pfds[i].fd = ((int *)fd_array->data)[i];
		pfds[i].events = POLLIN;

		/* Draining old events */
		while (read(pfds[i].fd, &ev, sizeof(ev)) > 0)
			;
	}

	while (true) {
		if (poll(pfds, n, -1) < 0) {
			perror("poll");
			free(pfds);
			return -1;
		}

		for (size_t i = 0; i < n; i++) {
			if (!(pfds[i].revents & POLLIN)) continue;
			if (read(pfds[i].fd, &ev, sizeof(ev)) != sizeof(ev))
				continue;
			if (ev.type != EV_KEY || ev.value != 1) continue;
			if (ev.code != cfg.key_pressed) {
				free(pfds);
				return ev.code;
			}
		}
	}

	free(pfds);
	return -1;
}

/**
 * @brief Displays the current configuration settings in a menu.
 *
 * Iterates through the global `cfg_map`, creating and printing lines for each
 * configuration item. It then waits for the user to press the menu key to close
 * the display.
 */
void print_config(array_t *fd_array)
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

	close_menu(&head, fd_array);
}

/**
 * @brief Provides a menu for the user to change keybindings.
 *
 * Stops the autoclicker worker and displays a menu of available keybindings
 * that can be changed. The user can select a binding to modify and is then
 * prompted to press a new key. The function checks for conflicts with existing
 * keybindings before applying the change.
 */
void change_keybindings(array_t *fd_array)
{
	atomic_store(&worker_run, false);
	struct slist *head = nullptr;
	struct printed_line *line;
	char *name;
	int j = 0;
start:
	line = create_line("Select which key you want to change\n");
	head = slist_push(head, line);
	for (int i = 0; i < (int)cfg_map_length; i++) {
		if (cfg_map[i].type != CFG_KEY) continue;
		j += (1 << i);
		name = get_name(i);
		line = create_line("Press %d to change %s\n", i, name);
		head = slist_push(head, line);
	}
	line = create_line("Press ESC to exit this menu\n");
	head = slist_push(head, line);

	int c = get_input(fd_array);
	if (c == KEY_ESC) {
		slist_delete(&head, remove_line);
		return;
	}

	int value = (c - 1) % 10;
	if (c < 2 || c > 11 || !(j & (1 << value))) {
		line = create_line("Invalid selection, try again\n");
		head = slist_push(head, line);
		goto start;
	}
	int key_value, field_code;

select_key:
	name = get_name(value);
	line = create_line("Selected to change %s\n", name);
	head = slist_push(head, line);

	key_value = convert_field_to_int(value);
	line = create_line("Current activation key: %s\n", kctc(key_value));
	head = slist_push(head, line);
	line = create_line(
		"Press the key you wish to be the new activation key\n");
	head = slist_push(head, line);

	c = get_input(fd_array);

	for (int i = 0; i < (int)cfg_map_length; i++) {
		if (cfg_map[i].type != CFG_KEY) continue;
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

	switch (exit_function(&head, fd_array)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
		break;
	}

	*(int *)cfg_map[value].field = c;
	config_save(value);
}

/**
 * @brief Provides a menu for the user to change the key being clicked
 *
 * Stops the autoclicker worker and  lets the user select a new key to use for
 * auto clicking.
 */
void change_autoclick_btn(array_t *fd_array)
{
	atomic_store(&worker_run, false);
	struct slist *head = nullptr;
	struct printed_line *line;
start:
	line = create_line("Changing autoclick button\n");
	head = slist_push(head, line);

	line = create_line("Current key pressed: %s\n", kctc(cfg.key_pressed));
	head = slist_push(head, line);
	line = create_line(
		"Press the key you wish to be the new key pressed\n");
	head = slist_push(head, line);
	line = create_line("Press Arrow Right for Right Mouse Button\n");
	head = slist_push(head, line);
	line = create_line("Press Arrow Left for Left Mouse Button\n");
	head = slist_push(head, line);

	int c = get_input(fd_array);

	if (c == 105)
		line = create_line(
			"New autoclick key will be Left Mouse Button\n");
	else if (c == 106)
		line = create_line(
			"New autoclick key will be Right Mouse Button\n");
	else
		line = create_line("New autoclick key will be: %s\n", kctc(c));

	head = slist_push(head, line);

	switch (exit_function(&head, fd_array)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
		break;
	}
	if (c == 105)
		cfg.key_pressed = BTN_LEFT;
	else if (c == 106)
		cfg.key_pressed = BTN_RIGHT;
	else
		cfg.key_pressed = c;

	config_save(key_pressed);
}
/**
 * @brief Provides a menu for the user to change the autoclick interval.
 *
 * Stops the autoclicker worker and prompts the user to enter a new interval
 * in milliseconds.
 */
void change_autoclick_interval()
{
	atomic_store(&worker_run, false);
	struct slist *head = nullptr;
start:
	struct printed_line *line =
		create_line("Current interval: %dms\n", cfg.interval_ms);
	head = slist_push(head, line);
	int result = get_number_stdin("Enter your new interval: ");
	if (result < 0) {
		slist_delete(&head, remove_line);
		return;
	}

	line = create_line("New interval will be: %dms\n", result);
	head = slist_push(head, line);

	switch (exit_function_stdin(&head)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
	}

	cfg.interval_ms = result;
	config_save(interval_ms);
}

/**
 * @brief Toggles the autoclicker worker thread on or off.
 *
 * If the autoclicker is started, it prints a confirmation message. The
 * `start_line` global is used to keep track of this message so it can be
 * cleared later.
 */
void autoclick_toggle()
{
	if (start_line != nullptr) slist_delete(&start_line, remove_line);

	if (submit_task()) {
		struct printed_line *line;
		line = create_line("Autoclicker running.\n");
		start_line = slist_push(start_line, line);
	}
}

/**
 * @brief Provides a menu for setting a timed duration for the autoclicker.
 *
 * Prompts the user to enter a duration in seconds. On confirmation, it sets
 * the autoclicker task to `AUTOCLICK_TIMER` mode with the specified duration.
 */
void autoclick_timer()
{
start:
	struct slist *head = nullptr;
	struct printed_line *line;
	int duration = get_number_stdin(
		"Enter how many seconds the autoclicker will be active: ");
	if (duration < 0) {
		slist_delete(&head, remove_line);
		return;
	}

	line = create_line("Clicker will be active for %ds\n", duration);
	head = slist_push(head, line);

	switch (exit_function_stdin(&head)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
	}

	ac_set(AUTOCLICK_TIMER, duration * 1000);
}

/**
 * @brief Provides a menu for setting a specific number of clicks.
 *
 * Prompts the user to enter a number of clicks. On confirmation, it sets
 * the autoclicker task to `AUTOCLICK_AMOUNT` mode with the specified amount.
 */
void autoclick_amount()
{
start:
	struct slist *head = nullptr;
	struct printed_line *line;
	int amount = get_number_stdin("Enter how often you want to click: ");
	if (amount < 0) {
		slist_delete(&head, remove_line);
		return;
	}

	line = create_line("Will click  %d times\n", amount);
	head = slist_push(head, line);

	switch (exit_function_stdin(&head)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
	}

	ac_set(AUTOCLICK_AMOUNT, amount);
}

/**
 * @brief Initiates the process of recording a new mouse pattern.
 *
 * Displays instructions for the user to create a pattern by moving the mouse
 * and left-clicking. The recording stops on a right-click. The created pattern
 * is then written to a file and set as the active pattern for the worker.
 */
void record_pattern()
{
	struct slist *head = nullptr;
	struct printed_line *line = create_line("Create your Pattern\n");
	head = slist_push(head, line);
	line = create_line("Press right mousebutton when you are done.\n");
	head = slist_push(head, line);
	struct slist *pattern = create_pattern();
	line = create_line("Pattern created\n");
	head = slist_push(head, line);

	if (write_pattern(pattern) < 0) {
		line = create_line("Failed to save pattern\n");
		head = slist_push(head, line);
	}

	ac_set_pattern(pattern);
	slist_delete(&head, remove_line);
}

/**
 * @brief Provides a menu for playing the recorded pattern a number of times.
 *
 * Prompts the user to enter how many times the pattern should be repeated.
 * On confirmation, it sets the autoclicker task to `AUTOCLICK_PATTERN` mode
 * with the specified number of repetitions.
 */
void autoclick_pattern()
{
start:
	struct slist *head = nullptr;
	struct printed_line *line;
	int amount = get_number_stdin(
		"Enter how often you want to repeat your pattern: ");
	if (amount < 0) {
		slist_delete(&head, remove_line);
		return;
	}

	line = create_line("Will repeat pattern  %d times\n", amount);
	head = slist_push(head, line);

	switch (exit_function_stdin(&head)) {
	case 'r':
		goto start;
	case -1:
		return;
	case '\n':
	}

	ac_set(AUTOCLICK_PATTERN, amount);
}

/**
 * @brief Displays a standard exit/confirm menu and waits for user input.
 * @details Prints options to save, quit, or redo, then waits for the user to
 * press Enter, ESC, or 'r'. Cleans up all printed lines associated with the
 * menu upon exit.
 * @param head_ptr A pointer to the head of an `slist` of `printed_line` structs
 * to be cleaned up.
 * @return The character code of the user's choice ('\n', ESC_KEY, 'r', or -1 on
 * error).
 */
int exit_function(struct slist **head_ptr, array_t *fd_array)
{
	if (!head_ptr) return -1;

	struct printed_line *line;

	line = create_line("Press Enter to save.\n");
	*head_ptr = slist_push(*head_ptr, line);

	line = create_line("Press ESC to undo & quit the menu\n");
	*head_ptr = slist_push(*head_ptr, line);

	line = create_line("Press r to redo your entry\n");
	*head_ptr = slist_push(*head_ptr, line);

	int c;
	while ((c = get_input(fd_array))) {
		if (c == KEY_R) {
			c = 'r';
			break;
		} else if (c == KEY_ENTER) {
			c = '\n';
			break;
		} else if (c == KEY_ESC) {
			c = -1;
			break;
		}
	}

	slist_delete(head_ptr, remove_line);
	return c;
}

/**
 * @brief Waits for the user to press a specific key or the exit key to close a
 * menu.
 * @param head_ptr A pointer to the head of the `slist` of lines to be removed
 * upon closing.
 * @param key_code The special key code that also closes the menu.
 */
void close_menu(struct slist **head_ptr, array_t *fd_array)
{
	if (!head_ptr) return;

	int c;
	while ((c = get_input(fd_array))) {
		if (c == KEY_ESC || c == KEY_ENTER || c == cfg.key_show_config)
			break;
	}

	slist_delete(head_ptr, remove_line);
}
