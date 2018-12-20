#include "e820.h"
#include <stdint.h>
uint64_t get_ram(void)
{
  uint32_t *ptr = (uint32_t *)(ARDS_POS+0x8); // first ards
  uint64_t ret = 0;
  for (int i = 0; i < *(uint16_t *)(ARDS_POS); ++i) {
    if (*(ptr+0x4) == 1) {
      uint64_t tmp = *(uint64_t *)ptr;
      tmp += *(uint64_t *)(ptr+2);
      if (tmp > ret) {
        ret = tmp;
      }
    }
    ptr += 5;
  }
  return ret;
}
uint64_t convert_ram_gib(uint64_t byte)
{
  return byte / 1073741824U;
}
uint64_t convert_ram_mib(uint64_t byte)
{
  return byte / 1048576U;
}
uint64_t convert_ram_kib(uint64_t byte)
{
  return byte / 1024U;
}
