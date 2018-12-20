#ifndef E820_H_
#define E820_H_
#include <stdint.h>
#define ARDS_POS (uintptr_t)0x200
uint64_t get_ram(void);
uint64_t convert_ram_kib(uint64_t byte);
uint64_t convert_ram_mib(uint64_t byte);
uint64_t convert_ram_gib(uint64_t byte);
#endif
