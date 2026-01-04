/**
 * @file constants.h
 * @brief Defines compile-time constants used throughout the application.
 *
 * This file contains paths to system devices, configuration files, and other
 * fixed values.
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

/* Sets the max length for a line, used for reading config, or input */
#define MAX_LINE_LENGTH 128

/* Ascii code for Esc or ^[ */
#define ESC_KEY 27

/* Sets the screenheight, shouldn't be hardcoded */
#define SCREENHEIGHT 1080

/* Sets the screenwidth, shouldn't be hardcoded */
#define SCREENWIDTH 1920

/* Sets the x coordinate to which the pattern is played from (relatively) */
#define X_POS_START 0

/* Sets the y coordinate to which the pattern is played from (relatively) */
#define Y_POS_START 0

#endif
