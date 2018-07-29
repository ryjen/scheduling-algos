#include <stdlib.h>

#include "types.h"
#include "scheduler.h"
#include "process.h"
#include "queue.h"
#include "algorithm.h"

static Process * __spn_get(void *arg) {
  if (arg == NULL) {
    return NULL;
  }

  Queue *list = (Queue*) arg;

  // keep the current process
  Process *p = queue_pop_front(list);

  // sort the rest for next time
  queue_sort(list, process_compare_current_service_times);

  return p;
}

static int __spn_put(Process *p, void *arg) {
  if (p == NULL || arg == NULL) {
    return -1;
  }

  Queue *q = (Queue *) arg;

  // non-premptive so keep at front
  return queue_push_front(q, p);
}

int main() {

  // data is a simple queue
  Queue *queue = new_queue();

  // create the algorithm
  Algorithm *algo = new_queue_algorithm(queue, __spn_get, __spn_put);

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



