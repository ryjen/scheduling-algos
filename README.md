# Scheduling algorithms

## design

### model

#### queue

Queue's represent a doubly linked list of processes.  Used in:

* scheduler new process arrivals
* an algorithm processing queue
* scheduler completed processes for post processing

#### processes

Represents a process in the scheduler.  The key information is:

* arrival time
* service time
* completion time

They also maintains a state of their current clock ticks.

#### algorithms

Represents an algorithm to handle queue operations through various callbacks.

* OnProcessArrive: used by the scheduler to put arrivals onto the queue
* OnProcessReady: used by the scheduler to test for queued processes
* OnProcessGet: used by the scheduler to obtain a process for a time slice (tick)
* OnProcessPut: used by the scheduler to finish a process time slice

Arbitrary data can be passed as an argument to the callbacks.

### logic

#### scheduler

A producer/consumer design pattern that:

1. accepts new arrivals on arrival time
2. sends processes to the algorithm queues
3. consumes scheduled processes
4. performs completion statistics

The scheduler maintains a clock tick for time sliced processing.


#### first come, first serve (ftfs)

self explanatory

#### shortest process next (spn)

See [wiki](https://en.wikipedia.org/wiki/Shortest_job_next)

#### shortest time remaining (str)

See [wiki](https://en.wikipedia.org/wiki/Shortest_remaining_time)

#### round robin (rr)

Accepts a quantum integer as input with a default of 3.

self explanatory.  see [wiki](https://en.wikipedia.org/wiki/Round-robin_scheduling)

#### multi level feedback queue

Accepts a quantum and the number of queues as input (defaults of 3).

See [wiki](https://en.wikipedia.org/wiki/Multilevel_feedback_queue)
  
#### lottery

The [lottery algorithm](https://en.wikipedia.org/wiki/Lottery_scheduling) uses process tickets and randomization to schedule the next process.

The way processes get tickets is left to the implementation.  I've chosen to have a global maximum tickets of 100.
A random number betweeen 0 and 99 generates the winning ticket.  


If a ticket is greater than the winning lottery number, it gets processed.

**Implementation 1**

The total tickets is divided evenly between processes.  For example, 2 processes will get 50 tickets each. Meaning process A will have 0-49 and process B will have 50-99.

**Implementation 2**

The total tickets is divided based on a percentage of the process service time.  The processes are sorted based on service time. Since the list is sorted, the processes with the greatest amount of tickets are first.

For example, if process A has a service time of 9 and process B has a service time of 3 then the total service time is 12.  
Meaning process A will get 75 tickets and process B will get 25 tickets.

## testing

```make test```

```./generate-processes | ./fcfs.verify```

```./generate-processes | ./lottery.verify```

etc.


