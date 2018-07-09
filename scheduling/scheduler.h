#ifndef RYJEN_OS_SCHEDULER_H
#define RYJEN_OS_SCHEDULER_H

Scheduler *new_scheduler();

void delete_scheduler(Scheduler *);

int scheduler_set_algorithm(Scheduler *, Algorithm);

int scheduler_run(Scheduler *);

int scheduler_queue_empty(Scheduler *);

int scheduler_add_process(Scheduler *, Process *);

#endif

