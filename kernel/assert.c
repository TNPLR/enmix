#include "assert.h"
#include "kio.h"
#include "interrupt.h"

void error_wall(const char *filename,
    int line, const char *func, const char* condition)
{
  disable_interrupt();
  kputs("**************************************************\n");
  kputs("[EMERG] ASSERTION MASSAGE\n");
  kputs("**************************************************\n");
  kputs("FILE: ");
  kputs(filename);
  kputs("\nLINE: ");
  kputuint(line, 10);
  kputs("\nFUNCTION: ");
  kputs(func);
  kputs("CONDITION: ");
  kputs(condition);
  kputs("\n**************************************************\n");
  kputs("[EMERG] ASSERTION MASSAGE END\n");
  kputs("**************************************************\n");
}
