#include <stdlib.h>

#include "types.h"
#include "algorithm.h"

struct algorithm {
  Callback callback;
  int quantum;
  int premptive;
};

Algorithm *new_algorithm(Callback callback) {
  Algorithm *a = (Algorithm*) malloc(sizeof(Algorithm));

  if (a == NULL) {
    abort();
  }

  a->callback = callback;
  a->quantum = 0;
  return a;
}

void delete_algorithm(Algorithm *a) {
  if (a == NULL) {
    return;
  }

  free(a);
}

int algorithm_quantum(Algorithm *a) {
  if (a == NULL) {
    return -1;
  }

  return a->quantum;
}

int algorithm_premptive(Algorithm *a) {
  if (a == NULL) {
    return -1;
  }

  return a->premptive;
}

Process *algorithm_run(Algorithm *a, Queue *list) {
  if (a == NULL) {
    return NULL;
  }

  return a->callback(list);
}

int algorithm_set_quantum(Algorithm *a, int value) {
  if (a == NULL || value < 0) {
    return -1;
  }

  // a quantum assumes premptive
  a->premptive = 1;
  a->quantum = value;
  return 0;
}

int algorithm_set_premptive(Algorithm *a, int value) {
  if (a == NULL) {
    return -1;
  }

  a->premptive = value;
  return 0;
}


