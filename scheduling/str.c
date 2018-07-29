#include <stdlib.h>

#include "types.h"
#include "scheduler.h"
#include "process.h"
#include "queue.h"
#include "algorithm.h"

static Process *__str_start(void *arg) {
  if (arg == NULL) {
    return NULL;
  }

  Queue *q = (Queue *) arg;

  // sort all the processes with current service times
  queue_sort(q, process_compare_current_service_times);

  // return the first
  return queue_pop_front(q);
}

static int __str_finish(Process *p, void *arg) {
  if (p == NULL || arg == NULL) {
    return -1;
  }

  Queue *q = (Queue *) arg;

	Process *next = queue_peek_front(q);

	// next process least service time...
	if (process_compare_current_service_times(next, p) < 0) {
	  
    // prempt
    if (process_prempt(p)) {
      return -1;
    }

    // put new process on back of queue
		return queue_push_back(q, p);
	}

	// keep as current
  return queue_push_front(q, p);
}

int main() {

  // data is just a queue
  Queue *queue = new_queue();

  // create the algorithm
  Algorithm *algo = new_queue_algorithm(queue, __str_start, __str_finish);

  // create the scheduler
  Scheduler *sched = new_scheduler(algo);
 
  // read the processes
  scheduler_read_processes(sched);

  // run
  int result = scheduler_run(sched);

  delete_scheduler(sched);

  delete_queue(queue);

  return result;
}



