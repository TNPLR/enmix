#ifndef BITMAP_H_
#define BITMAP_H_
struct bitmap {
  uint64_t btmp_bytes_len;
  uint8_t *bits;
};
void bitmap_init(struct bitmap *btmp);

#endif
