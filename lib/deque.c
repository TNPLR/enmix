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
  disable_interrupt();
  dest->prev->next = src;
  src->prev = dest->prev;
  src->next = dest;
  dest->prev = src;
  enable_interrupt();
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
  node->prev->next = node->next; 
  node->next->prev = node->prev;
  node->prev = NULL;
  node->next = NULL;
  return node;
}

int deque_empty(struct deque * deq)
{
  return deq->first.next == &deq->last;
}
