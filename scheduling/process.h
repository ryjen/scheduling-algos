#ifndef RYJEN_OS_PROCESS_H
#define RYJEN_OS_PROCESS_H

#define PROCESS_ERROR -1
#define PROCESS_END    0
#define PROCESS_ALIVE  1
#define PROCESS_PREMPT 2

/**
 * Allocates a new process
 * @return the process instance
 */
Process *new_process();

/**
 * Destroys a process instance
 * @param Process the process instance
 */
void delete_process(Process *);

/**
 * Gets the name of the process
 * @param Process the process instance
 * @return the name as a string
 */
const char *process_name(Process *);

/**
 * sleeps for microseconds
 * @param int the value in microseconds
 * @return 0 on success, -1 on error
 */
int microsleep(int);

/**
 * A comparator for process current service times
 * @param Process the first process
 * @param Process the second process
 * @return 0 if the times are equal, a negative value if the first
 *         process is less than, a positive value if the second
 *         process is less than
 */
int process_compare_current_service_times(Process *, Process *);

/**
 * A comparator for process current arrival times
 * @param Process the first process
 * @param Process the second process
 * @return 0 if the times are equal, a negative value if the first
 *         process is less than, a positive value if the second
 *         process is less than
 */
int process_compare_arrival_times(Process *, Process *);

/**
 * Runs the process
 * @param Process the process instance
 * @return 0 on success, -1 on error
 */
int process_run(Process *);

/**
 * Gets the process arrival time
 * @param Process the process instance
 * @return the arrival time as an integer
 */
int process_arrival_time(Process *);

/**
 * Gets the process current arrival time. Measured based on the
 * amount of times this process has been serviced.
 * @param Process the process instance
 * @return the current arrival time as an integer
 */
int process_current_arrival_time(Process *);

/**
 * Gets the process service time.
 * @param Process the process instance
 * @return the service time as an integer
 */
int process_service_time(Process *);

/**
 * Gets the process current service time. Measured based on the
 * amount of time this process has been serviced.
 * @param Process the process instance
 * @return the current service time as an integer
 */
int process_current_service_time(Process *);

/**
 * Gets the process completion time.
 * @param Process the process instance
 * @return the completion time as an integer
 */
int process_completion_time(Process *);

/**
 * Sets the arrival time for a process
 * @param Process the process instance
 * @param in the arrival time value
 * @return 0 on success, -1 on error
 */
int process_set_arrival_time(Process *, int);

/**
 * Sets the process service time
 * @param Process the process instance
 * @param int the service time value
 * @return 0 on success, -1 on error
 */
int process_set_service_time(Process *, int);

/**
 * Sets the process completion time
 * @param Process the process instance
 * @param int the completion time value
 * @return 0 on success, -1 on error
 */
int process_set_completion_time(Process *, int);

/**
 * Finishes a process. Sets the internal state to not active.
 * @param Process the process instance
 * @return 0 on success, -1 on error
 */
int process_prempt(Process *);

/**
 * Gets the process quantum, or how long the process has been running
 * @param Process the process instance
 * @return the process quantum as an integer
 */
int process_current_tick(Process *);

#endif

