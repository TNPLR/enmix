#include "mutex_lock.h"
#include "thread.h"
#include "deque.h"
#include <stdint.h>
#include <stddef.h>

/* init semaphore */
void semaphore_init(struct semaphore * sema, uint_fast8_t value)
{
  sema->value = value;
  deque_init(&sema->standby);
}

/* init mutex_lock */
void mutex_lock_init(struct mutex_lock * lock)
{
  lock->owner = NULL;
  lock->owner_request_count = 0;
  semaphore_init(&lock->semaphore, 1); // value for mutex is 1
}
