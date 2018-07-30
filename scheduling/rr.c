#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "process.h"
#include "algorithm.h"

typedef struct round_robin RR;

struct round_robin {
  int quantum;
  Queue *queue;
};

RR *new_round_robin(int quantum) {
  RR *v = malloc(sizeof(RR));

  if (v == NULL) {
    abort();
  }

  v->quantum = quantum;
  v->queue = new_queue();
  return v;
}

void delete_round_robin(RR *value) {
  if (value == NULL) {
    return;
  }

  delete_queue_list(value->queue);
  free(value);
}

static int __rr_arrive(Process *p, void *arg) {
  if (p == NULL || arg == NULL) {
    return -1;
  }

  RR *rr = (RR*) arg;

  return queue_push_back(rr->queue, p);
}

static int __rr_ready(void *arg) {
  if (arg == NULL) {
    return -1;
  }
  RR *data = (RR*) arg;
  return !queue_is_empty(data->queue);
}

static Process * __rr_get(void *arg) {
  if (arg == NULL) {
    return NULL;
  }

  RR *rr = (RR*) arg;

  return queue_pop_front(rr->queue);
}

static int __rr_put(Process *p, void *arg) {
  if (arg == NULL) {
    return -1;
  }

  RR *rr = (RR*) arg;

  // process has not reached the quantum...
  if (process_current_tick(p) < rr->quantum) {
    // keep it as current
    return queue_push_front(rr->queue, p);
  }

  // prempt
  if (process_prempt(p) == -1) {
    return -1;
  }

  // and put on back of queue
  return queue_push_back(rr->queue, p);
}

int main(int argc, char *argv[]) {

  int quantum = 3;

  if (argc > 1) {
    quantum = atoi(argv[1]);

    if (quantum < 0) {
      puts("invalid argument");
      return 1;
    }
  }

  RR *data = new_round_robin(quantum);

  // create the algorithm
  Algorithm *algo = new_algorithm(__rr_arrive, __rr_ready, __rr_get, __rr_put, data);

  // create the scheduler
  Scheduler *sched = new_scheduler(algo);

  // read the processes
  scheduler_read_processes(sched);

  // run
  int result = scheduler_run(sched);

  delete_scheduler(sched);

  delete_round_robin(data);

  return result;
}

