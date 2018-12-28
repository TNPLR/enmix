#include "thread.h"
#include "string.h"
#include "memory.h"
#include "interrupt.h"
#include "deque.h"
#include "kio.h"
#include "assert.h"
#include <stdint.h>
#include <stddef.h>

#define PG_SIZE 0x1000

struct task_struct * main_thread;
struct deque thread_ready_deque;
struct deque thread_all_deque;
static struct deque_node * thread_tag;

extern void switch_to(struct task_struct * cur, struct task_struct * next);

struct task_struct * running_thread(void)
{
  uint64_t rsp;
  asm("movq %%rsp, %0":"=g"(rsp));
  return (struct task_struct *)(rsp & 0xfffffffffffff000ULL);
}

static void kernel_thread(void (*func)(void *), void * args)
{
  enable_interrupt();
  func(args);
}

void thread_create(struct task_struct * pthread,
    void (*func)(void *), void * args)
{
  pthread->task_stack -= sizeof(struct interrupt_stack) / 8;
  pthread->task_stack -= sizeof(struct thread_stack) / 8;
  struct thread_stack * kthread_stack = (struct thread_stack *)pthread->task_stack;
  kthread_stack->rip = kernel_thread;

  kthread_stack->rbx = 0;
  kthread_stack->rbp = 0;
  kthread_stack->r12 = 0;
  kthread_stack->r13 = 0;
  kthread_stack->r14 = 0;
  kthread_stack->r15 = 0;
  kthread_stack->rdi = (uint64_t)func;
  kthread_stack->rsi = (uint64_t)args;
}

void init_thread(struct task_struct * pthread, const char *name, int priority)
{
  memset(pthread, 0, sizeof(*pthread));
  memcpy(pthread->name, name, 16); // TODO: detect name which is too long
  if (pthread == main_thread) {
    pthread->status = TASK_RUNNING;
  } else {
    pthread->status = TASK_READY;
  }

  pthread->priority = priority;
  pthread->task_stack = (uint64_t *)((uint64_t)pthread+PG_SIZE);
  pthread->ticks = priority;
  pthread->elapsed_ticks = 0;
  pthread->pgdir = NULL;
  pthread->stack_magic = 0x52735273; // magic number by myself
}

struct task_struct * thread_start(const char * name, int priority,
    void (*func)(void *), void * args)
{
  struct task_struct * thread = get_kernel_pages(1);
  init_thread(thread, name, priority);
  thread_create(thread, func, args);

  deque_push_back(&thread_ready_deque, &thread->general_tag);
  deque_push_back(&thread_all_deque, &thread->all_deque_tag);

  return thread;
}

static void make_main_thread(void)
{
  main_thread = running_thread();
  init_thread(main_thread, "main", 31);

  deque_push_back(&thread_all_deque, &main_thread->all_deque_tag);
}

void schedule(void)
{
  struct task_struct * cur = running_thread();
  if (cur->status == TASK_RUNNING) {
    deque_push_back(&thread_ready_deque, &cur->general_tag);
    cur->ticks = cur->priority;
    cur->status = TASK_READY;
  } else {
    // nothing
  }
  ASSERT(!deque_empty(&thread_ready_deque));
  thread_tag = NULL;
  thread_tag = deque_pop_front(&thread_ready_deque);
  struct task_struct * next = NODE_ENTRY(struct task_struct,
      general_tag, thread_tag);
  kputs("[DEBUG] Schedule: 0x");
  kputuint(next->stack_magic, 16);
  kputs("\n");
  next->status = TASK_RUNNING;
  switch_to(cur, next);
}

void thread_sys_init(void)
{
  kputs("[INFO] Thread init start\n");
  deque_init(&thread_ready_deque);
  deque_init(&thread_all_deque);
  make_main_thread();
  kputs("[INFO] Thread init done\n");
}
