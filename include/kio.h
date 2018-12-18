#ifndef KIO_H_
#define KIO_H_
#include <stdint.h>
extern void set_kputc(void (*)(char const));
void kputs(char const *s);
void (*kputc)(char const c);
void kputuint(uintmax_t num, unsigned base);
#endif
