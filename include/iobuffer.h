#ifndef IOBUFFER_H_
#define IOBUFFER_H_
#include "mutex_lock.h"
#include <stdint.h>
#define BUFSIZE 64
struct iobuffer {
  struct mutex_lock lock;
  struct task_struct * input;
  struct task_struct * output;
  char buf[BUFSIZE];
  int32_t head;
  int32_t tail;
};
void iobuffer_init(struct iobuffer * buf);
int iobuffer_full(struct iobuffer * buf);
int iobuffer_empty(struct iobuffer * buf);
char iobuffer_getchar(struct iobuffer * buf);
void iobuffer_putchar(struct iobuffer * buf, const char c);
#endif
