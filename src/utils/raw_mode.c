#include "raw_mode.h"

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_term;
struct termios tmp;

void disable_raw_mode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
}

void enable_raw_mode()
{
	tcgetattr(STDIN_FILENO, &orig_term);
	atexit(disable_raw_mode); // restore on exit

	tmp = orig_term;
	tmp.c_lflag &= ~(ECHO | ICANON); // turn off echo and canonical mode
	tmp.c_cc[VMIN] = 1;		 // read one char at a time
	tmp.c_cc[VTIME] = 0;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp);
}

void enable_input()
{
	tmp.c_lflag |= ECHO;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp);
	tcflush(STDIN_FILENO, TCIFLUSH);
}

void disable_input()
{
	tmp.c_lflag |= ECHO;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tmp);
}
