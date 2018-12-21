#include "bitmap.h"
#include "string.h"
#include <stdint.h>
// Need to rewrite

void bitmap_init(struct bitmap *btmp) {
  memset(btmp->bits, 0, btmp->btmp_bytes_len);
}

int bitmap_test(struct bitmap * btmp, uint64_t bit_idx)
{
  uint64_t byte_idx = bit_idx / 8;
  uint64_t bit = bit_idx % 8;
  return (btmp->bits[byte_idx] & (1 << bit));
}

int bitmap_scan(struct bitmap * btmp, uint64_t cnt)
{
  uint64_t idx_byte = 0;
  while ((0xFF == btmp->bits[idx_byte]) && (idx_byte < btmp->btmp_bytes_len)) {
    ++idx_byte;
  }
  if (idx_byte == btmp->btmp_bytes_len) {
    return -1;
  }
  int idx_bit = 0;
  while ((uint8_t)(1 << idx_bit) & btmp->bits[idx_byte]) {
    ++idx_bit;
  }
  int bit_idx_start = idx_byte * 8 + idx_bit;
  if (cnt == 1) {
    return bit_idx_start;
  }

  uint64_t bit_left = (btmp->btmp_bytes_len * 8 - bit_idx_start);
  uint64_t next_bit = bit_idx_start + 1;
  uint64_t count = 1;

  bit_idx_start = -1;
  while (bit_left-- > 0) {
    if (!(bitmap_test(btmp, next_bit))) {
      ++count;
    } else {
      count = 0;
    }
    if (count == cnt) {
      bit_idx_start = next_bit - cnt + 1;
      break;
    }
    ++next_bit;
  }
  return bit_idx_start;
}

void bitmap_set(struct bitmap * btmp, uint64_t bit_idx, int8_t value)
{
  uint64_t byte_idx = bit_idx / 8;
  uint64_t bit = bit_idx % 8;
  if (value) {
    btmp ->bits[byte_idx] |= (1 << bit);
  } else {
    btmp->bits[byte_idx] &= ~(1 << bit);
  }
}
