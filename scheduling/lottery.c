#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "process.h"
#include "algorithm.h"

// global number of tickets
#define NUM_TICKETS 100

// a lottery type
typedef struct lottery Lottery;

// a callback to distribute tickets
typedef int (*OnDistribution)(Lottery *);

// lottery data
struct lottery {
  // an array of process indexes;
  int ticket_distribution[NUM_TICKETS];
  // a callback to distribute tickets
  OnDistribution on_distribution;
  // an array of current processes
  Queue *queue;
};

// Memory allocation
Lottery *new_lottery(OnDistribution distributer) {
  Lottery *l = malloc(sizeof(Lottery));
  if (l == NULL) {
    abort();
  }
  memset(l->ticket_distribution, 0, NUM_TICKETS * sizeof(int));
  l->on_distribution = distributer;
  l->queue = new_queue();
  return l;
}

void delete_lottery(Lottery *l) {
  if (l == NULL) {
    return;
  }
  delete_queue_list(l->queue);
  free(l);
}

static int __lottery_arrive(Process *p, void *arg) {
  if (p == NULL || arg == NULL) {
    return -1;
  }

  Lottery *l = (Lottery*) arg;

  // put on the queue
  if (queue_push_back(l->queue, p)) {
    return -1;
  }

  // redistribute with new process
  return l->on_distribution(l);
}

static int __lottery_ready(void *arg) {
  if (arg == NULL) {
    return -1;
  }
  Lottery *l = (Lottery*) arg;
  return !queue_is_empty(l->queue);
}

static Process *__lottery_get(void *arg) {
  if (arg == NULL) {
    return NULL;
  }

  Lottery *l = (Lottery *) arg;

  // generate the winning ticket
  int ticket = rand() % NUM_TICKETS;

  // get the process for the ticket
  int winner = l->ticket_distribution[ticket];

  // get the process
  return queue_remove_at(l->queue, winner);
}

static int __lottery_put(Process *p, void *arg) {
  if (arg == NULL) {
    return -1;
  }

  Lottery *l = (Lottery*) arg;

  // put back on queue
  if (queue_push_back(l->queue, p)) {
    return -1;
  }

  // and redistribute tickets
  return l->on_distribution(l);
}

// a simple lottery that divides up tickets evenly based on queue size
int __distribution_simple(Lottery *l) {
  if (l == NULL) {
    return -1;
  }

  const int qsize = queue_size(l->queue);

  // the bucket size
  int bucket = NUM_TICKETS / qsize;

  for (int i = 0, p = 0; i < NUM_TICKETS; i++) {
    l->ticket_distribution[i] = p;

    if (i == bucket) {
      p++;
      bucket += bucket;
    }
  }

  return 0;
}

// simply iterates processes to find the total service time
static int __process_service_time_iterator(Queue *queue, int index, Process *p, void *arg) {
  if (queue == NULL || index == -1 || p == NULL || arg == NULL) {
    return -1;
  }

  int *total = (int *) arg;

  *total += process_current_service_time(p);

  return QUEUE_ITERATE_NEXT;
}


// an algorithm to auto-assign tickets and a winner
int  __distribution_service_time(Lottery *l) {
  if (l == NULL) {
    return -1;
  }

  int total_service_time = 0;

  // get the total service time of the queue
  if (queue_iterate(l->queue, __process_service_time_iterator, &total_service_time)) {
    return -1;
  }

  for (int i = 0, j = -1, pos = 0; i < NUM_TICKETS; i++) {
    l->ticket_distribution[i] = j;

    if (i >= pos) {
      Process *p = queue_peek_at(l->queue, ++j);

      // get the next process ticket count
      int tickets = (((float) process_current_service_time(p) / (float) total_service_time) * (float) NUM_TICKETS);

      pos += tickets;
    }
  }

  return 0;
}


int main() {

  srand(time(0));

  // a new lottery
  Lottery *lottery = new_lottery(__distribution_service_time);

  // create the algorithm
  Algorithm *algo = new_algorithm(__lottery_arrive, __lottery_ready, __lottery_get, __lottery_put, lottery);

  // create the scheduler
  Scheduler *sched = new_scheduler(algo);

  // read the processes
  scheduler_read_processes(sched);

  // run
  int result = scheduler_run(sched);

  // cleanup
  delete_scheduler(sched);

  delete_lottery(lottery);

  return result;
}

