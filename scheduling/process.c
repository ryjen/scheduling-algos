#define _DEFAULT_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "process.h"

// a process in the queue
struct process {
  // process id
  char *name;
  // the time the process arrives
  int arrival;
  // the time needed to process
  int service;

  // status of the process
  int status;
  // the time the process completed
  int complete;
 
  // how many ticks serviced so far
  int total_ticks;
  // the ticks serviced before premption
  int ticks;
};

Process *new_process(const char *name) {
  Process *p = (Process *) malloc(sizeof(Process));

  if (p == NULL) {
    abort();
  }

  p->name = name == NULL ? NULL : strdup(name);
  p->status = PROCESS_END;
  p->arrival = 0;
  p->service = 0;
  p->complete = 0;
  p->ticks = 0;
  p->total_ticks = 0;
  return p;
}

void delete_process(Process *p) {
  if (p == NULL) {
    return;
  }
  if (p->name) {
    free(p->name);
  }
  free(p);
}

const char *process_name(Process *p) {
  if (p == NULL) {
    return NULL;
  }

  return p->name;
}

int process_arrival_time(Process *p) {
  return p == NULL ? 0 : p->arrival;
}

int process_current_arrival_time(Process *p) {
  return p == NULL ? 0 : (p->arrival + p->total_ticks);
}

int process_set_arrival_time(Process *p, int value) {
  if (p == NULL) {
    return -1;
  }

  p->arrival = value;
  return 0;
}

int process_service_time(Process *p) {
  return p == NULL ? 0 : p->service;
}

int process_current_service_time(Process *p) {
  return p == NULL ? 0 : (p->service - p->total_ticks);
}

int process_set_service_time(Process *p, int value) {
  if (p == NULL) {
    return -1;
  }

  p->service = value;
  return 0;
}

int process_completion_time(Process *p) {
  return p == NULL ? 0 : p->complete;
}

int process_set_completion_time(Process *p, int value) {
  if (p == NULL) {
    return -1;
  }

  p->complete = value;
  return 0;
}

int process_compare_current_service_times(Process *a, Process *b) {
  if (a == NULL) {
    return b ? -1 : 0;
  }
  if (b == NULL) {
    return a ? 1 : 0;
  }

  return process_current_service_time(a) - process_current_service_time(b);
}

int process_compare_arrival_times(Process *a, Process *b) {
  if (a == NULL) {
    return b ? -1 : 0;
  }
  if (b == NULL) {
    return a ? -1 : 0;
  }

  return a->arrival - b->arrival;
}

int microsleep(int milliseconds) {
#if _POSIX_C_SOURCE >= 199309L
  struct timespec ts;
  ts.tv_sec = milliseconds / 1000;
  ts.tv_nsec = (milliseconds % 1000) * 1000000;
  return nanosleep(&ts, NULL);
#else
  return usleep(milliseconds * 1000);
#endif
}

int process_run(Process *p) {
  if (p == NULL) {
    return PROCESS_ERROR;
  }

  if (p->service == p->ticks) {
    return PROCESS_END;
  }

  p->ticks++;
  p->total_ticks++;

  return p->service - p->total_ticks;
}

int process_prempt(Process *p) {
  if (p == NULL) {
    return -1;
  }

  p->ticks = 0;
  return 0;
}

int process_current_tick(Process *p) {
  if (p == NULL) {
    return -1;
  }

  return p->ticks;
}

