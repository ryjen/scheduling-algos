#ifndef RYJEN_OS_ALGORITHM_H
#define RYJEN_OS_ALGORITHM_H

/**
 * Allocates a new algorithm
 * @param Callback the callback function to custom the next in queue
 * @return the algorithm instance
 */
Algorithm *new_algorithm(Callback);

/**
 * Destroys an algorithm instance
 * @param Algorithm the algorithm instance
 */
void delete_algorithm(Algorithm *);

/**
 * Sets the quantum for an algorithm
 * @param Algorithm the algorithm instance
 * @param int the quantum value
 * NOTE: implicitly sets premptive on positive value
 * @return 0 on success, -1 on error
 */
int algorithm_set_quantum(Algorithm *, int);

/**
 * Sets the premptive flag for an algorithm
 * @param Algorithm the algorithm instance
 * @param int the flag value, positive if true
 * @return 0 on success, -1 on error
 */
int algorithm_set_premptive(Algorithm *, int);

/**
 * Gets the premptive value for an algorithm instance
 * @param Algorithm the algorithm instance
 * @return 0 on success, -1 on error
 */
int algorithm_premptive(Algorithm *);

/**
 * Gets the quantum value for an algorithm instance
 * @param Algorithm the algorithm instance
 * @return 0 on success, -1 on error
 */
int algorithm_quantum(Algorithm *);

/**
 * Runs the algorithm, typically by calling the callback
 * @param Algorithm the algorithm instance
 * @param Queue an instance of the queue being run on
 * @return the next process in the queue
 */
Process *algorithm_run(Algorithm *, Queue *);

#endif

