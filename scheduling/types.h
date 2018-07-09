#ifndef RYJEN_OS_TYPES_H
#define RYJEN_OS_TYPES_H

typedef struct scheduler Scheduler;

typedef struct queue Queue;

typedef struct queue_item QueueItem;

typedef struct process Process;

typedef Process * (*Algorithm) (Queue *);

typedef int (*Comparator) (QueueItem *, QueueItem *);

#endif

