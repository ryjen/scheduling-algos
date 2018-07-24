#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "types.h"
#include "scheduler.h"
#include "queue.h"
#include "process.h"
#include "algorithm.h"

#define SCHEDULER_ERROR -1
#define SCHEDULER_END    0
#define SCHEDULER_ALIVE  1
#define SCHEDULER_DONE   2

struct scheduler {
	// a queue for new arrivals
	Queue *arrivals;
	// a processing queue
	Queue *queue;
	// completed processes queue
	Queue *completed;
	// the specified algorithm to find the next process
	Algorithm *algorithm;
	// a return status
	int status;
	// a return error
	int error;
	// ticks in the clock
	int tick;

	// thread locking
	pthread_cond_t can_produce;
	pthread_cond_t can_consume;
	pthread_mutex_t lock;
};

/**
 * allocates a new scheduler instance
 * @return the scheduler created
 */
Scheduler *new_scheduler(Algorithm *algo) {

	Scheduler *value = (Scheduler *) malloc(sizeof(Scheduler));

	if (value == NULL) {
		abort();
	}

	value->arrivals = new_queue();
	value->queue = new_queue();
	value->completed = new_queue();

	value->algorithm = algo;
	value->status = SCHEDULER_END;
	value->error = 0;
	value->tick = 0;

	pthread_mutex_init(&value->lock, NULL);

	pthread_cond_init(&value->can_produce, NULL);
	pthread_cond_init(&value->can_consume, NULL);

	return value;
}

/**
 * frees an allocated scheduler instance
 * NOTE: does not delete processes in the queues
 * @param value the scheduler instance
 */
void delete_scheduler(Scheduler *value) {
	delete_queue_list(value->arrivals);
	delete_queue_list(value->queue);
	delete_queue(value->completed);

	delete_algorithm(value->algorithm);

	pthread_mutex_destroy(&value->lock);

	pthread_cond_destroy(&value->can_consume);
	pthread_cond_destroy(&value->can_produce);
}


/**
 * if the scheduler has an error sets the scheduler status
 * @return 1 if there is error otherwise 0
 */
static int __scheduler_error(Scheduler *sched, int err, const char *message) {
	if (err) {
		sched->status = SCHEDULER_ERROR;
		sched->error = err;
		printf("%d : %s\n", err, message);
	}
	return err;
}


/**
 * checks the arrival time to see if a process can be executed
 * @param sched the scheduler instance
 * @return 1 on success, 0 on failure, -1 on error
 */
static int __scheduler_has_new_arrival(Scheduler *sched) {
	if (sched == NULL) {
		return 0;
	}

	if (queue_is_empty(sched->arrivals)) {
		return 0;
	}

	Process *p = queue_peek_front(sched->arrivals);

	return process_arrival_time(p) <= sched->tick;
}

/**
 * handles premption for quantum times
 * @param sched the scheduler instance
 * @param p the process instance
 * @return 0 on success, -1 on error
 */
static int __scheduler_quantum_prempt(Scheduler *sched, Process *p) {
	if (sched == NULL || p == NULL) {
		return -1;
	}

	// non-premptive just keep the current process
	if (!algorithm_premptive(sched->algorithm)) {
		return queue_push_front(sched->queue, p);
	}

	int q = algorithm_quantum(sched->algorithm);

	// process has not reached the quantum...
	if (process_quantum(p) < q) {
		// keep it as current
		return queue_push_front(sched->queue, p);
	}

	// prempt
	if (process_finish(p) == -1) {
		return -1;
	}

	// and put on back of queue
	return queue_push_back(sched->queue, p);
}

/**
 * handles premption for service times
 * @param sched the scheduler instance
 * @param p the process instance
 * @return 0 on success, -1 on error
 */
static int __scheduler_service_prempt(Scheduler *sched, Process *p) {

	if (sched == NULL || p == NULL) {
		return -1;
	}

	// non-premptive....
	if (!algorithm_premptive(sched->algorithm)) {
		// put new process on back of queue
		return queue_push_back(sched->queue, p);
	}

	Process *curr = queue_peek_front(sched->queue);

	// current process least service time...
	if (process_compare_current_service_times(curr, p) < 0) {
		// put new process on back of queue
		return queue_push_back(sched->queue, p);
	}

	// prempt the current
	if (process_finish(curr) == -1) {
		return -1;
	}

	// then set the next process as current
	if (queue_push_front(sched->queue, p) == -1) {
		return -1;
	}

	return 0;
}

/**
 * produces new arrivals and puts them on the queue
 * @param arg the thread parameter (should be scheduler instance)
 * @return NULL (check scheduler instance for error number)
 */
static void *__scheduler_produce(void *arg) {
	Scheduler *sched = (Scheduler *) arg;
	int err = 0;

	// while the scheduler still has arrivals...
	while(sched->status == SCHEDULER_ALIVE) {

		// lock the scheduler
		err = pthread_mutex_lock(&sched->lock);

		if (__scheduler_error(sched, err, "pthread_mutex_lock")) {
			break;
		}

		// while there is no new arrival...
		while(!__scheduler_has_new_arrival(sched)) {

			// wait until there is a clock tick
			err = pthread_cond_wait(&sched->can_produce, &sched->lock);

			if (__scheduler_error(sched, err, "pthread_cond_wait")) {
				break;
			}
		}

		if (err) {
			break;
		}

		// remove the first arrival and put on the queue
		Process *p = queue_pop_front(sched->arrivals);

		if (p != NULL) {

			err = __scheduler_service_prempt(sched, p);

			if (__scheduler_error(sched, err, "scheduler_queue_process")) {
				break;
			}
		}

		// when nothing in the arrival queue, set the scheduler as "done"
		if (queue_size(sched->arrivals) == 0) {
			sched->status = SCHEDULER_DONE;
		}

		// unlock the scheduler
		err = pthread_mutex_unlock(&sched->lock);

		if (__scheduler_error(sched, err, "pthread_mutex_unlock")) {
			break;
		}

		// signal that there is a new arrival to service
		err = pthread_cond_signal(&sched->can_consume);

		if (__scheduler_error(sched, err, "pthread_cond_signal")) {
			break;
		}
	}

	// destructor will handle additional cleanup

	return NULL;
}

/**
 * consumes new arrival put on the queue.
 * the scheduler will use the algorithm specified to
 * process the queue.
 * @param arg the thread argument (should be a scheduler instance)
 * @return NULL (check scheduler for error number)
 */
static void *__scheduler_consume(void *arg) {
	Scheduler *sched = (Scheduler *) arg;
	int err = 0;

	// while the scheduler is still alive...
	while(sched->status >= SCHEDULER_ALIVE) {

		// lock the scheduler
		err = pthread_mutex_lock(&sched->lock);

		if (__scheduler_error(sched, err, "pthread_mutex_lock")) {
			break;
		}

		// while there is nothing in the queue...
		while (queue_is_empty(sched->queue)) {
			err = pthread_cond_wait(&sched->can_consume, &sched->lock);

			if (__scheduler_error(sched, err, "pthread_cond_wait")) {
				break;
			}
		}

		if (err) {
			break;
		}

		Process *p = algorithm_run(sched->algorithm, sched->queue);

		// if there is a process in the queue...
		if (p != NULL) {

			// output and update tick count
			printf("Time %02d : Process %s Arrival %02d Service %02d\n", sched->tick, process_name(p), 
					process_current_arrival_time(p), process_current_service_time(p));

			sched->tick++;

			int current = process_run(p);

			// execute a time slice on the process
			switch(current) {
				case 0:
					// no more service time, set as completed
					process_set_completion_time(p, sched->tick);
					err = queue_push_back(sched->completed, p);
					break;
				case -1:
					// handle funkiness
					sched->status = SCHEDULER_ERROR;
					break;
				default: 
					err = __scheduler_quantum_prempt(sched, p);
					break;

			}

			if (__scheduler_error(sched, err, "process_tick")) {
				break;
			}
		}

		// quick check to stop the consumer if producer is done
		if (sched->status == SCHEDULER_DONE) {
			if (queue_size(sched->queue) == 0) {
				sched->status = SCHEDULER_END;
			}
		}

		// unlock the scheduler
		err = pthread_mutex_unlock(&sched->lock);

		if (__scheduler_error(sched, err, "pthread_mutex_unlock")) {
			break;
		}

		// finally check the producer for new items
		err = pthread_cond_signal(&sched->can_produce);

		if (__scheduler_error(sched, err, "pthread_mutex_unlock")) {
			break;
		}

		microsleep(100);
	}

	// destructor will handle additional cleanup

	return NULL;
}

/**
 * runs the scheduler by spawning an arrival producer,
 * a process consumer, and a cpu ticks.
 * @param sched the scheduler instance
 * @return 0 on success, otherwise an integer indicating an error
 */
int scheduler_run(Scheduler *sched) {

	pthread_t producer, consumer;

	// set scheduler status
	sched->status = SCHEDULER_ALIVE;

	// start the arrival producer
	int err = pthread_create(&producer, 0, __scheduler_produce, sched);

	if (__scheduler_error(sched, err, "pthread_create")) {
		return sched->error;
	}

	// start the process consumer
	err = pthread_create(&consumer, 0, __scheduler_consume, sched);

	if (__scheduler_error(sched, err, "pthread_create")) {
		return sched->error;
	}

	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);

	printf("\n%-24s : %.2f\n", "Average Turn Around Time", scheduler_avg_turnaround_time(sched));
	printf("%-24s : %.2f\n\n", "Average Wait Time", scheduler_avg_wait_time(sched));

	return sched->error;
}


/**
 * adds a process to the scheduler arrivals queue
 * @param sched the scheduler
 * @param p the process
 * @return -1 on error, 0 on success
 */
int scheduler_add_process(Scheduler *sched, Process *p) {
	// sanitize
	if (sched == NULL || p == NULL) {
		return -1;
	}

	if (pthread_mutex_lock(&sched->lock)) {
		return -1;
	}

	// push process onto arrivals queue
	if (queue_push_back(sched->arrivals, p)) {
		pthread_mutex_unlock(&sched->lock);
		return -1;
	}

	// sort the arrivals based on arrival time
	if (queue_sort(sched->arrivals, process_compare_arrival_times) == -1) {
		pthread_mutex_unlock(&sched->lock);
		return -1;
	}

	if (pthread_mutex_unlock(&sched->lock)) {
		return -1;
	}

	if (pthread_cond_signal(&sched->can_produce)) {
		return -1;
	}

	return 0;
}

/**
 * reads process information from input
 * @param sched the scheduler instance
 * @return 0 if items were added to the queue, 1 if not, -1 on error
 */
int scheduler_read_processes(Scheduler *sched) {

	char buf[BUFSIZ] = {0};

	// prompt the user
	puts("Enter processes in the following format (enter blank line to quit):\n");

	puts("<Name> <Arrival Time> <Service Time>\n");

	// while reading from standard input...
	while(fgets(buf, BUFSIZ, stdin)) {
		char name[100] = {0};
		int atime = 0;
		int stime = 0;

		// scan the line for parameters
		if (sscanf(buf, "%99s %d %d", name, &atime, &stime) != 3) {
			// check for empty line
			if (buf[0] == '\n') {
				break;
			}
			// otherwise print error
			puts("Sorry, invalid format or could not scan.  Please try again.");
			continue;
		}

		// create a new process with specified parameters
		Process *p = new_process(name);

		if (process_set_arrival_time(p, atime) == -1) {
			puts("unable to set arrival time");
			return -1;
		}

		if (process_set_service_time(p, stime) == -1) {
			puts("unable to set process service time");
			return -1;
		}

		// finally add the process to the arrivals queue
		if (scheduler_add_process(sched, p) == -1) {
			puts("unable to add process to scheduler");
			return -1; 
		}

		printf("Added : Process %s Arrival %02d Service %02d\n", name, atime, stime);
	}

	printf("\n");

	// return 0 if items were added to queue 1 otherwise
	return queue_size(sched->arrivals) > 0 ? 0 : 1;
}

// iterates a queue tracking the total turnaround time
static int __process_turnaround_time_iterator(Queue *list, int index, Process *p, void *arg) {

	if (list == NULL || arg == NULL || p == NULL || index == -1) {
		return -1;
	}

	int *total = (int*) arg;

	*total += (process_completion_time(p) - process_arrival_time(p));

	return QUEUE_ITERATE_NEXT;
}

// iterates a queue tracking the total wait time
static int __process_wait_time_iterator(Queue *list, int index, Process *p, void *arg) {
	if (list == NULL || arg == NULL || p == NULL || index == -1) {
		return -1;
	}

	int *total = (int *) arg;

	*total += ((process_completion_time(p) - process_arrival_time(p)) - process_service_time(p));

	return QUEUE_ITERATE_NEXT;
}

static int __process_service_time_iterator(Queue *list, int index, Process *p, void *arg) {
  if (list == NULL || arg == NULL || p == NULL || index == -1) {
    return -1;
  }

  int *total = (int*) arg;

  *total += process_service_time(p);

  return QUEUE_ITERATE_NEXT;
}

float scheduler_avg_turnaround_time(Scheduler *sched) {

	if (sched == NULL || sched->status != SCHEDULER_END) {
		return -1;
	}

	int total = 0;

	if (queue_iterate(sched->completed, __process_turnaround_time_iterator, &total) == -1) {
		return -1;
	}

	return (float) total / (float) queue_size(sched->completed);
}

float scheduler_avg_wait_time(Scheduler *sched) {
	if (sched == NULL || sched->status != SCHEDULER_END) {
		return -1;
	}

	int total = 0;

	if (queue_iterate(sched->completed, __process_wait_time_iterator, &total) == -1) {
		return -1;
	}

	return (float) total / (float) queue_size(sched->completed);
}

int scheduler_total_remaining_service_time(Scheduler *sched) {
  if (sched == NULL) {
    return -1;
  }

  int total = 0;

  if (queue_iterate(sched->queue, __process_service_time_iterator, &total) == -1) {
    return -1;
  }

  return total;
}


