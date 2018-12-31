#include "iobuffer.h"
#include "thread.h"
#include <stddef.h>
void iobuffer_init(struct iobuffer * buf)
{
  mutex_lock_init(&buf->lock);
  buf->input = NULL;
  buf->output = NULL;
  buf->head = 0;
  buf->tail = 0;
}

static int32_t next_pos(int32_t pos)
{
  return (pos + 1) % BUFSIZE;
}

int iobuffer_full(struct iobuffer * buf)
{
  return next_pos(buf->head) == buf->tail;
}

static int iobuffer_empty(struct iobuffer * buf)
{
  return buf->head = buf->tail;
}

static void wait(struct task_struct ** standby)
{
  *standby = running_thread();
  block_thread(TASK_BLOCKED);
}

static void wakeup(struct task_struct ** standby)
{
  unblock_thread(*standby);
  *standby = NULL;
}

char iobuffer_getchar(struct iobuffer * buf)
{
  while (iobuffer_empty(buf)) {
    mutex_lock_get(&buf->lock);
    wait(&buf->output);
    mutex_lock_release(&buf->lock);
  }
  char byte = buf->buf[buf->tail];
  buf->tail = next_pos(buf->tail);

  if (buf->input != NULL) {
    wakeup(&buf->input);
  }

  return byte;
}

void iobuffer_putchar(struct iobuffer * buf, const char c)
{
  while (iobuffer_full(buf)) {
    mutex_lock_get(&buf->lock);
    wait(&buf->input);
    mutex_lock_release(&buf->lock);
  }
  buf->buf[buf->head] = c;
  buf->head = next_pos(buf->head);

  if (buf->output != NULL) {
    wakeup(&buf->output);
  }
}
