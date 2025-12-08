/**
 * @file conversion.h
 * @brief Provides functions for converting between different key code representations.
 */
#ifndef CONVERSION_H
#define CONVERSION_H

#include <stddef.h>

/**
 * @brief Converts a Linux input event code to a human-readable string.
 * @param code The input event code (e.g., `KEY_A`).
 * @return A constant string representing the key, or "UNKNOWN" if not found.
 */
[[nodiscard]] char const *kctc(int code);

/**
 * @brief Converts an ASCII character or escape sequence to a Linux input event code.
 * @param buf Pointer to the character buffer.
 * @param len Length of the buffer.
 * @return The corresponding input event code, or -1 if no mapping is found.
 */
[[nodiscard]] int ascii_to_evcode(const char *buf, size_t len);

/**
 * @brief Converts a Linux input event code to its ASCII character equivalent.
 * @param code The input event code.
 * @return The corresponding ASCII character code, or 0 if no direct equivalent exists.
 */
int evcode_to_ascii(int code);

#endif
