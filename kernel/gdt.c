#include "gdt.h"
#include "string.h"
#include <stdint.h>

static struct tss tss;

static struct bit64_desc gen64desc(void * desc_addr, uint32_t limit,
    uint8_t attr_low, uint8_t attr_high) {
  uint32_t desc_base = (uint32_t)desc_addr;
  struct bit64_desc desc;
  desc.limit_low16 = (uint16_t)(limit & 0xFFFF);
  desc.base_low16 = (uint16_t)(desc_base & 0xFFFF);
  desc.base_mid8 = (uint8_t)((desc_base >> 16) & 0xFF);
  desc.attribute_low8 = attr_low;
  desc.limit_attr_high4 =
    ((uint8_t)((limit >> 16) & 0xF) | (attr_high));
  desc.base_high8 = (uint8_t)(desc_base >> 24);
  return desc;
}

static struct bit128_desc gen128desc(void * desc_addr, uint32_t limit,
    uint8_t attr_low, uint8_t attr_high) {
  uint64_t desc_base = (uint64_t)desc_addr;
  struct bit64_desc desc;
  desc.limit_low16 = (uint16_t)(limit & 0xFFFF);
  desc.base_low16 = (uint16_t)(desc_base & 0xFFFF);
  desc.base_mid8 = (uint8_t)((desc_base >> 16) & 0xFF);
  desc.attribute_low8 = attr_low;
  desc.limit_attr_high4 =
    ((uint8_t)((limit >> 16) & 0xF) | (attr_high));
  desc.base_high8 = (uint8_t)((desc_base >> 24) & 0xFF);
  desc.base_high32 = (uint32_t)((desc_base >> 32) & 0xFFFFFFFFU);
  desc.reserved = 0;
  return desc;
}

void gdt_tss_init(void)
{
  kputs("[INFO] GDT TSS INIT\n");
  uint32_t tss_size = sizeof(tss)  
  memset(&tss, 0, tss_size);
  tss.rsp0 = 0;
  tss.io_base = tss_size;
  *(struct bit128_desc *)0x940 = bit128_desc(&tss, tss_size-1,
      TSS_ATTR_LOW, TSS_ATTR_HIGH); // 0x910 (gdt addr) + 0x30 (8 * 6)
  *(struct bit64_desc *)0x950 = bit64_desc((void *)0, (void *) 0,
      USER_CODE_ATTR_LOW, USER_CODE_ATTR_HIGH); // 0x910 (gdt addr) + 0x40 (8 * 6 + 16)
  *(struct bit64_desc *)0x958 = bit64_desc((void *)0, (void *) 0,
      USER_DATA_ATTR_LOW, USER_DATA_ATTR_HIGH); // 0x910 (gdt addr) + 0x48 (8 * 6 + 16 + 8 * 1)
  struct desc_register = {.limit = 8*6+16+8*1-1, .base = 0x910};
  asm volatile("lgdt %0"::"m"(desc_register));
  asm volatile("ltr %w0"::"r"(0x30));
}

void tss_desc_init(void)
{
}
