#include <stdlib.h>
#include <pthread.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "process.h"

#define SCHEDULER_ERROR -1
#define SCHEDULER_END    0
#define SCHEDULER_ALIVE  1

struct scheduler {
  Queue *arrivals;
  Queue *queue;
  Algorithm get_next;
  int status;
  int error;
  int tick;
  pthread_cond_t new_arrival;
  pthread_cond_t new_tick;
  pthread_mutex_t lock;
};

Scheduler *new_scheduler() {

  Scheduler *value = (Scheduler *) malloc(sizeof(Scheduler));

  if (value == NULL) {
    abort();
  }

  value->arrivals = NULL;
  value->queue = NULL;
  value->get_next = NULL;
  value->status = SCHEDULER_END;
  value->error = 0;
  value->tick = 0;
  pthread_mutex_init(&value->lock, NULL);
  pthread_cond_init(&value->new_arrival, NULL);
  pthread_cond_init(&value->new_tick, NULL);
  return value;
}


void delete_scheduler(Scheduler *value) {
  delete_queue_list(value->arrivals);
  delete_queue_list(value->queue);
  pthread_mutex_destroy(&value->lock);
  pthread_cond_destroy(&value->new_arrival);
  pthread_cond_destroy(&value->new_tick);
}

int scheduler_set_algorithm(Scheduler *sched, Algorithm handler) {
  if (sched == NULL || handler == NULL) {
    return -1;
  }

  sched->get_next = handler;
  return 0;
}

/**
 * if the scheduler has an error sets the scheduler status
 * @return 1 if there is error otherwise 0
 */
static int __scheduler_error(Scheduler *sched, int err) {
  if (err) {
    sched->status = SCHEDULER_ERROR;
    sched->error = err;
  }
  return err;
}


static int __scheduler_has_new_arrival(Scheduler *sched) {
  if (sched == NULL) {
    return -1;
  }

  if (sched->arrivals == NULL) {
    return 0;
  }

  Process *p = queue_peek_front(sched->arrivals);

  return process_arrival_time(p) < sched->tick;
}

/**
 * unlinks the first new arrival and adds it to the queue
 */
static int __scheduler_arrival_to_queue(Scheduler *sched) {
  Process *p = queue_pop_front(sched->arrivals);

  if (p == NULL) {
    return -1;
  }

  return queue_push_back(sched->queue, p);
}

/**
 * produces new arrivals and puts them on the queue
 */
static void *__scheduler_produce(void *arg) {
  Scheduler *sched = (Scheduler *) arg;

  // while the scheduler still has arrivals...
  while(sched->status == SCHEDULER_ALIVE &&
      sched->arrivals != NULL) {

    // lock the scheduler
    int err = pthread_mutex_lock(&sched->lock);

    if (__scheduler_error(sched, err)) {
      break;
    }

    // while there is no new arrival...
    while(__scheduler_has_new_arrival(sched)) {
      // wait until there is a clock tick
      err = pthread_cond_wait(&sched->new_tick, &sched->lock);

      if (__scheduler_error(sched, err)) {
        break;
      }
    }

    if (err) {
      break;
    }

    // ok, put the arrival on the queue
    err = __scheduler_arrival_to_queue(sched);

    if (__scheduler_error(sched, err)) {
      break;
    }

    // unlock the scheduler
    err = pthread_mutex_unlock(&sched->lock);

    if (__scheduler_error(sched, err)) {
      break;
    }

    // signal that there is a new arrival to service
    err = pthread_cond_signal(&sched->new_arrival);

    if (__scheduler_error(sched, err)) {
      break;
    }
  }
  
  return NULL;
}

/**
 * consumes new arrival put on the queue.
 * the scheduler will use the algorithm specified to
 * process the queue.
 */
static void *__scheduler_consume(void *arg) {

  Scheduler *sched = (Scheduler *) arg;

  // while the scheduler is still alive...
  while(sched->status == SCHEDULER_ALIVE) {

    // lock the scheduler
    int err = pthread_mutex_lock(&sched->lock);

    if (__scheduler_error(sched, err)) {
      break;
    }

    // while there is nothing in the queue...
    while (sched->queue == NULL) {
      err = pthread_cond_wait(&sched->new_arrival, &sched->lock);

      if (__scheduler_error(sched, err)) {
        break;
      }
    }

    if (err) {
      break;
    }

    Process *p = sched->get_next(sched->queue);

    if (p == NULL) {
      sched->status = SCHEDULER_END;
      break;
    }

    err = process_tick(p);

    // process a tick for the process
    switch(err) {
      case PROCESS_END:
        break;
      case PROCESS_PREMPT:
        queue_push_back(sched->queue, p);
        break;
      case PROCESS_ERROR:
        sched->status = SCHEDULER_ERROR;
        break;   
      default:
        break;
    }

    err = pthread_mutex_unlock(&sched->lock);

    if (__scheduler_error(sched, err)) {
      return sched;
    }

  }

  return NULL;
}

/**
 * runs the scheduler by spawning an arrival producer,
 * a process consumer, and a cpu ticks.
 */
int scheduler_run(Scheduler *sched) {

  pthread_t producer, consumer;

  // set scheduler status
  sched->status = SCHEDULER_ALIVE;

  // start the arrival producer
  int err = pthread_create(&producer, 0, __scheduler_produce, sched);

  if (__scheduler_error(sched, err)) {
    return sched->error;
  }

  // start the process consumer
  err = pthread_create(&consumer, 0, __scheduler_consume, sched);

  if (__scheduler_error(sched, err)) {
    return sched->error;
  }

  // while the scheduler is still alive....
  while(sched->status == SCHEDULER_ALIVE) {
    
    // lock the scheduler
    err = pthread_mutex_lock(&sched->lock);

    if (__scheduler_error(sched, err)) {
      break;
    }

    // sleep for a bit
    microsleep(100);

    // increment tick
    sched->tick++;

    // unlock
    err = pthread_mutex_unlock(&sched->lock);

    if (__scheduler_error(sched, err)) {
      break;
    }

    // signal a new tick
    err = pthread_cond_signal(&sched->new_tick);

    if (__scheduler_error(sched, err)) {
      break;
    }
  }

  return sched->error;
}

int scheduler_add_process(Scheduler *sched, Process *p) {
  if (sched == NULL) {
    return -1;
  }

  if (queue_push_back(sched->arrivals, p)) {
    return -1;
  }

  if (queue_sort(sched->arrivals)) {
    return -1;
  }
  
  return 0;
}


