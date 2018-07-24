#ifndef RYJEN_OS_SCHEDULER_H
#define RYJEN_OS_SCHEDULER_H

/**
 * Allocates a new scheduler
 * @param Algorithm the algorithm to use
 * @return the scheduler instance
 */
Scheduler *new_scheduler(Algorithm *);

/**
 * Destroys a scheduler instance
 * @param Scheduler the scheduler instance
 */
void delete_scheduler(Scheduler *);

/**
 * Runs the scheduler by starting a producer of process arrivals
 * and a consumer of a process queue. The algorithm specified will determine
 * premption and next process in queue.
 * @param Scheduler the scheduler instance
 * @return 0 on success, -1 on error
 */
int scheduler_run(Scheduler *);

/**
 * Adds a process to the scheduler arrivals.  Is safe to call after scheduler_run()
 * has been started.
 * @param Scheduler the scheduler instance
 * @param Process the process instance
 * @return 0 on success, -1 on error
 */
int scheduler_add_process(Scheduler *, Process *);

/**
 * Prompts and reads one or more processes from standard input.
 * @param Scheduler the scheduler instance
 * @return 0 on success, -1 on error
 */
int scheduler_read_processes(Scheduler *);

/**
 * Gets the average turnaround time for the scheduler
 * @param Scheduler the scheduler instance
 * @return the average turnaround time as a floating point
 */
float scheduler_avg_turnaround_time(Scheduler *);

/**
 * Gets the average wait time for the scheduler
 * @param Scheduler the scheduler instance
 * @return the average wait time as a floating point
 */
float scheduler_avg_wait_time(Scheduler *);

/**
 * Gets the remaining service time for all processes
 * @param Scheduler the scheduler instance
 * @return the remaining service time as an integer
 */
int scheduler_total_remaining_service_time(Scheduler *);

#endif

