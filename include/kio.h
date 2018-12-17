#ifndef KIO_H_
#define KIO_H_
extern void set_kputc(void (*)(char const));
void kputs(char const *s);
void (*kputc)(char const c);
#endif
