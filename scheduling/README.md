# Scheduling algorithms

### testing

```./generate_processes | ./fcfs```

### sample for First Come First Serve implementation

```
static Process * __first_come_first_serve(Queue *list) {
  if (list == NULL) {
    return NULL;
  }

  // return the first of the unmodified queue
  return queue_pop_front(list);
}

int main() {

  // create the algorithm
  Algorithm *algo = new_algorithm(__first_come_first_serve);

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

```