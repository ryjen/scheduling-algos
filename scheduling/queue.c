#include <stdlib.h>

#include "types.h"
#include "queue.h"
#include "process.h"

struct queue_item {
  QueueItem *next;
  QueueItem *prev;
  Process *process;
};

struct queue {
  QueueItem *first;
  QueueItem *last;
  Comparator comparator;
};

static int __queue_default_compare(QueueItem *a, QueueItem *b) {
  if (a == NULL) {
    return b ? -1 : 0;
  }

  if (b == NULL) {
    return a ? 1 : 0;
  }

  return process_arrival_time(a->process) - process_arrival_time(b->process);
}

Queue *new_queue() {

  Queue *q = (Queue *) malloc(sizeof(Queue));

  if (q == NULL) {
    abort();
  }

  q->first = NULL;
  q->last = NULL;
  q->comparator = __queue_default_compare;
  return q;
}

QueueItem *new_queue_item() {
  QueueItem *queue = (QueueItem *) malloc(sizeof(Queue));

  if (queue == NULL) {
    abort();
  }
  queue->next = NULL;
  queue->prev = NULL;
  queue->process = NULL;
 
  return queue;
}
void delete_queue(Queue *queue) {
  if (queue == NULL) {
    return;
  }
  free(queue);
}

void delete_queue_item(QueueItem *item) {
  if (item == NULL) {
    return;
  }
  free(item);
}

void delete_queue_list(Queue *list) {
  if (list == NULL) {
    return;
  }
  for (QueueItem *prev = NULL, *it = list->first; it; it = prev) {
    prev = it;
    delete_queue_item(it);
  }
  delete_queue(list);
}

int queue_set_comparator(Queue *queue, Comparator value) {
  if (queue == NULL || value == NULL) {
    return -1;
  }

  queue->comparator = value;
  return 0;
}

static int __queue_unlink(Queue *list, QueueItem *item) {

  if (list == NULL || item == NULL) {
    return -1;
  }

  if (list->first == item) {
    list->first = item->next;
  }

  if (list->last == item) {
    list->last = item->prev;
  }

  if (item->next) {
    item->next->prev = item->prev;
  }

  if (item->prev) {
    item->prev->next = item->next;
  }

  item->next = NULL;
  item->prev = NULL;
  return 0;
}

static QueueItem* __queue_append(Queue *list, QueueItem *item) {

  if (list == NULL || item == NULL) {
    return NULL;
  }

  if (list->last != NULL) {
    QueueItem *it = list->last;
    it->next = item;
    item->prev = it;
  } else {
    item->prev = NULL;
  }

  QueueItem *next = item->next;
  item->next = NULL;
  list->last = item;
  return next;
}

static QueueItem *__queue_insert(Queue *list, QueueItem *item) {
  if (list == NULL || item == NULL) {
    return NULL;
  }

  if (list->first != NULL) {
    QueueItem *it = list->first;
    it->prev = item;
    item->next = it;
  } else {
    item->next = NULL;
  }
  QueueItem *prev = item->prev;
  item->prev = NULL;
  list->first = item;
  return prev;
}

static Queue *__queue_merge(Queue *left, Queue *right, Comparator compare) {
  if (left == NULL || right == NULL) {
    return NULL;
  }

  Queue *result = new_queue();

  QueueItem *l = left->first;
  QueueItem *r = right->first;

  while(l != NULL && r != NULL) {
    if (compare(l, r) <= 0) {
      l = __queue_append(result, l);
    } else {
      r = __queue_append(result, r);
    }
  }

  while (l != NULL) {
    l = __queue_append(result, l);
  }
  while (r != NULL) {
    r = __queue_append(result, r);
  }

  delete_queue(left);
  delete_queue(right);

  return result;
}

int queue_push_front(Queue *list, Process *p) {
  if (list == NULL || p == NULL) {
    return -1;
  }

  QueueItem *item = new_queue_item();

  item->process = p;

  __queue_insert(list, item);

  return 0;
}

int queue_push_back(Queue *list, Process *p) {
  if (list == NULL || p == NULL) {
    return -1;
  }

  QueueItem *item = new_queue_item();

  item->process = p;

  __queue_append(list, item);

  return 0;
}

Queue *__queue_sort(Queue *list) {

  if (list == NULL) {
    return NULL;
  }

  Queue *left = new_queue();
  Queue *right = new_queue();
  int pos = 0;

  left->comparator = right->comparator = list->comparator;

  for (QueueItem *it = list->first, *next = NULL; it; it = next, pos++) {
    next = it->next;
    __queue_unlink(list, it);
    if (pos % 2 == 0) {
      __queue_append(left, it);
    } else {
      __queue_append(right, it); 
    }
  }
    
  left = __queue_sort(left);
  right = __queue_sort(right);

  return __queue_merge(left, right, list->comparator);
}

Process *queue_pop_front(Queue *list) {
  if (list == NULL || list->first == NULL)  {
    return NULL;
  }

  QueueItem *item = list->first;

  Process *p = item->process;

  __queue_unlink(list, item);

  delete_queue_item(item);

  return p;
}

Process *queue_peek_front(Queue *list) {

  if (list == NULL || list->first == NULL) {
    return NULL;
  }

  return list->first->process;
}

int queue_sort(Queue *list) {
  if (list == NULL) {
    return -1;
  }

  Queue *result = __queue_sort(list);

  if (result == NULL) {
    return -1;
  }

  list->first = result->first;
  list->last = result->last;

  delete_queue(result);
  return 0;
}

