#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "algorithm.h"

static Process * __round_robin(Queue *list) {
  if (list == NULL) {
    return NULL;
  }

  // return the first in the queue and let the 
  // quantum prempt
  return queue_pop_front(list);
}

int main(int argc, char *argv[]) {

  int q = 3;

  if (argc > 1) {
    q = atoi(argv[1]);
  
    if (q < 0) {
      q = 1;
    }
  }

  // create the algorithm
  Algorithm *algo = new_algorithm(__round_robin);

  algorithm_set_quantum(algo, q);

  // create the scheduler
  Scheduler *sched = new_scheduler(algo);

  // read the processes
  scheduler_read_processes(sched);

  // run
  int result = scheduler_run(sched);

  delete_scheduler(sched);

  return result;
}

