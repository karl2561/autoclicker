#include "time.h"

#include <stdlib.h>
#include <threads.h>
#include <time.h>

struct timespec *create_timespec(int ms)
{
	struct timespec *ts = malloc(sizeof(struct timespec));
	ts->tv_sec = ms / 1000;
	ts->tv_nsec = (ms % 1000) * 1'000'000L;
	return ts;
}
