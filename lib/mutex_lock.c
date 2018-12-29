#include "mutex_lock.h"
#include "thread.h"
#include "interrupt.h"
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

void semaphore_p(struct semaphore * sema)
{
  int int_status = get_interrupt();
  if (int_status) {
    disable_interrupt();
  }
  while (!sema->value) {
    /* Current thread should not be in standby deque */
    deque_push_back(&sema->standby, &running_thread()->general_tag);
    block_thread(TASK_BLOCKED);
  }
  --sema->value;
  if (int_status) {
    enable_interrupt();
  }
}

void semaphore_v(struct semaphore * sema)
{
  int int_status = get_interrupt();
  if (int_status) {
    disable_interrupt();
  }
  if (!deque_empty(&sema->standby)) {
    struct task_struct * thread_blocked =
      NODE_ENTRY(struct task_struct, general_tag,
          deque_pop_front(&sema->standby));
    unblock_thread(thread_blocked);
  }
  ++sema->value;
  if (int_status) {
    enable_interrupt();
  }
}

void mutex_lock_get(struct mutex_lock * lock)
{
  if (lock->owner != running_thread()) {
    semaphore_p(&lock->semaphore);
    lock->owner = running_thread();
    lock->owner_request_count = 1;
  } else {
    ++lock->owner_request_count;
  }
}

void mutex_lock_release(struct mutex_lock * lock)
{
  if (lock->owner_request_count > 1) {
    --lock->owner_request_count;
    return;
  }
  lock->owner = NULL;
  --lock->owner_request_count;
  semaphore_v(&lock->semaphore);
}
