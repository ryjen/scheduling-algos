#ifndef RYJEN_OS_TYPES_H
#define RYJEN_OS_TYPES_H

// A scheduler type
typedef struct scheduler Scheduler;

// A queue type
typedef struct queue Queue;

// A process type
typedef struct process Process;

// An algorithm type
typedef struct algorithm Algorithm;

// A callback for algorithms
typedef Process * (*Callback) (Queue *);

// A comparator for queues
typedef int (*Comparator) (Process *, Process *);

// An iterator for queues
typedef int (*Iterator) (Queue *, int, Process *, void *);

#endif

