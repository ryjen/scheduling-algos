#ifndef RYJEN_OS_PROCESS_H
#define RYJEN_OS_PROCESS_H

#define PROCESS_ERROR -1
#define PROCESS_END    0
#define PROCESS_ALIVE  1
#define PROCESS_PREMPT 2

Process *new_process();
void delete_process(Process *);

int microsleep(int);

int process_tick(Process *);

int process_arrival_time(Process *);

#endif

