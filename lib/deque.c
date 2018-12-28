#include "deque.h"
#include "interrupt.h"
#include <stddef.h>
void deque_init(struct deque * deq)
{
  deq->first.prev = NULL;
  deq->first.next = &deq->last;
  deq->last.prev = &deq->first;
  deq->last.next = NULL;
}

void deque_insert(struct deque_node * dest, struct deque_node * src)
{
  int int_status = get_interrupt();
  if (int_status) {
    disable_interrupt();
  }
  dest->prev->next = src;
  src->prev = dest->prev;
  src->next = dest;
  dest->prev = src;
  if (int_status) {
    enable_interrupt();
  }
}

void deque_push_front(struct deque * deq, struct deque_node * node)
{
  deque_insert(deq->first.next, node);
}

void deque_push_back(struct deque * deq, struct deque_node * node)
{
  deque_insert(&deq->last, node);
}

struct deque_node * deque_pop_front(struct deque * deq)
{
  return deque_remove(deq->first.next);
}

struct deque_node * deque_pop_back(struct deque * deq)
{
  return deque_remove(deq->last.prev);
}

struct deque_node * deque_remove(struct deque_node * node)
{
  int int_status = get_interrupt();
  if (int_status) {
    disable_interrupt();
  }
  node->prev->next = node->next; 
  node->next->prev = node->prev;
  if (int_status) {
    enable_interrupt();
  }
  return node;
}

int deque_empty(struct deque * deq)
{
  return deq->first.next == &deq->last;
}

int deque_exist(struct deque * deq, struct deque_node * node)
{
  struct deque_node tmp_node = deq->first;
  while (tmp_node.next != NULL) {
    if (&tmp_node == node) {
      return 1;
    }
  }
  return 0;
}
