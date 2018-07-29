#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "queue.h"
#include "process.h"

extern int test_assert(int);

static int __queue_test_push_back() {
  Queue *q = new_queue();

  Process *p1 = new_process("P1");

  if (queue_push_back(q, p1) == -1) {
    delete_queue_list(q);
    return 1;
  }

  if (queue_size(q) != 1 || queue_peek_front(q) != p1
      || queue_peek_back(q) != p1) {
    delete_queue_list(q);
    return 1;
  }

  Process *p2 = new_process("P2");

  if (queue_push_back(q, p2) == -1) {
    delete_queue_list(q);
    return 1;
  }

  if (queue_size(q) != 2 || queue_peek_front(q) != p1
      || queue_peek_back(q) != p2) {
    delete_queue_list(q);
    return 1;
  }

  return 0;
}

static int __queue_test_push_front() {

  Queue *q = new_queue();

  Process *p1 = new_process("P1");

  if (queue_push_front(q, p1) == -1) {
    delete_queue_list(q);
    return 1;
  }

  Process *p2 = new_process("P2");

  if (queue_push_front(q, p2) == -1) {
    delete_queue_list(q);
    return 1;
  }

  if (queue_size(q) != 2) {
    delete_queue_list(q);
    return 1;
  }

  Process *p3 = queue_peek_front(q);

  if (p3 == NULL) {
    puts("no value on front");
    return 1;
  }

  if (p3 != p2) {
    printf("process %s != %s\n", process_name(p2), process_name(p3));
    return 1;
  }

  p3 = queue_peek_back(q);

  if (p3 == NULL) {
    puts("No value on front");
    return 1;
  }

  if (p1 != p3) {
    printf("process %s != %s\n", process_name(p1), process_name(p3));
    return 1;
  }

  return 0;
}

static int __queue_test_pop_back() {

  Queue *q = new_queue();

  Process *p1 = new_process("P1");

  if (queue_push_back(q, p1) == -1) {
    return 1;
  }

  Process *p2 = new_process("P2");

  if (queue_push_back(q, p2) == -1) {
    return 1;
  }

  Process *p3 = queue_pop_back(q);

  if (p3 == NULL) {
    puts("No value on front");
    return 1;
  }

  if (p3 != p2) {
    printf("process %s != %s\n", process_name(p2), process_name(p3));
    return 1;
  }

  if (queue_size(q) != 1) {
    printf("queue size %d != 1\n", queue_size(q));
    return 1;
  }

  p3 = queue_pop_back(q);

  if (p3 == NULL) {
    puts("No value on back");
    return 1;
  }

  if (p3 != p1) {
    printf("process %s != %s\n", process_name(p1), process_name(p3));
    return 1;
  }

  if (queue_size(q) != 0) {
    printf("queue size %d != 0\n", queue_size(q));
    return 1;
  }


  return 0;
}

static int __queue_test_pop_front() {
  Queue *q = new_queue();

  Process *p1 = new_process("P1");

  if (queue_push_back(q, p1) == -1) {
    return 1;
  }

  Process *p2 = new_process("P2");

  if (queue_push_back(q, p2) == -1) {
    return 1;
  }

  Process *p3 = queue_pop_front(q);

  if (p3 == NULL) {
    return 1;
  }

  if (p1 != p3) {
    return 1;
  }

  if (queue_size(q) != 1) {
    return 1;
  }

  p3 = queue_pop_front(q);

  if (p3 == NULL) {
    return 1;
  }

  if (p2 != p3) {
    return 1;
  }

  if (queue_size(q) != 0) {
    return 1;
  }

  return 0;
}

int queue_test() {

  int fail = __queue_test_push_back();
  printf("%-30s : %s\n", "queue_push_back", fail ? "FAIL" : "PASS");

  fail |= __queue_test_push_front();
  printf("%-30s : %s\n", "queue_push_front", fail ? "FAIL" : "PASS");

  fail |= __queue_test_pop_back();
  printf("%-30s : %s\n", "queue_pop_back", fail ? "FAIL" : "PASS");

  fail |= __queue_test_pop_front();
  printf("%-30s : %s\n", "queue_pop_front", fail ? "FAIL" : "PASS");

  return fail;
}
