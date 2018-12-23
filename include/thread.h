#ifndef THREAD_H_
#define THREAD_H_
#include <stdint.h>
enum task_status {
  TASK_RUNNING,
  TASK_READY,
  TASK_BLOCKED,
  TASK_WAITING,
  TASK_HANGING,
  TASK_DIED
};

struct interrupt_stack {
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbp;
  // We don't push rsp
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rbx;
  uint64_t rax;

  uint64_t error_code;
  void (*rip)(void);
  uint64_t cs;
  uint64_t eflags;
  void * rsp;
  uint64_t ss;
}__attribute__((aligned(8)));

struct thread_stack {
  // sysV abi says that callee should protect these registers
  // but we won't protect rsp
  uint64_t rbx;
  uint64_t rbp;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  void (*func)(void *); // call function (rdi)
  void *args; // ptr to args (rsi)

  void (*rip)(void (*)(void *), void * args);
  void (*reserved_retaddr); // no use pointer
}__attribute__((aligned(8)));

struct task_struct {
  uint64_t * task_stack;
  enum task_status status;
  uint8_t priority;
  char name[16];
  uint64_t stack_magic; // check stack limit
}__attribute__((aligned(8)));

void thread_create(struct task_struct *, void (*)(void *), void *);
void init_thread(struct task_struct *, const char *, int);
struct task_struct * thread_start(const char *, int, void (*)(void *), void *);
#endif
