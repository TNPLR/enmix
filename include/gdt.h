#ifndef TSS_H_
#define TSS_H_
#include <stdint.h>
#define TSS_ATTR_LOW 0x89
#define TSS_ATTR_HIGH 0x0
#define USER_CODE_ATTR_LOW 0xFA
#define USER_CODE_ATTR_HIGH 0x2F
#define USER_DATA_ATTR_LOW 0xF2
#define USER_DATA_ATTR_HIGH 0x20
struct desc_register {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));
struct bit64_desc {
  uint16_t limit_low16;
  uint16_t base_low16;
  uint8_t base_mid8;
  uint8_t attribute_low8;
  uint8_t limit_attr_high4;
  uint8_t base_high8;
} __attribute__((packed));
struct bit128_desc {
  uint16_t limit_low16;
  uint16_t base_low16;
  uint8_t base_mid8;
  uint8_t attribute_low8;
  uint8_t limit_attr_high4;
  uint8_t base_high8;
  uint32_t base_high32;
  uint32_t reserved;
} __attribute((packed));
struct tss_struct {
  uint32_t resv0; // reserved set to 0
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t resv1; // set to 0
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t resv2; // set to 0
  uint16_t resv3; // set to 0
  uint16_t io_map_base_addr;
} __attribute__((packed));
#endif /* TSS_H_ */
