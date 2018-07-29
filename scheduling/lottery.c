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

// simply iterates processes to find the total service time
static int __process_service_time_iterator(Queue *queue, int index, Process *p, void *arg) {
  if (queue == NULL || index == -1 || p == NULL || arg == NULL) {
    return -1;
  }
  
  int *total = (int *) arg;
 
  *total += process_current_service_time(p);

  return QUEUE_ITERATE_NEXT;
}

static int __lottery_arrive(Process *p, void *arg) {
  if (p == NULL || arg == NULL) {
    return -1;
  }

  Lottery *l = (Lottery*) arg;

  if (queue_push_back(l->queue, p)) {
    return -1;
  }
  
  // redistribute with new process
  return l->on_distribution(l);
}

static int __lottery_exists(void *arg) {
  if (arg == NULL) {
    return -1;
  }
  Lottery *l = (Lottery*) arg;
  return !queue_is_empty(l->queue);
}

// a simple lottery that divides up tickets evenly based on queue size
int __lottery_auto_assign_distribution(Lottery *l) {
  if (l == NULL) {
    return -1;
  }

  const int qsize = queue_size(l->queue);
 
  // the bucket size
  int bucket = NUM_TICKETS / qsize;
 
  memset(l->ticket_distribution, 0, NUM_TICKETS * sizeof(int));

  for (int i = 0, p = 0; i < NUM_TICKETS; i++) {
    l->ticket_distribution[i] = p;  

    if (i == bucket) {
      bucket += bucket;
      p++;
    }
  }

  return 0;
}

// an algorithm to auto-assign tickets and a winner
int  __lottery_service_time_distribution(Lottery *l) {
  if (l == NULL) {
    return -1;
  }

  const int qsize = queue_size(l->queue);

  int total_service_time = 0;

  if (queue_iterate(l->queue, __process_service_time_iterator, &total_service_time)) {
    return -1;
  }

  // determine the ticket for a process and check for a winner
  for (int i = 0, tickets = 0; i < qsize; i++) {
    Process *p = queue_peek_at(l->queue, i);
    
    // divy up the tickets between 0 and NUM_TICKETS based on service time
    int num_tickets = (((float) process_current_service_time(p) / (float) total_service_time) * (float) NUM_TICKETS);
    
    memset(&l->ticket_distribution[tickets], i, num_tickets);

    tickets += num_tickets;
  }

  return 0;
}

static Process *__lottery_start(void *arg) {
  if (arg == NULL) {
    return NULL;
  }

  Lottery *l = (Lottery *) arg;

  int ticket = rand() % NUM_TICKETS;

  int winner = l->ticket_distribution[ticket];

  return queue_remove_at(l->queue, winner);
}

static int __lottery_finish(Process *p, void *arg) {
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

int main() {

  srand(time(0));

  Lottery *lottery = new_lottery(__lottery_service_time_distribution);

  // create the algorithm
  Algorithm *algo = new_algorithm(__lottery_arrive, __lottery_exists, __lottery_start, __lottery_finish, lottery);

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

