#ifndef SLEEP_H
#define SLEEP_H

/* takes in a time in ms, returns a timestruct. Caller must free memomry */
[[nodiscard]] struct timespec *create_timespec(int ms);

#endif
