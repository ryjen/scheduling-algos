#include <stdlib.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "algorithm.h"

// start a process in the queue
static Process * __fcfs_get(void *arg) {
  if (arg == NULL) {
    return NULL;
  }

  Queue *queue = (Queue *) arg;

  // return the first of the unmodified queue
  return queue_pop_front(queue);
}

// finish a proces
static int __fcfs_put(Process *p, void *arg) {
  if (arg == NULL || p == NULL) {
    return -1;
  }

  Queue *queue = (Queue *) arg;

  // process should never prempt so put back on front
  return queue_push_front(queue, p);
}

int main() {

  Queue *queue = new_queue();

  // create the algorithm
  Algorithm *algo = new_queue_algorithm(queue, __fcfs_get, __fcfs_put);

  // create the scheduler
  Scheduler *sched = new_scheduler(algo);

  // read the processes
  scheduler_read_processes(sched);

  // run
  int result = scheduler_run(sched);

  // cleanup
  delete_scheduler(sched);

  delete_queue(queue);

  return result;
}

