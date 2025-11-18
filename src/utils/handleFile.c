#include "handleFile.h"
#include "raw_mode.h"

#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define PATH_UINPUT "/dev/uinput"
#define PATH_KEYEV "/dev/input/event3"

int file_open(char *path, int flags)
{
	int fd = open(path, flags);
	if (fd < 0)
		return 0;

	return fd;
}

void file_close(int fd)
{
	close(fd);
	return;
}

int create_autokey_setup(int keycode)
{
	int fd = file_open(PATH_UINPUT, O_WRONLY | O_NONBLOCK);
	if (!fd)
		return 0;

	ioctl(fd, UI_SET_EVBIT, EV_KEY);
	ioctl(fd, UI_SET_RELBIT, EV_REL);
	ioctl(fd, UI_SET_KEYBIT, keycode);
	ioctl(fd, UI_SET_EVBIT, EV_SYN);

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

void remove_autokey_setup(int fd)
{
	ioctl(fd, UI_DEV_DESTROY);
	file_close(fd);
}

int create_keypress_setup(int *flags)
{
	int fd = file_open(PATH_KEYEV, O_RDONLY);
	if (!fd) {
		return 0;
	}
	enable_raw_mode();

	*flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, *flags | O_NONBLOCK);

	return fd;
}

void remove_keypress_setup(int fd, int flags)
{
	fcntl(fd, F_SETFL, flags);
	disable_raw_mode();
	file_close(fd);
}
