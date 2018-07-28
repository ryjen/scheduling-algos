# Scheduling algorithms

## design

### model

#### queue

Queue's represent a doubly linked list of processes.  They are used in a scheduler for:
* new process arrivals
* a processing queue for algorithms to work with
* a list of completed processes for post processing

#### processes

Represents a process in the scheduler with arrival time, service time and completion time.  Also maintains a state of clock ticks and quantum ticks.

#### algorithms

Represents an algorithm to find the next process to run.  They issue callbacks and also define the quantum and whether the algorithm is premptive or not.

### logic

#### scheduler

A producer/consumer design pattern that accepts new arrivals on arrival time,  sends processes to the processing queue and performs calculations on timing.

Maintains a clock tick and performs a specified algorithm on the queue at each tick.


#### lottery

The lottery algorithm uses process tickets and randomization to schedule the next process.

The way processes get tickets is left to the implementor.  I've chosen to have a global maximum tickets of 100.

For example, if process A has a service time of 9 and process B has a service time of 3 then the total service time is 12.  

A random number betweeen 0 and 99 generates the winning ticket.  

If there are more processes than max tickets then only the first max tickets will be considered.

Going throught the list of tickets if a ticket is greater than the winning lottery number, then it gets processed.

**Implementation 1**

The total tickets is divided evenly between processes.  Both processes will get 50 tickets. Meaning process A will have 0-49 and process B will have 50-99.

**Implementation 2**

The total tickets is divided based on a percentage of the process service time.  The processes are sorted based on service time. Since the list is sorted, the processes with the greatest amount of tickets are first.

In the previous example, process A will get 75 tickets and process B will get 25 tickets.

## testing

```make test```

```./generate_processes | ./fcfs```


### sample for First Come First Serve implementation

```c
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