#ifndef BITMAP_H_
#define BITMAP_H_
#include <stdint.h>
struct bitmap {
  uint64_t btmp_bytes_len;
  uint8_t *bits;
};
void bitmap_init(struct bitmap * btmp);
int bitmap_test(struct bitmap * btmp, uint64_t bit_idx);
int bitmap_scan(struct bitmap * btmp, uint64_t cnt);
void bitmap_set(struct bitmap * btmp, uint64_t bit_idx, int8_t value);
#endif
