#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

typedef struct data {
  char* id;
} TestData;

TestData *new_test_data(char *id) {
  TestData *val = (TestData*) malloc(sizeof(TestData));
  val->id = id;
  return val;
}

char *test_data_id(TestData *data) {
  return data ? data->id : "null";
}

int test_data_compare(void *a, void *b) {
  
  return strcmp(test_data_id((TestData*)a), test_data_id((TestData*)b));
}

static int __queue_test_push_back() {
  Queue *q = new_queue();

  TestData *p1 = new_test_data("P1");

  if (queue_push_back(q, p1) == -1) {
    delete_queue_list(q);
    return 1;
  }

  if (queue_size(q) != 1 || queue_peek_front(q) != p1
      || queue_peek_back(q) != p1) {
    delete_queue_list(q);
    return 1;
  }

  TestData *p2 = new_test_data("P2");

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

  TestData *p1 = new_test_data("P1");

  if (queue_push_front(q, p1) == -1) {
    delete_queue_list(q);
    return 1;
  }

  TestData *p2 = new_test_data("P2");

  if (queue_push_front(q, p2) == -1) {
    delete_queue_list(q);
    return 1;
  }

  if (queue_size(q) != 2) {
    delete_queue_list(q);
    return 1;
  }

  TestData *p3 = queue_peek_front(q);

  if (p3 == NULL) {
    puts("no value on front");
    return 1;
  }

  if (p3 != p2) {
    printf("TestData %s != %s\n", test_data_id(p2), test_data_id(p3));
    return 1;
  }

  p3 = queue_peek_back(q);

  if (p3 == NULL) {
    puts("No value on front");
    return 1;
  }

  if (p1 != p3) {
    printf("TestData %s != %s\n", test_data_id(p1), test_data_id(p3));
    return 1;
  }

  return 0;
}

static int __queue_test_pop_back() {

  Queue *q = new_queue();

  TestData *p1 = new_test_data("P1");

  if (queue_push_back(q, p1) == -1) {
    return 1;
  }

  TestData *p2 = new_test_data("P2");

  if (queue_push_back(q, p2) == -1) {
    return 1;
  }

  TestData *p3 = queue_pop_back(q);

  if (p3 == NULL) {
    puts("No value on front");
    return 1;
  }

  if (p3 != p2) {
    printf("TestData %s != %s\n", test_data_id(p2), test_data_id(p3));
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
    printf("TestData %s != %s\n", test_data_id(p1), test_data_id(p3));
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

  TestData *p1 = new_test_data("P1");

  if (queue_push_back(q, p1) == -1) {
    return 1;
  }

  TestData *p2 = new_test_data("P2");

  if (queue_push_back(q, p2) == -1) {
    return 1;
  }

  TestData *p3 = queue_pop_front(q);

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

static int __queue_test_sort() {

  Queue *q = new_queue();

  TestData *p1 = new_test_data("P3");

  if (queue_push_back(q, p1) == -1) {
    return 1;
  }

  TestData *p2 = new_test_data("P2");

  if (queue_push_back(q, p2) == -1) {
    return 1;
  }

  TestData *p3 = new_test_data("P1");

  if (queue_push_back(q, p3) == -1) {
    return 1;
  }

  if (queue_sort(q, test_data_compare) == -1) {
    return 1;
  }

  TestData *p = queue_pop_front(q);

  if (p != p3) {
    printf("expected %s got %s", test_data_id(p3), test_data_id(p));
    return 1;
  }

  p = queue_pop_front(q);

  if (p != p2) {
    printf("expected %s got %s", test_data_id(p1), test_data_id(p));
    return 1;
  }

  p = queue_pop_front(q);

  if (p != p1) {
    printf("expected %s got %s", test_data_id(p2), test_data_id(p));
    return 1;
  }

  delete_queue_data(q);

  return 0;
}

int __queue_test_remove() {

  Queue *queue = new_queue();

  TestData *p1 = new_test_data("P1");

  if (queue_push_back(queue, p1)) {
    return 1;
  }

  TestData *p2 = new_test_data("P2");

  if (queue_push_back(queue, p2)) {
    return 1;
  }

  if (queue_size(queue) != 2) {
    return 1;
  }

  if (queue_remove(queue, p1)) {
    printf("unable to remove TestData");
    return 1;
  }

  TestData *p = queue_pop_front(queue);

  if (p != p2) {
    printf("expected %s got %s\n", test_data_id(p2), test_data_id(p));
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

  fail |= __queue_test_sort();
  printf("%-30s : %s\n", "queue_sort", fail ? "FAIL" : "PASS");

  fail |= __queue_test_remove();
  printf("%-30s : %s\n", "queue_remove", fail ? "FAIL" : "PASS");

  return fail;
}
