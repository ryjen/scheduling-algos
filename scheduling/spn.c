#include <stdlib.h>

#include "types.h"
#include "scheduler.h"
#include "process.h"
#include "queue.h"
#include "algorithm.h"

static Process * __shortest_process_next(Queue *list) {
  if (list == NULL) {
    return NULL;
  }

  // keep the current process
  Process *p = queue_pop_front(list);

  // sort the rest
  queue_sort(list, process_compare_current_service_times);

  return p;
}

int main() {

  // create the algorithm
  Algorithm *algo = new_algorithm(__shortest_process_next);

  // create the scheduler
  Scheduler *sched = new_scheduler(algo);

  // read the processes
  scheduler_read_processes(sched);
  
  // run
  int result = scheduler_run(sched);

  // cleanup
  delete_scheduler(sched);

  return result;
}



