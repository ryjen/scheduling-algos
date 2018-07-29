#ifndef RYJEN_OS_ALGORITHM_H
#define RYJEN_OS_ALGORITHM_H

// A callback to get a process for a time slice
typedef Process * (*OnProcessGet) (void *);

// A callback to put back a process after a time slice
typedef int (*OnProcessPut) (Process *, void *);

// A callback to handle new arrivals
typedef int (*OnProcessArrive) (Process *, void *);

// A callback to determine if there is a process ready
typedef int (*OnProcessReady) (void *);

/**
 * Allocates a new algorithm
 * @param OnProcessArrive callback for when a process arrives
 * @param OnProcessReady callback to test if a process exists for starting
 * @param OnProcessGet callback to get a process for a time slice
 * @param OnProcessPut callback to put back a process after a time slice
 * @param void* the data argument passed to callbacks
 * @return the algorithm instance
 */
Algorithm *new_algorithm(OnProcessArrive, OnProcessReady, OnProcessGet, OnProcessPut, void *);

/**
 * Alloates a new algorithm that uses a queue as an argument
 * @param OnProcessStart callback for starting a process
 * @param OnProcessNext callback for finishing a process time slice
 * @return the algorithm instance
 */
Algorithm *new_queue_algorithm(Queue *queue, OnProcessGet, OnProcessPut);

/**
 * Destroys an algorithm instance
 * @param Algorithm the algorithm instance
 */
void delete_algorithm(Algorithm *);

/**
 * Gets a process from the algorithm for a time slice
 * @param Algorithm the algorithm instance
 * @return the next process in the queue
 */
Process *algorithm_process_get(Algorithm *);

/**
 * determines what to do with a new arrival
 * @param Algorithm the algorithm instance
 * @param Process the process arrival
 * @return 0 on success, -1 on error
 */
int algorithm_process_arrive(Algorithm *, Process *);

/**
 * puts a process back into queue after a time slice, handling premption if necessary
 * @param Algorithm the algorithm instance
 * @param Process the process to prempt
 * @return 0 on success, -1 on error
 */
int algorithm_process_put(Algorithm *, Process *);

/**
 * tests if the algorithm has another process
 * @param Algorithm the algorithm instance
 * @return 1 on success, 0 on failure, -1 on error
 */
int algorithm_process_ready(Algorithm*);

#endif

