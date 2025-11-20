#ifndef RAWMODE_H
#define RAWMODE_H

/* Enables raw mode, disables output to terminal, reading one char at a time.
 * Restores settings on quit */
extern void raw_mode_setup();

/* Enables input back to it's original, used to restore settings atexit */
void enable_input();

#endif
