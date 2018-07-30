#include <stdlib.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "algorithm.h"
#include "process.h"

typedef struct mlfq MLFQ;

struct mlfq {
  // an array of queues
  Queue **queues;
  // size of the array
  int size;
  // current queue index
  int current_index;
  // an array of queue quantums
  int *quantums;
};

MLFQ *new_mlfq(int size, int initial_quantum) {
  MLFQ *val = (MLFQ*) malloc(sizeof(MLFQ));

  if (val == NULL) {
    abort();
  }

  val->queues = calloc(size, sizeof(Queue*));
  val->quantums = calloc(size, sizeof(int));
  val->size = size;
  val->current_index = 0;

  for (int i = 0; i < size; i++) {
    val->queues[i] = new_queue();
    val->quantums[i] = initial_quantum;
    // increment the quantum for each queue TODO: use callback
    initial_quantum += initial_quantum;
  }
  return val;
}

void delete_mlfq(MLFQ *q) {
  if (q == NULL) {
    return;
  }

  for (int i = 0; i < q->size; i++) {
    delete_queue_list(q->queues[i]);
  }

  free(q->queues);

  free(q->quantums);

  free(q);
}

static int __mlfq_arrive(Process *p, void *arg) {
  if (p == NULL || arg == NULL) {
    return -1;
  }

  MLFQ *data = (MLFQ*) arg;

  // always put on back of top level queue
  return queue_push_back(data->queues[0], p);
}

static int __mlfq_ready(void *arg) {
  if (arg == NULL) {
    return -1;
  }
  MLFQ *data = (MLFQ *) arg;

  // any non empty queue
  for (int i = 0; i < data->size; i++) {
    if (!queue_is_empty(data->queues[i])) {
      return 1;
    }
  }
  return 0;
}

static Process *__mlfq_get(void *arg) {
  if (arg == NULL) {
    return NULL;
  }

  MLFQ *data = (MLFQ*) arg;

  // TODO: promote up from lower level queues if needed

  // first non empty queue from the top level
  for (int i = 0; i < data->size; i++) {
    if (!queue_is_empty(data->queues[i])) {
      // track the current index (hacky)
      data->current_index = i;
      // return the first process in FIFO queue
      return queue_pop_front(data->queues[i]);
    }
  }
  return NULL;
}

static int __mlfq_put(Process *p, void *arg) {
  if (p == NULL || arg == NULL) {
    return -1;
  }

  MLFQ *data = (MLFQ*) arg;

  // validate current index
  if (data->current_index < 0 || data->current_index >= data->size) {
    return -1;
  }

  // get the current queue
  Queue *q = data->queues[data->current_index];

  // process has not reached the quantum...
  if (process_current_tick(p) < data->quantums[data->current_index]) {
    // keep on the current queue
    return queue_push_front(q, p);
  }

  // otherwise, prempt
  if (process_prempt(p) == -1) {
    return -1;
  }

  // if there is another queue, demote the process
  int next_queue = data->current_index + 1;

  if (next_queue < data->size) {
    q = data->queues[next_queue];
  }

  // and put on back of FIFO queue
  return queue_push_back(q, p);
}

int main() {

  MLFQ *data = new_mlfq(3, 3);

  // create the algorithm
  Algorithm *algo = new_algorithm(__mlfq_arrive, __mlfq_ready, __mlfq_get, __mlfq_put, data);

  // create the scheduler
  Scheduler *sched = new_scheduler(algo);

  // read the processes
  scheduler_read_processes(sched);

  // run
  int result = scheduler_run(sched);

  // cleanup
  delete_scheduler(sched);

  delete_mlfq(data);

  return result;
}

