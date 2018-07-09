#include <stdlib.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"

static Process * __first_come_first_serve(Queue *list) {
  if (list == NULL) {
    return NULL;
  }

  return queue_pop_front(list);
}

int main() {

  Scheduler *sched = new_scheduler();

  scheduler_set_algorithm(sched, __first_come_first_serve);

  return scheduler_run(sched);
}

