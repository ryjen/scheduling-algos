#include <stdlib.h>

#include "types.h"
#include "algorithm.h"
#include "queue.h"

struct algorithm {
  OnProcessArrive on_arrive;
  OnProcessReady on_ready;
  OnProcessGet on_get;
  OnProcessPut on_put;
  void *arg;
};

Algorithm *new_algorithm(OnProcessArrive arrive, OnProcessReady ready, OnProcessGet get, OnProcessPut put, void *data) {
  Algorithm *a = (Algorithm*) malloc(sizeof(Algorithm));

  if (a == NULL) {
    abort();
  }

  a->on_arrive = arrive;
  a->on_ready = ready;
  a->on_get = get;
  a->on_put = put;
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

static int __algorithm_queue_ready(void *arg) {
  if (arg == NULL) {
    return -1;
  }
  Queue *q = (Queue *) arg;
  return !queue_is_empty(q);
}

Algorithm *new_queue_algorithm(Queue *queue, OnProcessGet get, OnProcessPut put) {
  return new_algorithm(__algorithm_queue_arrive, __algorithm_queue_ready, get, put, queue);
}

void delete_algorithm(Algorithm *a) {
  if (a == NULL) {
    return;
  }

  free(a);
}

Process *algorithm_process_get(Algorithm *a) {
  if (a == NULL) {
    return NULL;
  }

  return a->on_get(a->arg);
}

int algorithm_process_arrive(Algorithm *a, Process *p) {
  if (a == NULL || p == NULL) {
    return -1;
  }

  return a->on_arrive(p, a->arg);
}

int algorithm_process_put(Algorithm *a, Process *p) {
  if (a == NULL || p == NULL) {
    return -1;
  }

  return a->on_put(p, a->arg);
}

int algorithm_process_ready(Algorithm *a) {
  if (a == NULL) {
    return -1;
  }

  return a->on_ready(a->arg);
}

