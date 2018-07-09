#define _BSD_SOURCE
#include <stdlib.h>
#include <unistd.h>

#include "types.h"
#include "process.h"

struct process {
  int arrival;
  int service;
  int status;
  int tick;
  int complete;
};

Process *new_process() {
  Process *p = (Process *) malloc(sizeof(Process));

  if (p == NULL) {
    abort();
  }

  p->status = PROCESS_END;
  p->arrival = 0;
  p->service = 0;
  p->complete = 0;
  return p;
}

void delete_process(Process *p) {
  free(p);
}

int process_arrival_time(Process *p) {
  return p == NULL ? 0 : p->arrival;
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

int process_tick(Process *p) {
  if (p == NULL) {
    return PROCESS_ERROR;
  }

  if (p->service == 0) {
    return PROCESS_END;
  }

  p->tick++;
  p->service--;

  switch(p->service) {
    case 0:
      return PROCESS_END;
    default:
      return PROCESS_ALIVE;
  }
}

