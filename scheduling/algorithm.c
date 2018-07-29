#include <stdlib.h>

#include "types.h"
#include "algorithm.h"
#include "queue.h"

struct algorithm {
  OnProcessArrive on_arrive;
  OnProcessExists on_exists;
  OnProcessStart on_start;
  OnProcessFinish on_finish;
  void *arg;
};

Algorithm *new_algorithm(OnProcessArrive arrive, OnProcessExists exists, OnProcessStart start, OnProcessFinish finish, void *data) {
  Algorithm *a = (Algorithm*) malloc(sizeof(Algorithm));

  if (a == NULL) {
    abort();
  }

  a->on_arrive = arrive;
  a->on_exists = exists;
  a->on_start = start;
  a->on_finish = finish;
  a->arg = data;
  return a;
}

static int __algorithm_queue_arrive(Process *p, void *arg) {
  if (p == NULL || arg == NULL) {
    return -1;
  }
  Queue *q = (Queue*) arg;

  return queue_push_back(q, p);
}

static int __algorithm_queue_exists(void *arg) {
  if (arg == NULL) {
    return -1;
  }
  Queue *q = (Queue *) arg;
  return !queue_is_empty(q);
}

Algorithm *new_queue_algorithm(Queue *queue, OnProcessStart start, OnProcessFinish finish) {
  return new_algorithm(__algorithm_queue_arrive, __algorithm_queue_exists, start, finish, queue);
}

void delete_algorithm(Algorithm *a) {
  if (a == NULL) {
    return;
  }

  free(a);
}

Process *algorithm_process_start(Algorithm *a) {
  if (a == NULL) {
    return NULL;
  }

  return a->on_start(a->arg);
}

int algorithm_process_arrive(Algorithm *a, Process *p) {
  if (a == NULL || p == NULL) {
    return -1;
  }

  return a->on_arrive(p, a->arg);
}

int algorithm_process_finish(Algorithm *a, Process *p) {
  if (a == NULL || p == NULL) {
    return -1;
  }

  return a->on_finish(p, a->arg);
}

int algorithm_process_exists(Algorithm *a) {
  if (a == NULL) {
    return -1;
  }

  return a->on_exists(a->arg);
}

