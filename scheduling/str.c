#include <stdlib.h>

#include "types.h"
#include "scheduler.h"
#include "process.h"
#include "queue.h"
#include "algorithm.h"

static Process *__shortest_time_remaining(Queue *list) {
  if (list == NULL) {
    return NULL;
  }

  // sort all processes
  queue_sort(list, process_compare_current_service_times);

  // return the first
  return queue_pop_front(list);
}

int main() {

  // create the algorithm
  Algorithm *algo = new_algorithm(__shortest_time_remaining);

  // this is a premptive algorithm
  algorithm_set_premptive(algo, 1);

  // create the scheduler
  Scheduler *sched = new_scheduler(algo);
 
  // read the processes
  scheduler_read_processes(sched);

  // run
  int result = scheduler_run(sched);

  delete_scheduler(sched);

  return result;
}



