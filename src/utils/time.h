#ifndef SLEEP_H
#define SLEEP_H

struct timer_arg {
	unsigned int ms;
	struct printed_line *head;
};

extern void sleep_ms(int ms);

#endif
