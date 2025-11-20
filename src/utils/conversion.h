#ifndef CONVERSION_H
#define CONVERSION_H

#include <stddef.h>

/* Takes a Linux input event code and returns a string of the meaning */
[[nodiscard]] char const *kctc(int code);

/* Takes a string of ASCII escaped characters and returns the Linux input event
 * code */
[[nodiscard]] int ascii_to_evcode(const char *buf, size_t len);

#endif
