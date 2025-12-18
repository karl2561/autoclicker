/**
 * @file handle_file.c
 * @brief Implements functions for file I/O, device setup, and configuration
 * management.
 */
#include "handle_file.h"

#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @brief Creates and configures a virtual input device using uinput.
 * @param keycode The keycode that the virtual device will be able to emit
 * (e.g., for mouse clicks).
 * @return The file descriptor for the new uinput device, or a negative value on
 * error.
 */
int create_autokey_setup(int keycode)
{
	int fd = open(PATH_UINPUT, O_WRONLY | O_NONBLOCK);
	if (fd < 0)
		return fd;

	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_KEYBIT, keycode);

	ioctl(fd, UI_SET_EVBIT, EV_REL);
	ioctl(fd, UI_SET_RELBIT, REL_X);
	ioctl(fd, UI_SET_RELBIT, REL_Y);

	ioctl(fd, UI_SET_EVBIT, EV_ABS);
	ioctl(fd, UI_SET_ABSBIT, ABS_X);
	ioctl(fd, UI_SET_ABSBIT, ABS_Y);

	ioctl(fd, UI_SET_EVBIT, EV_SYN);

	struct uinput_abs_setup abs_setup;
	memset(&abs_setup, 0, sizeof(abs_setup));
	abs_setup.code = ABS_X;
	abs_setup.absinfo.minimum = 0;
	abs_setup.absinfo.maximum = SCREENHEIGHT;
	ioctl(fd, UI_ABS_SETUP, &abs_setup);

	abs_setup.code = ABS_Y;
	abs_setup.absinfo.minimum = 0;
	abs_setup.absinfo.maximum = SCREENWIDTH;
	ioctl(fd, UI_ABS_SETUP, &abs_setup);

	struct uinput_setup usetup;
	memset(&usetup, 0, sizeof(usetup));
	snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, "my-autoclicker");
	usetup.id.bustype = BUS_USB;
	usetup.id.vendor = 0x1234;
	usetup.id.product = 0x5678;
	usetup.id.version = 1;

	ioctl(fd, UI_DEV_SETUP, &usetup);
	ioctl(fd, UI_DEV_CREATE);

	return fd;
}

/**
 * @brief Destroys a virtual uinput device and closes its file descriptor.
 * @param fd The file descriptor of the uinput device to destroy.
 */
void remove_autokey_setup(int fd)
{
	ioctl(fd, UI_DEV_DESTROY);
	close(fd);
}

/**
 * @brief Opens the keyboard event device and sets it to non-blocking raw mode.
 * @param flags A pointer to an integer where the original file status flags
 * will be stored.
 * @return The file descriptor for the keyboard event device, or a negative
 * value on error.
 */
int create_keypress_setup(int *flags)
{
	int fd = open(PATH_KEYEV, O_RDONLY);
	if (fd < 0) {
		return fd;
	}
	raw_mode_setup();

	*flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, *flags | O_NONBLOCK);

	return fd;
}

/**
 * @brief Restores the original file status flags and closes the keyboard event
 * device.
 * @param fd The file descriptor of the keyboard event device.
 * @param flags The original file status flags to restore.
 */
void remove_keypress_setup(int fd, int flags)
{
	fcntl(fd, F_SETFL, flags);
	close(fd);
}

/**
 * @brief Inserts a configuration value from a line of text into the config
 * struct.
 * @param position The index in `cfg_map` corresponding to the config setting.
 * @param line The line of text from the config file.
 */
static void insert_from_configs(int position, char *line)
{
	int length = strlen(cfg_map[position].name);
	if (strncmp(line, cfg_map[position].name, length))
		return;

	int tmp_value;
	// returns 1 if it read one int successfully.
	// returns 0 if it saw something, but couldn't parsse it
	// returns EOF (usually -1) -> end of input or empty line
	if (sscanf(line + length + 1, "%d", &tmp_value) != 1)
		return;

	if (cfg_map[position].type == CFG_KEY)
		*(int *)cfg_map[position].field = tmp_value;
	if (cfg_map[position].type == CFG_BTN)
		*(int *)cfg_map[position].field = tmp_value;
	else if (cfg_map[position].type == CFG_INT_A)
		atomic_store((atomic_int *)cfg_map[position].field, tmp_value);
}

/**
 * @brief Reads application settings from the configuration file.
 * @return 0 on success, -1 if the config file cannot be opened.
 */
int read_config()
{
	FILE *fconf = fopen(PATH_CONFIG_SAVE, "r");
	if (!fconf)
		return -1;

	char line[MAX_LINE_LENGTH];
	while (fgets(line, sizeof(line), fconf))
		for (size_t i = 0; i < cfg_map_length; i++)
			insert_from_configs(i, line);

	fclose(fconf);
	return 0;
}

/**
 * @brief Writes the current application settings to the configuration file.
 * @return 0 on success, -1 if the config file cannot be opened for writing.
 */
int write_config()
{
	FILE *fconf = fopen(PATH_CONFIG_SAVE, "w");
	if (!fconf)
		return -1;

	for (size_t i = 0; i < cfg_map_length; i++) {
		int value = convert_field_to_int(i);
		fprintf(fconf, "%s=%d\n", cfg_map[i].name, value);
	}

	fclose(fconf);
	return 0;
}
