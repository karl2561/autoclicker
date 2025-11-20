#include "raw_mode.h"

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_term;

void raw_mode_setup()
{
	struct termios tmp;
	tcgetattr(STDIN_FILENO, &orig_term);
	atexit(enable_input);

	tmp = orig_term;
	tmp.c_lflag &= ~(ECHO | ICANON);
	tmp.c_cc[VMIN] = 1;
	tmp.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp);
}

void enable_input()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
	tcflush(STDIN_FILENO, TCIFLUSH);
}
