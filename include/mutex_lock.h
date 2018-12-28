#ifndef MUTEX_LOCK_H_
#define MUTEX_LOCK_H_
#include "deque.h"
#include "thread.h"
#include <stdint.h>
struct semaphore {
  uint_fast8_t value;
  struct deque standby;
};
struct mutex_lock {
  struct task_struct * owner;
  struct semaphore semaphore;
  uint_fast16_t owner_request_count;
};

void semaphore_init(struct semaphore * sema, uint_fast8_t value);
void mutex_lock_init(struct mutex_lock * lock);
#endif /* MUTEX_LOCK_H_ */
