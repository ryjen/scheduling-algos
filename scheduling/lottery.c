#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "process.h"
#include "algorithm.h"

// global number of tickets
#define NUM_TICKETS 100

// a lottery type
typedef struct lottery Lottery;

// lottery data
struct lottery {
  // total amount of service time remaining
  int total_service_time;
  // an array of current processes
  Process **processes;
  // the winning ticket value
  int winning_ticket;
  // the winning process
  Process *winner;
};

// simply iterates processes to find the total service time
static int __build_lottery_info(Queue *queue, int index, Process *p, void *arg) {
  if (queue == NULL || index == -1 || p == NULL || arg == NULL) {
    return -1;
  }

  Lottery *list = (Lottery *) arg;
 
  list->total_service_time += process_service_time(p);

  list->processes[index] = p;

  return QUEUE_ITERATE_NEXT;
}

int qsort_processes(const void *a, const void *b) {
  return process_service_time((Process*) a) - process_service_time((Process*) b);
}

// an algorithm to auto-assign tickets and a winner
static Process * __lottery_auto_ticket(Queue *list) {
  if (list == NULL) {
    return NULL;
  }

  const int qsize = queue_size(list);

  // if there is only one item
  if (qsize <= 1) {
    // quickly return it
    return queue_pop_front(list);
  }

  Lottery lottery;

  // assign lottery winning ricket
  lottery.winning_ticket = rand() % NUM_TICKETS;

  // initialize lottery data
  lottery.winner = NULL;
  lottery.total_service_time = 0;
  
  // allocate dynamic data
  lottery.processes = calloc(qsize, sizeof(Process*));

  // first, build the lottery info from the queue
  if (queue_iterate(list, __build_lottery_info, &lottery) == -1) {
    return NULL;
  }

  // sort the processes on service time in ascending order
  qsort(lottery.processes, qsize, sizeof(Process*), qsort_processes);

  // determine the ticket for a process and check for a winner
  for (int i = 0, ticket = 0; i < qsize; i++) {
    Process *p = lottery.processes[i];
    
    // divy up the tickets between 0 and NUM_TICKETS based on service time
    ticket += (((float) process_service_time(p) / (float) lottery.total_service_time) * (float) NUM_TICKETS);
 
    if (ticket > lottery.winning_ticket) {
      lottery.winner = p;
      break;
    }
  }

  // cleanup
  free(lottery.processes);

  // no winner?
  if (lottery.winner == NULL) {
    puts("Error no lottery winner");
    return NULL;
  }

  // remove the winner from the queue
  if (queue_remove(list, lottery.winner) == -1) {
    return NULL;
  }

  // return the winner
  return lottery.winner;
}

int main() {

  srand(time(0));

  // create the algorithm
  Algorithm *algo = new_algorithm(__lottery_auto_ticket);

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
