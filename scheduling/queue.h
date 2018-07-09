#ifndef RYJEN_OS_QUEUE_H
#define RYJEN_OS_QUEUE_H

Queue *new_queue();

void delete_queue_list(Queue *);

int queue_push_back(Queue *, Process *);

Process *queue_pop_front(Queue *);

Process *queue_peek_front(Queue *);

int queue_sort(Queue *);

#endif

