#include "src/keypress.h"
#include "src/utils/config.h"
#include "src/utils/conversion.h"
#include "src/utils/handleFile.h"
#include "src/utils/printLine.h"
#include "src/utils/slist.h"

#include <stdio.h>
#include <threads.h>

struct slist *head = nullptr;

int main()
{
	struct printed_line *line;

	if (read_config()) {
		line = create_line("Couldn't read config file.\n");
		head = slist_push(head, line);
	}

	int flags = 0, fd = create_keypress_setup(&flags);
	if (fd < 0) {
		perror("File open!\n");
		return 1;
	}

	line = create_line("Autoclicker running.\nPress %s to show "
			   "info, %s to exit the program or menu.\n",
		kctc(cfg.key_show_config), kctc(cfg.key_quit));
	head = slist_push(head, line);

	int key_code = 0;
	while (key_code >= 0) {
		key_code = get_input(fd);

		if (key_code == cfg.key_show_config)
			print_config();
		else if (key_code == cfg.key_start)
			autoclick_toggle();
		else if (key_code == cfg.key_quit)
			break;
		else if (key_code == cfg.key_change_interval)
			change_autoclick_interval();
		else if (key_code == cfg.key_menu)
			change_keybindings();
		else if (key_code == cfg.key_timer)
			autoclick_timer();
		else if (key_code == cfg.key_amount)
			autoclick_amount();
		else if (key_code == cfg.key_create_pattern)
			record_pattern();
		else if (key_code == cfg.key_play_pattern)
			autoclick_pattern(1);
	}

	if (write_config()) {
		line = create_line("Couldn't write to config file.\n");
		head = slist_push(head, line);
		return 1;
	}

	atomic_store(&start, false);

	remove_keypress_setup(fd, flags);
	slist_delete(&head, remove_line);
	return 0;
}
