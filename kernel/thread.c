#include "thread.h"
#include "string.h"
#include "memory.h"
#include <stdint.h>

#define PG_SIZE 0x1000

static void kernel_thread(void (*func)(void *), void * args)
{
  func(args);
}

void thread_create(struct task_struct * pthread,
    void (*func)(void *), void * args)
{
  pthread->task_stack -= sizeof(struct interrupt_stack) / 8;
  pthread->task_stack -= sizeof(struct thread_stack) / 8;
  struct thread_stack * kthread_stack = (struct thread_stack *)pthread->task_stack;
  kthread_stack->rip = kernel_thread;
  kthread_stack->func = func;
  kthread_stack->args = args;

  kthread_stack->rbx = 0;
  kthread_stack->rbp = 0;
  kthread_stack->r12 = 0;
  kthread_stack->r13 = 0;
  kthread_stack->r14 = 0;
  kthread_stack->r15 = 0;
}

void init_thread(struct task_struct * pthread, const char *name, int priority)
{
  memset(pthread, 0, sizeof(*pthread));
  memcpy(pthread->name, name, 16); // TODO: detect name which is too long
  pthread->status = TASK_RUNNING;
  pthread->priority = priority;
  pthread->task_stack = (uint64_t *)((uint64_t)pthread+PG_SIZE);
  pthread->stack_magic = 0x52735273; // magic number by myself
}

struct task_struct * thread_start(const char * name, int priority,
    void (*func)(void *), void * args)
{
  struct task_struct * thread = get_kernel_pages(1);
  init_thread(thread, name, priority);
  thread_create(thread, func, args);

  asm volatile("movq %0, %%rsp;"
      "popq %%rbx; popq %%rbp; popq %%r12;"
      "popq %%r13; popq %%r14; popq %%r15;"
      "popq %%rdi; popq %%rsi; ret"::"g"(thread->task_stack):"memory");
  return thread;
}
