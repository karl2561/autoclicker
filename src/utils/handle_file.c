/**
 * @file handle_file.c
 * @brief Implements functions for file I/O, device setup, and configuration
 * management.
 */
#include "handle_file.h"
#include "array_t.h"

#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdlib.h>
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
	int fd = open(UINPUT_PATH, O_WRONLY | O_NONBLOCK);
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
 * @brief Gets a list of all matching input descriptor endings
 * @param udev used to enumerate over
 * @param property to match the list against
 * @param value of that property
 * @return A malloced array of all matches, containing the integers.
 */
array_t *get_input_device_list(
	char const *const property, char const *const value)
{
	struct udev *udev = udev_new();
	array_t *i = array_init(sizeof(int));
	struct udev_enumerate *e = udev_enumerate_new(udev);
	if (!udev || !i || !e)
		goto error;

	udev_enumerate_add_match_subsystem(e, "input");
	udev_enumerate_add_match_property(e, property, value);
	udev_enumerate_scan_devices(e);

	struct udev_list_entry *entry;
	struct udev_device *dev = nullptr;
	int const event_len = strlen(EVENT_PATH);
	char const *devnode = nullptr;
	char const *path = nullptr;
	udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(e))
	{
		path = udev_list_entry_get_name(entry);
		if (!(dev = udev_device_new_from_syspath(udev, path)))
			continue;

		if ((devnode = udev_device_get_devnode(dev)) &&
			strncmp(devnode, EVENT_PATH, event_len) == 0) {
			int event_int = atoi(devnode + event_len);
			if (array_push(i, &event_int) < 0) {
				perror("Failed to insert all elements");
				break;
			}
		}

		udev_device_unref(dev);
	}

	free(udev);
	free(e);

	return i;
error:
	if (udev)
		free(udev);
	if (i)
		free(i);
	if (e)
		free(e);

	return nullptr;
}

/**
 * @brief Helper function to open several event files
 * @param Array of ints, which files to open
 * @return Array of file descriptors, nullptr on failure. Caller must free.
 */
array_t *open_files(array_t *files)
{
	char path[MAX_LINE_LENGTH];
	int n, value, fd;
	array_t *fd_array = array_init(sizeof(int));
	if (!fd_array)
		return nullptr;
	for (size_t i = 0; i < files->len; i++) {
		value = ((int *)files->data)[i];
		n = snprintf(path, MAX_LINE_LENGTH, "%s%d", EVENT_PATH, value);
		if (n < 0 || n >= MAX_LINE_LENGTH) {
			perror("Could not create path");
			continue;
		}

		if ((fd = open(path, O_RDONLY)) < 0) {
			perror("Could not open file");
			continue;
		}

		if (array_push(fd_array, &fd) < 0) {
			perror("Failed to push fd");
			close(fd);
			break;
		}
	}

	return fd_array;
}

/**
 * @brief Helper function to use with array_forEach to close an array of file
 * descriptors
 * @param file to close
 */
void close_file(void *fd)
{
	close(*(int *)fd);
}

/**
 * @brief Opens the keyboard event device and sets it to non-blocking raw mode.
 * @param flags a nullptr, will return malloced.
 * @return array of results, nullptr on failure.
 */
array_t *create_keypress_setup(array_t *flag_array)
{
	int fd, flags;
	raw_mode_setup();
	array_t *keyboard = nullptr;
	array_t *fd_array = nullptr;
	if (!flag_array)
		goto error;
	keyboard = get_input_device_list("ID_INPUT_KEYBOARD", "1");
	if (!keyboard)
		goto error;

	fd_array = open_files(keyboard);
	if (!fd_array)
		goto error;

	for (size_t i = 0; i < fd_array->len; i++) {
		fd = ((int *)fd_array->data)[i];
		flags = fcntl(fd, F_GETFL, 0);

		if (flags < 0) {
			perror("F_GETFL failed");
			flags = 0;
		}

		if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
			perror("F_SETFL failed");
			continue;
		}

		if (array_push(flag_array, &flags) < 0) {
			perror("Failed to push flags");
			break;
		}
	}

	free(keyboard);
	return fd_array;
error:
	if (keyboard)
		array_free(keyboard);
	if (fd_array)
		array_free(fd_array);
	return nullptr;
}

/**
 * @brief Restores the original file status flags and closes all fds.
 * @param array of file descriptors
 * @param array of their flags
 */
void remove_keypress_setup(array_t *fd_array, array_t *flag_array)
{
	bool setflags = true;
	if (!fd_array)
		return;

	if (!flag_array || fd_array->len != flag_array->len)
		setflags = false;

	int value, flags;
	for (size_t i = 0; i < fd_array->len; i++) {
		value = ((int *)fd_array->data)[i];
		if (setflags) {
			flags = ((int *)flag_array->data)[i];
			if (fcntl(value, F_SETFL, flags) < 0)
				perror("Failed to restore flags");
		}

		close(value);
	}
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
