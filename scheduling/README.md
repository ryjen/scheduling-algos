# Scheduling algorithms

## design

### model

#### queue

Queue's represent a doubly linked list of processes.  They are used in a scheduler for:

* new process arrivals
* a processing queue for algorithms to work with
* a list of completed processes for post processing

#### processes

Represents a process in the scheduler.  The key information is:

* arrival time
* service time
* completion time.  

They also maintains a state of clock ticks.

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
3. performs completion statistics

The scheduler maintains a clock tick for time sliced processing.


#### lottery

The lottery algorithm uses process tickets and randomization to schedule the next process.

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

```./generate_processes | ./fcfs | ./fcfs.test```

```./generate_processes | ./lottery 42 | ./lottery.test```

etc.


