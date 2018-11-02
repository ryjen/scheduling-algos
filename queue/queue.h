#ifndef RYJEN_OS_QUEUE_H
#define RYJEN_OS_QUEUE_H

typedef struct queue Queue;

// A comparator for queues
typedef int (*Comparator) (void *, void *);

// An iterator for queues
typedef int (*Iterator) (Queue *, int, void *, void *);

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
 * Destroys a queue and all its items.
 * @param Queue the queue instance
 */
void delete_queue_list(Queue *);

/**
 * Destroys a queue and all its item values
 * @param Queue the queue instance
 */
void delete_queue_data(Queue *);

/**
 * Pushes a void on the back of the queue
 * @param Queue the queue instance
 * @param void the void instance
 * @return 0 on success, -1 on error
 */
int queue_push_back(Queue *, void *);

/**
 * Pushes a void on the front of the queue
 * @param Queue the queue instance
 * @param void the void instance
 * @return 0 on success, -1 on error
 */
int queue_push_front(Queue *, void *);

/**
 * Pops a void from the front of a queue
 * @param Queue the queue instance
 * @return the void instance
 */
void *queue_pop_front(Queue *);

/**
 * Pops a void from the back of a queue
 * @param Queue the queue instance
 * @return the void instance
 */
void *queue_pop_back(Queue *);

/**
 * Peeks at the front of a queue
 * @param Queue the queue instance
 * @return the void at the front of the queue
 */
void *queue_peek_front(Queue *);

/**
 * Peeks at the back of a queue
 * @param Queue the queue instance
 * @return the void at the back of the queue
 */
void *queue_peek_back(Queue *);

/**
 * Peeks at an index in the queue
 * @param Queue the queue instance
 * @param int the index of the void
 * @return the void or NULL if not found
 */
void *queue_peek_at(Queue *, int);

/**
 * Removes a void from the queue
 * @param Queue the queue instance
 * @param void the void to remove
 * @return 0 on success, 1 on failure, -1 on error
 */
int queue_remove(Queue *, void *);

/**
 * Removes a void at an index from the queue
 * @param Queue the queue instance
 * @param int the index of the void
 * @return the removed void on success, null otherwise
 */
void* queue_remove_at(Queue *, int);

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

