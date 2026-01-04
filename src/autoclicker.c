/**
 * @file autoclicker.c
 * @brief Main entry point and event loop for the autoclicker application.
 */
#include "autoclicker.h"
#include "keypress.h"
#include "utils/array_t.h"
#include "utils/handle_file.h"
#include "worker.h"

#include <stdio.h>
#include <threads.h>

/** @brief Global head of the linked list for managing printed lines on the
 * screen. */
struct slist *head = nullptr;

/**
 * @brief Main entry point for the autoclicker application.
 *
 * Initializes the application, reads the configuration file, sets up keypress
 * event handling, and enters the main event loop. The loop listens for user
 * input and dispatches actions based on the configured keybindings.
 * Before exiting, it saves the configuration and cleans up resources.
 *
 * @return Returns 0 on successful execution, and 1 on error (e.g., if the
 *         keypress setup fails or the config cannot be written).
 */
int main()
{
	struct printed_line *line;
	create_abs_path();

	if (read_config()) {
		line = create_line("Couldn't read config file.\n");
		head = slist_push(head, line);
	}

	array_t *flag_array = array_init(sizeof(int));
	if (!flag_array) {
		perror("Failed to initialize flag array");
		return 1;
	}

	array_t *fd_array = create_keypress_setup(flag_array);
	if (!fd_array) {
		perror("Open file to read input");
		return 1;
	}

	start_worker();
	line = create_line(
		"Press %s to show info, %s to exit the program or menu.\n",
		kctc(cfg.key_show_config), kctc(cfg.key_quit));
	head = slist_push(head, line);

	int key_code = 0;
	while (key_code >= 0) {
		key_code = get_input(fd_array);

		if (key_code == cfg.key_show_config)
			print_config(fd_array);
		else if (key_code == cfg.key_start)
			autoclick_toggle();
		else if (key_code == cfg.key_quit)
			break;
		else if (key_code == cfg.key_change_interval)
			change_autoclick_interval();
		else if (key_code == cfg.key_menu)
			change_keybindings(fd_array);
		else if (key_code == cfg.key_timer)
			autoclick_timer();
		else if (key_code == cfg.key_amount)
			autoclick_amount();
		else if (key_code == cfg.key_create_pattern)
			record_pattern();
		else if (key_code == cfg.key_play_pattern)
			autoclick_pattern();
		else if (key_code == cfg.key_btn)
			change_autoclick_btn(fd_array);
	}

	stop_worker();
	atomic_store(&worker_run, false);

	slist_delete(&head, remove_line);
	remove_keypress_setup(fd_array, flag_array);
	array_free(flag_array);
	array_free(fd_array);
	return 0;
}
