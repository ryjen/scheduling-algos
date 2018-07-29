#ifndef RYJEN_OS_QUEUE_H
#define RYJEN_OS_QUEUE_H

// A comparator for queues
typedef int (*Comparator) (Process *, Process *);

// An iterator for queues
typedef int (*Iterator) (Queue *, int, Process *, void *);

/**
 * Allocates a new queue
 * @return the queue instance
 */
Queue *new_queue();

/**
 * Destroys a queue instance
 * @param Queue the queue instance
 */
void delete_queue(Queue *);

/**
 * Destroys a queue and all its contents.
 * @param Queue the queue instance
 */
void delete_queue_list(Queue *);

/**
 * Copies a queue (shallow)
 * @param Queue the queue instance
 * @return a copy of the queue
 */
Queue *queue_copy(Queue *);

/**
 * Pushes a process on the back of the queue
 * @param Queue the queue instance
 * @param Process the process instance
 * @return 0 on success, -1 on error
 */
int queue_push_back(Queue *, Process *);

/**
 * Pushes a process on the front of the queue
 * @param Queue the queue instance
 * @param Process the process instance
 * @return 0 on success, -1 on error
 */
int queue_push_front(Queue *, Process *);

/**
 * Pops a process from the front of a queue
 * @param Queue the queue instance
 * @return the process instance
 */
Process *queue_pop_front(Queue *);

/**
 * Pops a process from the back of a queue
 * @param Queue the queue instance
 * @return the process instance
 */
Process *queue_pop_back(Queue *);

/**
 * Peeks at the front of a queue
 * @param Queue the queue instance
 * @return the process at the front of the queue
 */
Process *queue_peek_front(Queue *);

/**
 * Peeks at the back of a queue
 * @param Queue the queue instance
 * @return the process at the back of the queue
 */
Process *queue_peek_back(Queue *);

/**
 * Peeks at an index in the queue
 * @param Queue the queue instance
 * @param int the index of the process
 * @return the process or NULL if not found
 */
Process *queue_peek_at(Queue *, int);

/**
 * Removes a process from the queue
 * @param Queue the queue instance
 * @param Process the process to remove
 * @return 0 on success, 1 on failure, -1 on error
 */
int queue_remove(Queue *, Process *);

/**
 * Removes a process at an index from the queue
 * @param Queue the queue instance
 * @param int the index of the process
 * @return the removed process on success, null otherwise
 */
Process* queue_remove_at(Queue *, int);

/**
 * Sorts a queue
 * NOTE: uses merge sort O(n log n)
 * @param Queue the queue instance (mutable)
 * @param Comparator how to compare contents
 * @return 0 on success, -1 on error
 */
int queue_sort(Queue *, Comparator);

/**
 * Tests if a queue is empty
 * @param Queue the queue instance
 * @return positive if true, 0 if false
 */
int queue_is_empty(Queue *);

/**
 * Gets the size of a queue
 * @param Queue the queue instance
 * @return the number of items in the queue
 */
int queue_size(Queue *);

/**
 * Iterates over a queue using a callback for each item
 * @param Queue the queue instance
 * @param Iterator the callback
 * @param void* the iterator parameter
 * @return 0 on success, -1 on error
 */
int queue_iterate(Queue *queue, Iterator iterator, void *);

// iterator errored
#define QUEUE_ITERATE_ERROR   -1
// iterator wants to finish
#define QUEUE_ITERATE_FINISH  0
// iterator wants the next item
#define QUEUE_ITERATE_NEXT    1

#endif

