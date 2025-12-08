/**
 * @file worker.h
 * @brief Defines the worker thread and task management for the autoclicker.
 *
 * This file contains the structures, enums, and functions for managing the
 * background worker thread that performs the autoclicking actions.
 */
#ifndef WORKER_H
#define WORKER_H

#include "create_pattern.h"
#include "utils/config.h"
#include "utils/handle_file.h"
#include "utils/print_line.h"
#include "utils/sleep.h"
#include "utils/slist.h"

#include <threads.h>

/** @brief Atomic flag to control the main loop of the autoclicker threads. */
extern atomic_bool worker_run;

/**
 * @enum autoclick_modes
 * @brief Defines the different operating modes for the autoclicker worker.
 */
typedef enum {
	AUTOCLICK_NORMAL,   /**< Continuous clicking. */
	AUTOCLICK_TIMER,    /**< Click for a specified duration. */
	AUTOCLICK_AMOUNT,   /**< Click a specified number of times. */
	AUTOCLICK_PATTERN,  /**< Replay a recorded mouse pattern. */
} autoclick_modes;

/**
 * @struct autoclicker_task_t
 * @brief Represents a task for the autoclicker worker.
 */
typedef struct {
	struct slist *pattern; /**< A linked list representing a mouse pattern. */
	unsigned i;            /**< A parameter for the task (e.g., duration or amount). */
	autoclick_modes mode;  /**< The operating mode for the task. */
} autoclicker_task_t;

/**
 * @struct worker_job_t
 * @brief Manages the state and synchronization for the worker thread.
 */
typedef struct {
	autoclicker_task_t task; /**< The task to be executed. */
	mtx_t mutex;             /**< Mutex for thread-safe access to the job data. */
	cnd_t cond;              /**< Condition variable to signal task availability. */
	atomic_bool available;   /**< Flag indicating if a new task is available. */
	atomic_bool running;     /**< Flag to control the worker's main loop. */
} worker_job_t;

/**
 * @brief Sets the mode and argument for the next autoclicker task.
 * @param new_mode The `autoclick_modes` to set.
 * @param i An unsigned integer argument for the mode (e.g., time in ms or click count).
 */
void ac_set(autoclick_modes new_mode, unsigned i);

/**
 * @brief Resets the autoclicker task to its default state (AUTOCLICK_NORMAL).
 */
void ac_clear();

/**
 * @brief Sets the mouse pattern for the worker, taking ownership of the provided list.
 * @details If a previous pattern exists, it is freed.
 * @param new_pattern A pointer to the head of an `slist` containing the new pattern.
 */
void ac_set_pattern(struct slist *new_pattern);

/**
 * @brief The main function for the worker thread.
 * @param arg An unused argument.
 * @return 0 on successful completion.
 */
int autoclicker_worker(void *arg);

/**
 * @brief Initializes and starts the worker thread.
 */
void start_worker();

/**
 * @brief Signals the worker thread to stop and waits for it to terminate.
 */
void stop_worker();

/**
 * @brief Submits a task to the worker thread and signals it to start.
 * @return `true` if the autoclicker is now running, `false` if it was stopped.
 */
bool submit_task();

/**
 * @brief The thread function for continuous autoclicking.
 * @param arg A pointer to the file descriptor of the uinput device.
 * @return Always returns 0.
 */
extern int autoclick_thread(void *arg);

/**
 * @brief The thread function for timed autoclicking. It stops the main `autoclick_thread` after a duration.
 * @param arg An unused argument.
 * @return Always returns 0.
 */
extern int timer_thread(void *arg);

/**
 * @brief The thread function for clicking a specific number of times.
 * @param arg A pointer to the file descriptor of the uinput device.
 * @return Always returns 0.
 */
extern int amount_thread(void *arg);

/**
 * @struct pattern_arg_t
 * @brief Arguments for pattern-based clicking threads. (Currently unused).
 */
typedef struct {
	struct slist *pattern;
	int fd;
} pattern_arg_t;

/**
 * @brief Thread function that executes `pattern_thread` a specified number of times.
 * @param arg A pointer to the file descriptor of the uinput device.
 * @return 0 on success, 1 on failure (e.g., no pattern available).
 */
int pattern_thread_amount(void *arg);

/**
 * @brief Thread function that replays a recorded mouse pattern once.
 * @param arg A pointer to the file descriptor of the uinput device.
 * @return Always returns 0.
 */
extern int pattern_thread(void *arg);

#endif
