#include "raw_mode.h"

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_term;
struct termios tmp;

void raw_mode_setup()
{
	tcgetattr(STDIN_FILENO, &orig_term);
	atexit(enable_input); // restore on exit

	tmp = orig_term;
	tmp.c_lflag &= ~(ECHO | ICANON); // turn off echo and canonical mode
	tmp.c_cc[VMIN] = 1;		 // read one char at a time
	tmp.c_cc[VTIME] = 0;

	disable_input();
}

void disable_input()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp);
}

void enable_input()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
	tcflush(STDIN_FILENO, TCIFLUSH);
}
