#include "slist.h"

#include <threads.h>

void sleep_ms(int ms)
{
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1'000'000L;
	thrd_sleep(&ts, NULL);
}
