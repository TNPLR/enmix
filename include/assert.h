#ifndef ASSERT_H_
#define ASSERT_H_
void error_wall(const char *filename,
    int line, const char *func, const char* condition);
#define ERROR_WALL(...) error_wall(__FILE__, __LINE__, __func__, __VA_ARGS__)
#if NDEBUG
#define ASSERT(x) ((void *)0)
#else
#define ASSERT(x) \
  if (x); else ERROR_WALL(#x)
#endif /* NDEBUG */
#endif
