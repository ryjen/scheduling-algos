#include <stdlib.h>

#include "types.h"
#include "queue.h"
#include "process.h"

typedef struct queue_item QueueItem;

struct queue_item {
  QueueItem *next;
  QueueItem *prev;
  Process *process;
};

struct queue {
  QueueItem *first;
  QueueItem *last;
};

Queue *new_queue() {

  Queue *q = (Queue *) malloc(sizeof(Queue));

  if (q == NULL) {
    abort();
  }

  q->first = NULL;
  q->last = NULL;
  return q;
}

static QueueItem *__new_queue_item() {
  QueueItem *queue = (QueueItem *) malloc(sizeof(QueueItem));

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

static void __delete_queue_item(QueueItem *item) {
  if (item == NULL) {
    return;
  }
  free(item);
}

void delete_queue_list(Queue *list) {
  if (list == NULL) {
    return;
  }
  for (QueueItem *next = NULL, *it = list->first; it; it = next) {
    next = it->next;
    __delete_queue_item(it);
  }
  delete_queue(list);
}

void delete_queue_processes(Queue *list) {
  if (list == NULL) {
    return;
  }

  for (QueueItem *next = NULL, *it = list->first; it; it = next) {
    next = it->next;
    delete_process(it->process);
    __delete_queue_item(it);
  }
  delete_queue(list);
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
  if (list->first == NULL) {
    list->first = item;
  }
  return next;
}

static QueueItem *__queue_prepend(Queue *list, QueueItem *item) {
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
  if (list->last == NULL) {
    list->last = item;
  }
  return prev;
}

int queue_push_front(Queue *list, Process *p) {
  if (list == NULL || p == NULL) {
    return -1;
  }

  QueueItem *item = __new_queue_item();

  item->process = p;

  __queue_prepend(list, item);

  return 0;
}

int queue_push_back(Queue *list, Process *p) {
  if (list == NULL || p == NULL) {
    return -1;
  }

  QueueItem *item = __new_queue_item();

  item->process = p;

  __queue_append(list, item);

  return 0;
}

static int __queue_merge(Queue *result, Queue *left, Queue *right, Comparator compare) {
  if (left == NULL || right == NULL) {
    return -1;
  }

  QueueItem *l = left->first;
  QueueItem *r = right->first;

  while(l != NULL && r != NULL) {
    if (compare(l->process, r->process) <= 0) {
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

  return 0;
}
static int __queue_merge_sort(Queue *list, Comparator comparator) {

  if (list == NULL) {
    return -1;
  }

  if (list->first == NULL || list->first->next == NULL) {
    return 0;
  }

  Queue *left = new_queue();
  Queue *right = new_queue();
  int pos = 0;

  for (QueueItem *it = list->first, *next = NULL; it; it = next, pos++) {
    next = it->next;
    __queue_unlink(list, it);
    if (pos % 2 == 0) {
      __queue_append(left, it);
    } else {
      __queue_append(right, it); 
    }
  }

  if (__queue_merge_sort(left, comparator)) {
    return -1;
  }
  if (__queue_merge_sort(right, comparator)) {
    return -1;
  }

  if (__queue_merge(list, left, right, comparator)) {
    return -1;
  }

  delete_queue(left);
  delete_queue(right);

  return 0;
}

int queue_sort(Queue *list, Comparator comparator) {
  if (list == NULL) {
    return -1;
  }

  return __queue_merge_sort(list, comparator);
}

Process *queue_pop_front(Queue *list) {
  if (list == NULL || list->first == NULL)  {
    return NULL;
  }

  QueueItem *item = list->first;

  Process *p = item->process;

  __queue_unlink(list, item);

  __delete_queue_item(item);

  return p;
}

Process *queue_pop_back(Queue *list) {
  if (list == NULL || list->first == NULL) {
    return NULL;
  }

  QueueItem *item = list->last;

  Process *p = item->process;

  __queue_unlink(list, item);

  __delete_queue_item(item);

  return p;
}

Process *queue_peek_front(Queue *list) {

  if (list == NULL || list->first == NULL) {
    return NULL;
  }

  return list->first->process;
}

Process *queue_peek_back(Queue *list) {

  if (list == NULL || list->last == NULL) {
    return NULL;
  }

  return list->last->process;
}

Process *queue_peek_at(Queue *list, int index) {
  if (list == NULL || index < 0) {
    return NULL;
  }

  int pos = 0;

  for (QueueItem *it = list->first; it; it = it->next, pos++) {
    if (pos == index) {
      return it->process;
    }
  }
  return NULL;
}

int queue_remove(Queue *list, Process *p) {
  if (list == NULL || p == NULL) {
    return -1;
  }

  for (QueueItem *it = list->first; it; it = it->next) {
    if (it->process != p) {
      continue;
    }

    __queue_unlink(list, it);
    __delete_queue_item(it);
    return 0;
  }

  return 1;
}

Process* queue_remove_at(Queue *list, int index) {
  if (list == NULL || index < 0) {
    return NULL;
  }

  int pos = 0;

  for (QueueItem *it = list->first; it; it = it->next) {
    if (pos == index) {
      Process *p = it->process;
      __queue_unlink(list, it);
      __delete_queue_item(it);
      return p;
    }
    pos++;
  }
  return NULL;
}


int queue_iterate(Queue *queue, Iterator iterator, void *arg) {

  int index = 0;

  for (QueueItem *it = queue->first, *next = NULL; it; it = next) {
    next = it->next;

    switch(iterator(queue, index++, it->process, arg)) {
      case QUEUE_ITERATE_FINISH:
        return 0;
      case -1:
        return -1;
      default:
        break;
    }
  }
  return 0;
}

int queue_size(Queue *queue) {
  int count = 0;
  for (QueueItem *it = queue->first; it; it = it->next) {
    count++;
  }
  return count;
}

int queue_is_empty(Queue *queue) {
  return queue->first == NULL || queue->last == NULL;
}

