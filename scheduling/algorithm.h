#ifndef RYJEN_OS_ALGORITHM_H
#define RYJEN_OS_ALGORITHM_H

// A callback to find the next process in queue
typedef Process * (*OnProcessStart) (void *);

// A callback to continue a process
typedef int (*OnProcessFinish) (Process *, void *);

// A callback to handle new arrivals
typedef int (*OnProcessArrive) (Process *, void *);

typedef int (*OnProcessExists) (void *);

/**
 * Allocates a new algorithm
 * @param OnProcessArrive callback for when a process arrives
 * @param OnProcessExists callback to test if a process exists for starting
 * @param OnProcessStart callback to start a process
 * @param OnProcessFinish callback to finish a process
 * @param void* the data argument passed to callbacks
 * @return the algorithm instance
 */
Algorithm *new_algorithm(OnProcessArrive, OnProcessExists, OnProcessStart, OnProcessFinish, void *);

/**
 * Alloates a new algorithm that uses a queue as an argument
 * @param OnProcessStart callback for starting a process
 * @param OnProcessNext callback for finishing a process time slice
 * @return the algorithm instance
 */
Algorithm *new_queue_algorithm(Queue *queue, OnProcessStart, OnProcessFinish);

/**
 * Destroys an algorithm instance
 * @param Algorithm the algorithm instance
 */
void delete_algorithm(Algorithm *);

/**
 * Runs the algorithm, typically by calling the callback
 * @param Algorithm the algorithm instance
 * @param Queue an instance of the queue being run on
 * @return the next process in the queue
 */
Process *algorithm_process_start(Algorithm *);

/**
 * determines what to do with a new arrival
 * @param Algorithm the algorithm instance
 * @param Process the process arrival
 * @return 0 on success, -1 on error
 */
int algorithm_process_arrive(Algorithm *, Process *);

/**
 * determines how to prempt a process if it has not finished
 * @param Algorithm the algorithm instance
 * @param Process the process to prempt
 * @return 0 on success, -1 on error
 */
int algorithm_process_finish(Algorithm *, Process *);

/**
 * tests if the algorithm has another process
 * @param Algorithm the algorithm instance
 * @return 1 on success, 0 on failure, -1 on error
 */
int algorithm_process_exists(Algorithm*);

#endif

