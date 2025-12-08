/**
 * @file worker.c
 * @brief Implements the worker thread and task management for the autoclicker.
 */
#include "worker.h"

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

/** @brief Atomic flag to control the main loop of the autoclicker threads. */
atomic_bool worker_run = false;
/** @brief The worker thread identifier. */
thrd_t worker_thread;

/** @brief The job definition for the worker thread, including task and sync
 * primitives. */
static worker_job_t job; // always auto zero-initialized!

/**
 * @brief Sets the mode and argument for the next autoclicker task.
 * @param new_mode The `autoclick_modes` to set.
 * @param i An unsigned integer argument for the mode (e.g., time in ms or click
 * count).
 */
void ac_set(autoclick_modes new_mode, unsigned i)
{
	mtx_lock(&job.mutex);
	job.task.mode = new_mode;
	job.task.i = i;
	mtx_unlock(&job.mutex);
}

/**
 * @brief Resets the autoclicker task to its default state (AUTOCLICK_NORMAL).
 */
void ac_clear()
{
	mtx_lock(&job.mutex);
	job.task.mode = AUTOCLICK_NORMAL;
	mtx_unlock(&job.mutex);
}

/**
 * @brief Sets the mouse pattern for the worker, taking ownership of the
 * provided list.
 * @details If a previous pattern exists, it is freed.
 * @param new_pattern A pointer to the head of an `slist` containing the new
 * pattern.
 */
void ac_set_pattern(struct slist *new_pattern)
{
	mtx_lock(&job.mutex);
	if (job.task.pattern)
		slist_delete(&job.task.pattern, remove_item);
	job.task.pattern = new_pattern;
	mtx_unlock(&job.mutex);
}

/**
 * @brief The main function for the worker thread.
 *
 * This thread waits on a condition variable (`job.cond`) until a task is
 * available. When signaled, it executes the task based on the `job.task.mode`.
 *
 * @param arg An unused argument.
 * @return 0 on successful completion, 1 on error.
 */
int autoclicker_worker(void *arg)
{
	(void)arg;
	while (job.running) {
		mtx_lock(&job.mutex);
		while (!job.available && job.running)
			cnd_wait(&job.cond, &job.mutex);

		if (!job.running) {
			mtx_unlock(&job.mutex);
			break;
		}

		autoclicker_task_t task = job.task;
		atomic_store(&job.available, false);
		mtx_unlock(&job.mutex);

		int fd = create_autokey_setup(cfg.key_pressed);
		if (fd < 0) {
			perror("Error creating setup");
			return 1;
		}

		switch (task.mode) {
		case AUTOCLICK_NORMAL:
			autoclick_thread(&fd);
			break;
		case AUTOCLICK_TIMER:
			thrd_t click, timer;
			thrd_create(&click, autoclick_thread, &fd);
			thrd_create(&timer, timer_thread, NULL);

			thrd_join(timer, NULL);
			thrd_join(click, NULL);
			break;
		case AUTOCLICK_AMOUNT:
			amount_thread(&fd);
			break;
		case AUTOCLICK_PATTERN:
			pattern_thread_amount(&fd);
			break;
		}
		ac_clear();
		remove_autokey_setup(fd);
	}

	return 0;
}

/**
 * @brief Initializes and starts the worker thread.
 */
void start_worker()
{
	job.task.mode = AUTOCLICK_NORMAL;
	ac_set_pattern(read_pattern());
	mtx_init(&job.mutex, mtx_plain);
	cnd_init(&job.cond);
	atomic_store(&job.available, false);
	atomic_store(&job.running, true);

	thrd_create(&worker_thread, autoclicker_worker, NULL);
}

/**
 * @brief Signals the worker thread to stop and waits for it to terminate.
 */
void stop_worker()
{
	atomic_store(&worker_run, false);
	atomic_store(&job.running, false);
	cnd_signal(&job.cond);

	thrd_join(worker_thread, NULL);

	ac_set_pattern(nullptr);
	mtx_destroy(&job.mutex);
	cnd_destroy(&job.cond);
}

/**
 * @brief Submits a task to the worker thread and signals it to start.
 * @details This function toggles the `worker_run` state. If the worker is now
 * supposed to run, it marks a job as available and signals the worker thread.
 * @return `true` if the autoclicker is now running, `false` if it was stopped.
 */
bool submit_task()
{
	atomic_store(&worker_run, !worker_run);
	if (!worker_run)
		return false;

	atomic_store(&job.available, true);
	cnd_signal(&job.cond);

	return true;
}

/**
 * @brief Writes a click event to a uinput file descriptor.
 * @param fd The file descriptor of the uinput device.
 * @param keycode The key code to press and release (e.g., `BTN_LEFT`).
 */
void write_click_event(int fd, int keycode)
{
	struct input_event click[4] = {
		{.type = EV_KEY, .code = keycode, .value = 1},
		{.type = EV_SYN, .code = SYN_REPORT, .value = 0},
		{.type = EV_KEY, .code = keycode, .value = 0},
		{.type = EV_SYN, .code = SYN_REPORT, .value = 0},
	};

	write(fd, &click, sizeof(click));
}

/**
 * @brief Writes a mouse movement event to a uinput file descriptor.
 * @param fd The file descriptor of the uinput device.
 * @param x The absolute x-coordinate.
 * @param y The absolute y-coordinate.
 */
void write_move_event(int fd, int x, int y)
{
	struct input_event move[3] = {
		{.type = EV_ABS, .code = ABS_X, .value = x},
		{.type = EV_ABS, .code = ABS_Y, .value = y},
		{.type = EV_SYN, .code = SYN_REPORT, .value = 0}};

	write(fd, &move, sizeof(move));
}

/**
 * @brief The thread function for continuous autoclicking.
 * @details This thread runs in a loop, emitting click events at the interval
 * specified by `cfg.interval_ms`, until `worker_run` becomes false.
 * @param arg A pointer to the file descriptor of the uinput device.
 * @return Always returns 0.
 */
int autoclick_thread(void *arg)
{
	int fd = *(int *)arg;

	struct timespec *wait = create_timespec(cfg.interval_ms);
	while (atomic_load(&worker_run)) {
		write_click_event(fd, cfg.key_pressed);
		thrd_sleep(wait, NULL);
	}

	free(wait);
	return 0;
}

/**
 * @brief A thread that acts as a timer, stopping the autoclicker after a set
 * duration.
 * @details It calculates the end time and runs a loop, updating a "time
 * remaining" display. The loop continues until the time expires or `worker_run`
 * becomes false. It then sets `worker_run` to false to stop the accompanying
 * `autoclick_thread`.
 * @param arg An unused argument.
 * @return Always returns 0.
 */
int timer_thread(void *arg)
{
	(void)arg;
	struct slist *head = nullptr;
	struct timespec end, now;
	mtx_lock(&job.mutex);
	struct timespec *duration = create_timespec(job.task.i);
	mtx_unlock(&job.mutex);
	clock_gettime(CLOCK_REALTIME, &end);
	end.tv_sec += duration->tv_sec;
	end.tv_nsec += duration->tv_nsec;
	double remaining = 1;
	struct timespec *wait = create_timespec(1000 / 60);

	while (atomic_load(&worker_run) && remaining > 0) {
		clock_gettime(CLOCK_REALTIME, &now);
		remaining = (end.tv_sec - now.tv_sec) +
			(end.tv_nsec - now.tv_nsec) * 1e-9;

		if (head) {
			slist_delete(&head, remove_line);
			head = nullptr;
		}
		struct printed_line *line =
			create_line("Time remaining: %.3f\n", remaining);
		head = slist_push(head, line);
		thrd_sleep(wait, NULL);
	}

	free(wait);
	atomic_store(&worker_run, false);
	if (head)
		slist_delete(&head, remove_line);
	return 0;
}

/**
 * @brief The thread function for clicking a specific number of times.
 * @details Clicks for the amount specified in `job.task.i`, with the configured
 * interval, or until `worker_run` becomes false.
 * @param arg A pointer to the file descriptor of the uinput device.
 * @return Always returns 0.
 */
int amount_thread(void *arg)
{
	int fd = *(int *)arg;
	mtx_lock(&job.mutex);
	unsigned amount = job.task.i;
	mtx_unlock(&job.mutex);

	struct timespec *wait = create_timespec(cfg.interval_ms);
	for (unsigned i = 0; i < amount && atomic_load(&worker_run); i++) {
		write_click_event(fd, cfg.key_pressed);
		thrd_sleep(wait, NULL);
	}

	free(wait);
	atomic_store(&worker_run, false);
	return 0;
}

/**
 * @brief Thread function that executes `pattern_thread` a specified number of
 * times.
 * @param arg A pointer to the file descriptor of the uinput device.
 * @return 0 on success, 1 on failure (e.g., no pattern available).
 */
int pattern_thread_amount(void *arg)
{
	mtx_lock(&job.mutex);
	int amount = job.task.i;
	if (!job.task.pattern) {
		printf("No pattern to read\n");
		return 1;
	}
	mtx_unlock(&job.mutex);

	for (int i = 0; i < amount && atomic_load(&worker_run); i++) {
		pattern_thread(arg);
	}

	return 0;
}

/**
 * @brief Thread function that replays a recorded mouse pattern once.
 * @details Iterates through the `job.task.pattern` linked list, executing each
 * movement and click event until the pattern is complete or `worker_run`
 * becomes false.
 * @param arg A pointer to the file descriptor of the uinput device.
 * @return Always returns 0.
 */
int pattern_thread(void *arg)
{
	int fd = *(int *)arg;
	mtx_lock(&job.mutex);
	struct slist *head = job.task.pattern;
	mtx_unlock(&job.mutex);

	struct Movement abs_pos = {.x = 0, .y = 0};
	struct timespec *wait = create_timespec(cfg.interval_ms);

	struct slist *head_line = nullptr;
	for (struct slist *node = head; node && atomic_load(&worker_run);
		node = node->next) {
		struct Movement *change = node->data;
		abs_pos.x += change->x;
		abs_pos.y += change->y;
		struct printed_line *line =
			create_line("x: %d, y: %d\n", abs_pos.x, abs_pos.y);
		head_line = slist_push(head_line, line);
		write_move_event(fd, abs_pos.x, abs_pos.y);
		write_click_event(fd, cfg.key_pressed);
		thrd_sleep(wait, NULL);
	}

	free(wait);
	slist_delete(&head_line, remove_line);
	return 0;
}
