/**
 * @file constants.h
 * @brief Defines compile-time constants used throughout the application.
 *
 * This file contains paths to system devices, configuration files, and other
 * fixed values.
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PATH_USB_MOUSE "/dev/input/by-id/usb-1bcf_USB_Optical_Mouse-event-mouse"
#define PATH_UINPUT "/dev/uinput"
#define PATH_KEYEV "/dev/input/event3"

/* Path is relative to the autoclicker executable */
#define PATH_PATTERN_SAVE ".pattern"
#define PATH_CONFIG_SAVE ".config"

#define MAX_LINE_LENGTH 128
#define ESC_KEY 27

#define SCREENHEIGHT 1080
#define SCREENWIDTH 1920

#endif
