#include "src/config.h"
#include "src/keypress.h"

#include <stdio.h>
#include <threads.h>

int main()
{

	if (read_config())
		printf("Couldn't read config\n");

	for (int i = 0; i < (int)cfg_map_length; i++)
		print_line_perm(i);

	thrd_t keypress;
	thrd_create(&keypress, keypress_thread, NULL);

	thrd_join(keypress, NULL);

	if (write_config()) {
		printf("Couldn't write config\n");
		return 1;
	}

	return 0;
}
